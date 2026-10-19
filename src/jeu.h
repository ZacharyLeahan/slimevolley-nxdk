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

$Id: jeu.h 247 2008-11-11 13:02:23Z mcmic $
*/

#ifndef _JEU_H
#define _JEU_H 1

#include <SDL.h>

#define HAUT_SOL 20

#define FILET_GAUCHE 397
#define FILET_DROITE 403
#define FILET_HAUT 70 /* hauteur du filet */

Uint32 delai_image(Uint32 debut, Uint32 maintenant);

void jeu_srv(void);
void jeu_clt(void);

#endif
