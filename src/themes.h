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

$Id: themes.h 248 2008-11-11 17:33:08Z vinduv $
*/

#ifndef _THEMES_H
#define _THEMES_H 1

#include <SDL_ttf.h>

#include "audio.h"

void init_th_stage1(void);
void init_th_stage2(void);
void theme_suivant(void);
void libere_themes(void);
void affiche_fond(SDL_Surface* surface_ecran);
void fond_partiel(SDL_Surface* surface_ecran, const Uint16 x, const Uint16 y, const Uint16 w, const Uint16 h);
void place_rects(void);

/* Fonctions privées */
void _charge_commun(void);
void _demarre_switcheur(void);
void _arret_switcheur(void);
bool _charge_theme_act(char* theme_select);
void _theme_suivant(bool initial);

char nom_theme[32];
char theme_act[32];

int menu_decalage, menu_t_police, menu_ecart;

SDL_Color coul_txt_menu, coul_txt_dial, coul_txt_jeu, coul_filet, coul_sol, coul_fond;

SDL_Surface *sol, *filet, *balle_img, *fleche, *oeil, *icone_jg, *icone_jd, *icone_fen;

SDL_Surface **img_grand_jg, **img_grand_jd, **img_2J_jg, **img_2J_jd, **img_3J_jg, **img_3J_jd;
Uint8 img_max_jg, img_max_jd;

SDL_Surface *fond, *fond_jeu;

TTF_Font* police; /* La police de caractères */
TTF_Font* police_menu;

Uint16 decalage_haut, decalage_gauche;
float ratio_police;

bool th_libere;

char* chemin_moi;

#endif
