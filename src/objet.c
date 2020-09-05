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

$Id: objet.c 270 2008-12-13 21:46:13Z mcmic $
*/

#include <SDL.h>
#include <math.h>
#include "slime.h"
#include "objet.h"

void deplace_joueur(Uint8 id, const Uint16 dt) {
/* Fonction basée sur certains cours de physique de terminale ;)    *
 * Elle permet de déplacer un objet en fonction du temps.           *
 * Attention, plus dt est grand plus l'approximation est mauvaise ! */

	if(dt > 250) return; /* Gros ralentissement (FPS < 4), on annule le déplacement */

	tab_joueurs[id].vy += conf.gravite * dt/1000;

	/* Mise à jour de la position... */
	tab_joueurs[id].x += (float)tab_joueurs[id].vx * dt/1000;
	tab_joueurs[id].y += (float)tab_joueurs[id].vy * dt/1000;

	/* On vérifie que le joueur ne sort pas du terrain. */
	if(tab_joueurs[id].x <= tab_joueurs[id].lim_gauche) {
		tab_joueurs[id].x = tab_joueurs[id].lim_gauche;
		tab_joueurs[id].vx = 0;
	} else if(tab_joueurs[id].x >= tab_joueurs[id].lim_droite) {
		tab_joueurs[id].x = tab_joueurs[id].lim_droite;
		tab_joueurs[id].vx = 0;
	}

	if(tab_joueurs[id].y >= tab_joueurs[id].lim_bas) {
    	tab_joueurs[id].y = tab_joueurs[id].lim_bas;
    	tab_joueurs[id].vx = 0;
    	tab_joueurs[id].vy = 0;
    }
}

void deplace_balle(Uint8 id, const Uint16 dt) {
	/* Equivalent strict de la fonction deplace_joueur() */
	if(dt > 250) return; /* Gros ralentissement (FPS < 4), on annule le déplacement */

	tab_balles[id].vy += conf.gravite * dt/1000;

	/* Mise à jour de la position... */
	tab_balles[id].x += (float)tab_balles[id].vx * dt/1000;
	tab_balles[id].y += (float)tab_balles[id].vy * dt/1000;
}

