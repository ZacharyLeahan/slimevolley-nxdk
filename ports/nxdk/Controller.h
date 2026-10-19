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
static inline int sv_pad_recovery(const SvPadState *p) {
    return (p->buttons&((1u<<6)|(1u<<7)))==((1u<<6)|(1u<<7));
}
#endif
