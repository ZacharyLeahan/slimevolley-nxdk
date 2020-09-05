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

$Id: objet.h 270 2008-12-13 21:46:13Z mcmic $
*/

#ifndef _OBJET_H
#define _OBJET_H 1

#include <stdbool.h>
#include <SDL.h>

typedef enum {
	GAUCHE,
	DROITE
} direction;

typedef enum {
	DESACTIVE = 0,
	CLAVIER = 1,
	INT_ART = 2,
	DISTANT = 3
} j_type;

typedef enum {
	NOOB = 0,
	BEGINNER,
	GOOD,
	GOD,
	NB_NIVEAUX
} j_niveau;

typedef struct balle_obj {
	Sint16 vx, vy; /* La vitesse */
	float x, y; /* La position */
	float p_x, p_y; /* La position précédente */
} balle_obj;

typedef struct joueur {
	Sint16 vx, vy; /* La vitesse */
	float x, y; /* La position */
	float p_x, p_y; /* La position précédente */

	Uint8 taille; /* Hauteur, largeur sur deux. */

	Uint8 skin_num;
	SDL_Surface** skin_tab;
	Uint32 dern_chg_coul;

	direction sens;

	Uint16 lim_bas, lim_gauche, lim_droite;
	Uint16 oeil_x, oeil_y;

	Uint8 conf_clavier;

	j_niveau niveau; /* Utilisé dans le cas d'une IA, les niveaux sont définis dans niv_ai */

	bool t_haut, t_bas, t_gauche, t_droite;

	j_type type;
} joueur;

void deplace_joueur(Uint8 id, const Uint16 dt);
void deplace_balle(Uint8 id, const Uint16 dt);

SDL_Surface* echelle_surface(SDL_Surface* orig, float ratio);
SDL_Surface** reduire_skins(SDL_Surface** grand_skin, Uint8 img_max, float ratio); /* change la taille des skin */
#endif
