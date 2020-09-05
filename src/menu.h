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

$Id: menu.h 270 2008-12-13 21:46:13Z mcmic $
*/

#ifndef _MENU_H
#define _MENU_H 1

#include <stdbool.h>
#include <SDL_types.h>

typedef struct menu_elem {
	char* (*aff_fnct)(void); /* Pointeur vers la fonction d'affichage */
	char* aff_txt; /* Si NULL, on utilise cette chaîne statique */

	char* (*aide_fnct)(void); /* Pointeur vers la fonction d'aide */
	char* aide_txt; /* Idem */

	void (*action)(void); /* Pointeur vers la fonction appelée en cas d'action sur l'élément */

	bool accepte_gd;
	/* Indique si la fonction d'action doit être appelée par appui sur les touches fléchées */
	bool cont_menu; /* Indique si le menu doit être fermé après l'exécution de l'action */
} menu_elem;

void affiche_menu(menu_elem* fonctions, Uint8 n, char* nom_retour);

int decalage; /* Pour aligner les slimes par rapport au texte */

Sint8 menu_act; /* L'ÈlÈment de menu actuellement choisi */
Uint8 menu_raf; /* L'ÈlÈment de menu actuellement rafraichi */
Sint8 action; /* Indique la touche pressÈe par l'utilisateur (gauche/retour/droite) */

#endif
