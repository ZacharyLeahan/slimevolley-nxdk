/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef SV_CONTROLLER_H
#define SV_CONTROLLER_H
#include <SDL.h>
enum { SV_UP=1, SV_DOWN=2, SV_LEFT=4, SV_RIGHT=8,
       SV_A=16, SV_BACK=32, SV_START=64, SV_X=128 };
typedef struct { Sint16 axes[6]; Uint8 hat; unsigned buttons; } SvPadState;
/* NXDK SDL axes: LX, LY, LT, RX, RY, RT. Triggers range -32768..32767;
 * zero can also be the initial value before the first trigger report. */
static inline unsigned sv_pad_mask(const SvPadState *p) {
    unsigned b=0;
    if((p->hat&SDL_HAT_UP)||p->axes[1]<-12000||p->axes[4]<-12000) b|=SV_UP;
    if((p->hat&SDL_HAT_DOWN)||p->axes[1]>12000||p->axes[4]>12000) b|=SV_DOWN;
    if((p->hat&SDL_HAT_LEFT)||p->axes[0]<-12000||p->axes[3]<-12000||p->axes[2]>0) b|=SV_LEFT;
    if((p->hat&SDL_HAT_RIGHT)||p->axes[0]>12000||p->axes[3]>12000||p->axes[5]>0) b|=SV_RIGHT;
    if(p->buttons&(1u<<0)) b|=SV_A;
    if(p->buttons&((1u<<1)|(1u<<6))) b|=SV_BACK;
    if(p->buttons&(1u<<7)) b|=SV_START;
    if(p->buttons&(1u<<2)) b|=SV_X;
    /* Opposing directions cancel, regardless of which control supplied them. */
    if((b&(SV_LEFT|SV_RIGHT))==(SV_LEFT|SV_RIGHT)) b&=~(SV_LEFT|SV_RIGHT);
    if((b&(SV_UP|SV_DOWN))==(SV_UP|SV_DOWN)) b&=~(SV_UP|SV_DOWN);
    return b;
}
/* Remember each source separately so a new press counts even when another
 * control already holds the same direction. Exact simultaneous opposites
 * preserve the previous winner, or remain neutral without one. */
typedef struct { unsigned sources[4]; unsigned horizontal, vertical; } SvPadHistory;
static inline unsigned sv_pad_resolve(const SvPadState *p, SvPadHistory *h) {
    unsigned sources[4]={0}, fresh[4], b=sv_pad_mask(p);
    const unsigned hats[4]={SDL_HAT_UP,SDL_HAT_DOWN,SDL_HAT_LEFT,SDL_HAT_RIGHT};
    for(int d=0;d<4;d++) {
        if(p->hat&hats[d]) sources[d]|=1;
        int axis=d<2?1:0;
        for(int stick=0;stick<2;stick++) {
            Sint16 value=p->axes[axis+stick*3];
            if((d==0||d==2)?value<-12000:value>12000) sources[d]|=2u<<stick;
        }
    }
    if(p->axes[2]>0) sources[2]|=8;
    if(p->axes[5]>0) sources[3]|=8;
    for(int d=0;d<4;d++) {fresh[d]=sources[d]&~h->sources[d];h->sources[d]=sources[d];}
    b&=~(SV_UP|SV_DOWN|SV_LEFT|SV_RIGHT);
    for(int pair=0;pair<2;pair++) {
        int a=pair*2,c=a+1;
        unsigned *winner=pair?&h->horizontal:&h->vertical;
        if(!sources[a]&&!sources[c]) *winner=0;
        else if(!sources[c]) *winner=1u<<a;
        else if(!sources[a]) *winner=1u<<c;
        else if(!!fresh[a]!=!!fresh[c]) *winner=fresh[a]?1u<<a:1u<<c;
        b|=*winner;
    }
    return b;
}
static inline int sv_pad_recovery(const SvPadState *p) {
    return (p->buttons&((1u<<6)|(1u<<7)))==((1u<<6)|(1u<<7));
}
#endif
