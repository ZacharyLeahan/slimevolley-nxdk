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

$Id: clavier.h 133 2008-03-27 17:30:09Z vinduv $
*/

#ifndef _CLAVIER_H
#define _CLAVIER_H 1

/* Gestion du clavier */

typedef struct touches_joueur {
	SDLKey haut, bas, gauche, droite;
} touches_joueur;

void configClavier(void);

#endif
