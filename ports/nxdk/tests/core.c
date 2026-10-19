/* Exercise actual upstream game initialization and audio conversion/callbacks. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slime.h"
#include "themes.h"
#include "audio.h"
#include "jeu.h"
void init_jeu(void);
void mixeur(void *,Uint8 *,int);
extern Uint8 num_gauche,num_droite;
extern SDL_AudioSpec format_obtenu;
extern slime_son *son_act;
extern Uint32 son_pos;
#ifdef SV_XBOX
void sv_game_controls(void) {}
#endif
static void players(void) {
    SDL_Surface full={0},small={0};full.w=100;full.h=100;small.w=80;small.h=80;
    SDL_Surface *large[]={&full,&full},*scaled[]={&small,&small};
    img_grand_jg=img_grand_jd=large;img_2J_jg=img_2J_jd=scaled;
    h_ecran=600;tab_joueurs[0].type=CLAVIER;tab_joueurs[NB_JOUEURS_G].type=CLAVIER;
    init_jeu();assert(num_gauche==1&&num_droite==1);
    assert(tab_joueurs[NB_JOUEURS_G].taille==100);
    tab_joueurs[1].type=INT_ART;tab_joueurs[NB_JOUEURS_G+1].type=CLAVIER;
    init_jeu();assert(num_gauche==2&&num_droite==2);
    assert(tab_joueurs[1].taille==80&&tab_joueurs[NB_JOUEURS_G+1].taille==80);
#ifdef SV_XBOX
    assert(NB_JOUEURS_T==4&&NB_JOUEURS_G==2);
#else
    assert(NB_JOUEURS_T==6&&NB_JOUEURS_G==3);
#endif
}
static void audio(void) {
    Uint8 data[]={1,2,3,4},out[16];slime_son sound={data,4,0};
    memset(out,0x7f,sizeof(out));format_obtenu.silence=0;son_act=&sound;son_pos=0;
    mixeur(NULL,out,sizeof(out));assert(memcmp(data,out,4)==0);
    for(int i=4;i<16;i++) assert(out[i]==0);
    assert(son_act==NULL&&son_pos==4);
    memset(out,0x7f,sizeof(out));mixeur(NULL,out,sizeof(out));
    for(int i=0;i<16;i++) assert(out[i]==0);
    SDL_AudioSpec source={0};source.freq=22050;source.format=AUDIO_S16SYS;source.channels=1;
    format_obtenu.freq=44100;format_obtenu.format=AUDIO_S16SYS;format_obtenu.channels=2;
    sons[0].longueur=2048;sons[0].donnees=calloc(1,sons[0].longueur);audio_desact=false;
    conversion_son(source,0);assert(sons[0].longueur==8192);free(sons[0].donnees);sons[0].donnees=NULL;
}
static void timing(void) {
    const Uint32 budget = 2000/MAX_FPS;
    assert(delai_image(1000,1000)==budget);
    assert(delai_image(1000,1000+budget-1)==1);
    assert(delai_image(1000,1000+budget)==0);
    assert(delai_image(1000,1000+budget+1)==0);
    assert(delai_image(1000,61000)==0);
    /* SDL's millisecond clock wraps after approximately 49 days. */
    assert(delai_image(UINT32_MAX-9,10)==budget-20);
    for(Uint32 elapsed=0;elapsed<100000;elapsed++)
        assert(delai_image(1000,1000+elapsed)<=budget);
}
#ifdef SV_XBOX
extern Sint8 conf_clav_act[4];
void change_conf(Uint8 id);
static void player_menu(void) {
    memset(tab_joueurs,0,sizeof(tab_joueurs));
    memset(conf_clav_act,-1,sizeof(Sint8)*4);
    /* Mandatory players must offer every AI level before cycling to port 1. */
    for(int side=0;side<2;side++) {
        int id=side*NB_JOUEURS_G;
        tab_joueurs[id].type=INT_ART;tab_joueurs[id].niveau=0;
        for(int level=1;level<NB_NIVEAUX;level++) {
            change_conf(id);
            assert(tab_joueurs[id].type==INT_ART && tab_joueurs[id].niveau==level);
        }
        change_conf(id);assert(tab_joueurs[id].type==CLAVIER);
        /* Cycling through the available ports must restart AI at level 0. */
        while(tab_joueurs[id].type==CLAVIER) change_conf(id);
        assert(tab_joueurs[id].type==INT_ART && tab_joueurs[id].niveau==0);
    }
    /* An optional teammate still cycles through levels and can be disabled. */
    tab_joueurs[1].type=INT_ART;tab_joueurs[1].niveau=0;
    for(int level=1;level<NB_NIVEAUX;level++) {
        change_conf(1);assert(tab_joueurs[1].niveau==level);
    }
    change_conf(1);assert(tab_joueurs[1].type==DESACTIVE);
    puts("PASS: Xbox player-menu controller/AI cycles");
}
#endif
int main(void) {players();audio();timing();
#ifdef SV_XBOX
player_menu();
#endif
puts("PASS: player/team bounds, skin selection, audio silence and converted length, frame-delay boundaries");return 0;}
