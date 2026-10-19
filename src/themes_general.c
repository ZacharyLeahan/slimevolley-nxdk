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

$Id: themes_general.c 251 2008-11-11 20:26:43Z vinduv $
*/

#include <SDL.h>
#include <stdbool.h>

#include "slime.h"
#include "jeu.h"
#include "themes.h"
#include "echelle.h"
#include "audio.h"

#define RATIO_2J .8
#define RATIO_3J .6
#define RATIO_IC .4

SDL_Rect filet_rect = { FILET_GAUCHE, HAUT_ECRAN - HAUT_SOL - FILET_HAUT, 0, 0 };
SDL_Rect sol_rect = { 0, HAUT_ECRAN - HAUT_SOL, 0, 0 };
SDL_Rect fond_rect;

void _reduire_th_act(void) {
	SDL_Rect oeil_droit = { 27, 8, 3, 3 };
	SDL_Rect oeil_gauche = { 11, 8, 3, 3 };

	/* L'image 0 est celle de l'IA. L'image 1, celle du joueur par défaut */

	icone_jg = echelle_surface(img_grand_jg[1], RATIO_IC);
	SDL_FillRect(icone_jg, &oeil_droit, SDL_MapRGB(icone_jg->format, 0x00, 0x00, 0x00));
	img_2J_jg[0] = echelle_surface(img_grand_jg[0], RATIO_2J);
	img_2J_jg[1] = echelle_surface(img_grand_jg[1], RATIO_2J);
	img_3J_jg[0] = echelle_surface(img_grand_jg[0], RATIO_3J);
	img_3J_jg[1] = echelle_surface(img_grand_jg[1], RATIO_3J);

	icone_jd = echelle_surface(img_grand_jd[1], RATIO_IC);
	SDL_FillRect(icone_jd, &oeil_gauche, SDL_MapRGB(icone_jd->format, 0x00, 0x00, 0x00));
	img_2J_jd[0] = echelle_surface(img_grand_jd[0], RATIO_2J);
	img_2J_jd[1] = echelle_surface(img_grand_jd[1], RATIO_2J);
	img_3J_jd[0] = echelle_surface(img_grand_jd[0], RATIO_3J);
	img_3J_jd[1] = echelle_surface(img_grand_jd[1], RATIO_3J);
	
	if(fond_jeu == NULL) { /* Thème classique : on initialise les images du sol et du filet */
		filet = SDL_CreateRGBSurface(SDL_HWSURFACE, FILET_DROITE-FILET_GAUCHE, FILET_HAUT, 32, 0, 0, 0, 0);
		SDL_FillRect(filet, NULL, SDL_MapRGB(filet->format, coul_filet.r, coul_filet.g, coul_filet.b));

		sol = SDL_CreateRGBSurface(SDL_HWSURFACE, LARG_ECRAN, HAUT_SOL, 32, 0, 0, 0, 0);
		SDL_FillRect(sol, NULL, SDL_MapRGB(sol->format, coul_sol.r, coul_sol.g, coul_sol.b));
	}
}

void init_th_stage1(void) {
	icone_fen = NULL;

	_charge_commun(); /* On charge les données communes aux thèmes (les slimes) */
	_demarre_switcheur(); /* Permet au gestionnaire sous-jacent d'initialiser son switcheur de thèmes */

	if(!_charge_theme_act(theme_act)) {
		_theme_suivant(true);
	}

	th_libere = false;
}

void init_th_stage2(void) {
	Uint8 i;

	/* A ce stade, toutes les ressources ont été chargées. */
	img_2J_jg = (SDL_Surface**)malloc((img_max_jg + 1) * sizeof(SDL_Surface*));
	img_3J_jg = (SDL_Surface**)malloc((img_max_jg + 1) * sizeof(SDL_Surface*));
	for(i = 2 ; i <= img_max_jg ; i++) {
		img_2J_jg[i] = echelle_surface(img_grand_jg[i], RATIO_2J);
		img_3J_jg[i] = echelle_surface(img_grand_jg[i], RATIO_3J);
	}

	img_2J_jd = (SDL_Surface**)malloc((img_max_jd + 1) * sizeof(SDL_Surface*));
	img_3J_jd = (SDL_Surface**)malloc((img_max_jd + 1) * sizeof(SDL_Surface*));
	for(i = 2 ; i <= img_max_jd ; i++) {
		img_2J_jd[i] = echelle_surface(img_grand_jd[i], RATIO_2J);
		img_3J_jd[i] = echelle_surface(img_grand_jd[i], RATIO_3J);
	}
	_reduire_th_act();
}

void _libere_th_act(void) {
	Uint8 i;

	if(th_libere) return;

	if(fond_jeu == NULL) { /* Thème classique */
		SDL_FreeSurface(filet);
		SDL_FreeSurface(sol);
	} else {
		SDL_FreeSurface(fond_jeu);
	}

	SDL_FreeSurface(fond);

	SDL_FreeSurface(icone_jg);
	SDL_FreeSurface(img_grand_jg[0]);
	SDL_FreeSurface(img_grand_jg[1]);
	SDL_FreeSurface(img_2J_jg[0]);
	SDL_FreeSurface(img_2J_jg[1]);
	SDL_FreeSurface(img_3J_jg[0]);
	SDL_FreeSurface(img_3J_jg[1]);

	SDL_FreeSurface(icone_jd);
	SDL_FreeSurface(img_grand_jd[0]);
	SDL_FreeSurface(img_grand_jd[1]);
	SDL_FreeSurface(img_2J_jd[0]);
	SDL_FreeSurface(img_2J_jd[1]);
	SDL_FreeSurface(img_3J_jd[0]);
	SDL_FreeSurface(img_3J_jd[1]);

	SDL_FreeSurface(balle_img);
	SDL_FreeSurface(fleche);
	SDL_FreeSurface(oeil);

	TTF_CloseFont(police);
	TTF_CloseFont(police_menu);

	th_libere = true;

	if(audio_desact) return;

	for(i = 0 ; i < NB_SONS ; i++) {
		if(sons[i].longueur != 0) {
			free(sons[i].donnees);
			sons[i].longueur = 0;
		}
	}
}

void theme_suivant(void) {
	stop_son();
	_libere_th_act();
	_theme_suivant(false);
	th_libere = false;
	_reduire_th_act();
}

void libere_themes(void) {
	Uint8 i;

	for(i = 2 ; i <= img_max_jg ; i++) {
		SDL_FreeSurface(img_grand_jg[i]);
		SDL_FreeSurface(img_2J_jg[i]);
		SDL_FreeSurface(img_3J_jg[i]);
	}

	for(i = 2 ; i <= img_max_jd ; i++) {
		SDL_FreeSurface(img_grand_jd[i]);
		SDL_FreeSurface(img_2J_jd[i]);
		SDL_FreeSurface(img_3J_jd[i]);
	}

	_libere_th_act();

	free(img_grand_jg);
	free(img_2J_jg);
	free(img_3J_jg);

	free(img_grand_jd);
	free(img_2J_jd);
	free(img_3J_jd);

	_arret_switcheur();
}

void affiche_fond(SDL_Surface* surface_ecran) {
	fond_rect.y = -decalage_haut;

	if(fond_jeu == NULL) {
		SDL_FillRect(surface_ecran, NULL, SDL_MapRGB(ecran->format, coul_fond.r, coul_fond.g, coul_fond.b));
		SDL_BlitSurface(sol, NULL, ecran, &sol_rect);
		SDL_BlitSurface(filet, NULL, ecran, &filet_rect);
	} else {
		SDL_BlitSurface(fond_jeu, NULL, surface_ecran, &fond_rect);
	}
}

void fond_partiel(SDL_Surface* surface_ecran, const Uint16 x, const Uint16 y, const Uint16 w, const Uint16 h) {
	SDL_Rect src_rect, dst_rect;

	dst_rect.x = src_rect.x = x;
	dst_rect.y = src_rect.y = y;
	dst_rect.w = src_rect.w = w;
	dst_rect.h = src_rect.h = h;

	src_rect.y += decalage_haut;

	if(fond_jeu == NULL) {
		SDL_FillRect(surface_ecran, &dst_rect, SDL_MapRGB(ecran->format, coul_fond.r, coul_fond.g, coul_fond.b));
	} else {
		SDL_BlitSurface(fond_jeu, &src_rect, surface_ecran, &dst_rect);
	}
}

void place_rects(void) {
	filet_rect.y = h_ecran - HAUT_SOL - FILET_HAUT;
	sol_rect.y = h_ecran - HAUT_SOL;
	fond_rect.y = -decalage_haut;
}
