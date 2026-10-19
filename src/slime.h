/*
This file is part of Slime Volley.

	Slime Volley is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Slime Volley is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Slime Volley.  If not, see <http://www.gnu.org/licenses/>.

Copyright (c) MCMic, VinDuv.

$Id: slime.h 236 2008-11-01 09:45:35Z vinduv $
*/

#ifndef _SLIME_H
#define _SLIME_H 1

#define MAX_FPS 50 /* On limite à 50 images par seconde */

#define LARG_ECRAN 800
#define HAUT_ECRAN 600

#define NUM_CONFIGS 4

#ifdef SV_XBOX
#define NB_JOUEURS_G 2
#else
#define NB_JOUEURS_G 3
#endif
#define NB_JOUEURS_T (2 * NB_JOUEURS_G)

#define MAX_BALLES 8

#include <SDL_ttf.h>
#include <stdbool.h>
#include "clavier.h"
#include "objet.h"
#include "config.h"

#ifdef I10N
	#define _(x) gettext(x)
	#include <libintl.h>
	#include <locale.h>
#else
	#define _(x) (x)
#endif
#define N_(x) (x)

#ifdef MACOSX
	#include <CGSPrivate.h>
	CGSTransitionType anim_suiv;
	CGSTransitionOption sens_suiv;
	#define prep_anim(x, y) anim_suiv = x; sens_suiv = y;
	#define SDL_Flip(x) if(anim_suiv != CGSNone & !plein_ecran) { animWindow(anim_suiv, sens_suiv); anim_suiv = CGSNone; } else { SDL_Flip(x); }
#else
	#define prep_anim(x, y)
#endif

#ifdef MACOS9
	#define M_PI	3.14159265358979323846264338327950288
	#define snprintf4(a, b, c, d) sprintf(a, c, d)
	#define snprintf5(a, b, c, d, e) sprintf(a, c, d, e)
#else
	#define snprintf4(a, b, c, d) snprintf(a, b, c, d)
	#define snprintf5(a, b, c, d, e) snprintf(a, b, c, d, e)
#endif

SDL_Event evenement;

void afficher(char* chaine, TTF_Font* police_texte, SDL_Color coul_text, Uint16 posX, Uint16 posY);
void aff_aide(const char* texte);

typedef struct config_slime {
	float saut_vy, saut_g_vx, saut_d_vx;
	float dep_x;
	float v_balle_rbd;
	float gravite;
	float coefficient;
} config_slime;

config_slime* creer_config(const float saut_vy, const float saut_g_vx, const float saut_d_vx, const float dep_x, const float v_ball_rbd, const float gravite, const float coefficient);

/* Les variables globales nécessaires... */

SDL_Surface *ecran; /* L'écran */

joueur tab_joueurs[NB_JOUEURS_T]; /* Ce tableau contient les configuration des joueurs */
touches_joueur touches[4];

Uint8 nb_balles;
balle_obj tab_balles[MAX_BALLES];

config_slime conf; /* La configuration actuelle (modifiable) */

Uint8 act_conf;

Uint32 ev_timer(Uint32 intervalle, void* param);

Uint16 h_ecran, l_ecran;

bool quitter;
bool fen_ok;
bool plein_ecran, aff_fps, aff_sc_perm;
#endif
