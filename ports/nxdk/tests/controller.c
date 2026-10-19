#include "../Controller.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    SvPadState p={0};
    assert(sv_pad_mask(&p)==0); /* initial trigger values must not move */
    p.axes[2]=p.axes[5]=-32768;assert(sv_pad_mask(&p)==0);
    for(int stick=0;stick<2;stick++) {
        int x=stick?3:0,y=stick?4:1;
        p.axes[x]=-12000;assert(sv_pad_mask(&p)==0);
        p.axes[x]=-12001;assert(sv_pad_mask(&p)==SV_LEFT);
        p.axes[x]=12001;assert(sv_pad_mask(&p)==SV_RIGHT);p.axes[x]=0;
        p.axes[y]=-12001;assert(sv_pad_mask(&p)==SV_UP);
        p.axes[y]=12001;assert(sv_pad_mask(&p)==SV_DOWN);p.axes[y]=0;
    }
    p.axes[2]=1;assert(sv_pad_mask(&p)==SV_LEFT);
    p.axes[5]=32767;assert(sv_pad_mask(&p)==0); /* both triggers */
    p.axes[2]=-32768;assert(sv_pad_mask(&p)==SV_RIGHT);p.axes[5]=-32768;
    p.hat=SDL_HAT_LEFT;p.axes[3]=32767;assert(sv_pad_mask(&p)==0);
    p.axes[3]=-32768;assert(sv_pad_mask(&p)==SV_LEFT);
    p.hat=0;assert(sv_pad_mask(&p)==SV_LEFT); /* release one duplicate */
    p.axes[3]=0;assert(sv_pad_mask(&p)==0);
    p.hat=SDL_HAT_UP;p.axes[4]=32767;assert(sv_pad_mask(&p)==0);
    p.axes[4]=0;p.hat=SDL_HAT_RIGHTUP;assert(sv_pad_mask(&p)==(SV_UP|SV_RIGHT));p.hat=0;
    p.buttons=(1u<<0)|(1u<<2);assert(sv_pad_mask(&p)==(SV_A|SV_X));
    p.buttons=(1u<<1)|(1u<<7);assert(!sv_pad_recovery(&p));
    assert(sv_pad_mask(&p)==(SV_BACK|SV_START));
    p.buttons=(1u<<6);assert(!sv_pad_recovery(&p));
    p.buttons|=1u<<7;assert(sv_pad_recovery(&p));
    memset(&p,0,sizeof(p));assert(sv_pad_mask(&p)==0 && !sv_pad_recovery(&p));
    puts("PASS: both sticks, triggers, deadzones, duplicate/opposing inputs, buttons and recovery chord");
}
