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

$Id: audio.h 272 2008-12-23 08:44:22Z vinduv $
*/

#ifndef _AUDIO_H
#define _AUDIO_H 1

#include <stdbool.h>

typedef struct slime_son {
	Uint8 *donnees; /* Les données de son */
	Uint32 longueur; /* La longueur du son */
	Uint32 pos; /* La position actuelle si le son est en lecture */
} slime_son;

void init_son(void);

void conversion_son(SDL_AudioSpec format_son, Uint8 id);

void libere_sons(void);

void joue_son(Sint8 id);

void stop_son(void);

bool audio_desact;
bool son_active;

enum {
	SON_RBD_SLIME	= 0,
	SON_RBD_MUR		= 1,
	SON_RBD_FILET	= 2,
	SON_JG_MARQUE	= 3,
	SON_JD_MARQUE	= 4,
	SON_JG_GAGNE	= 5,
	SON_JD_GAGNE	= 6,
	NB_SONS			= 7
};

slime_son sons[NB_SONS];

#endif
