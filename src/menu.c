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

$Id: menu.c 242 2008-11-08 13:40:13Z vinduv $
*/

#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "slime.h"
#include "menu.h"
#include "themes.h"
#include "audio.h"

#define TEXTE_GAUCHE 170

static void menu_interne(menu_elem* fonctions, Uint8 n, char* nom_retour, void (*sortie)(void)) {
	Uint8 i;

	bool cont = true;
	bool maj_requise = false;

	SDL_Rect pos1, pos2, pos_o1, pos_o2; /* Les positions des slimes et de leurs yeux */

	pos1.x = 45;
	pos2.x = 655;
	pos_o1.x = 109 + 4;
	pos_o2.x = 681 - 4;

	decalage = 32767;

	menu_act = 0;

	while(cont) {
		if(decalage == 32767) { /* Le décalage n'a pas été réglé */
			TTF_SizeUTF8(police_menu, "Hello World !", NULL, &decalage);
			if(img_grand_jg[1]->h < decalage) {
				decalage = (decalage - img_grand_jg[1]->h) / 2;
			} else {
				decalage = (img_grand_jg[1]->h - decalage) / 2;
			}
		}

		if(menu_act < 0) menu_act = n;
		if(menu_act > n) menu_act = 0;

		SDL_BlitSurface(fond, NULL, ecran, NULL);

		/* On affiche les éléments de menu... */
		for(i = 0 ; i < n ; i++) {
			menu_raf = i;
			afficher(fonctions[i].aff_fnct == NULL ? _(fonctions[i].aff_txt) : fonctions[i].aff_fnct(),
				police_menu, coul_txt_menu, TEXTE_GAUCHE, menu_decalage + menu_ecart * i);

			if(i == menu_act && fonctions[i].aide_fnct != NULL) {
				aff_aide(fonctions[i].aide_fnct());
			} else if(i == menu_act && fonctions[i].aide_txt != NULL) {
				aff_aide(_(fonctions[i].aide_txt));
			}
		}

		/* et l'élément Retour */
		afficher(_(nom_retour), police_menu, coul_txt_menu, TEXTE_GAUCHE, menu_decalage + menu_ecart * n);

		pos2.y = pos1.y = menu_decalage + menu_ecart * menu_act + decalage;
		pos_o1.y = pos_o2.y = pos1.y + 19;

		SDL_BlitSurface(img_grand_jg[1], NULL, ecran, &pos1);
		SDL_BlitSurface(img_grand_jd[1], NULL, ecran, &pos2);
		SDL_BlitSurface(oeil, NULL, ecran, &pos_o1);
		SDL_BlitSurface(oeil, NULL, ecran, &pos_o2);

		SDL_Flip(ecran);

		do {
			maj_requise = false;
			SDL_WaitEvent(&evenement);

			do {
				switch(evenement.type) {
					case SDL_QUIT:
						maj_requise = true;
						cont = false;
					break;
					case SDL_KEYDOWN:
						switch(evenement.key.keysym.sym) {
							case SDLK_ESCAPE:
								maj_requise = true;
								if(sortie) sortie(); else cont = false;
							break;

							case SDLK_UP:
								maj_requise = true;
								menu_act--;
							break;

							case SDLK_DOWN:
								maj_requise = true;
								menu_act++;
							break;

							case SDLK_LEFT:
								if(menu_act != n && fonctions[menu_act].accepte_gd) {
									maj_requise = true;
									action = -1;
									cont = fonctions[menu_act].cont_menu;
									fonctions[menu_act].action();
								}
							break;

							case SDLK_RETURN:
								maj_requise = true;
								joue_son(SON_RBD_SLIME);
								if(menu_act == n) {
									if(sortie) sortie(); else cont = false;
								} else {
									action = 0;
									cont = fonctions[menu_act].cont_menu;
									fonctions[menu_act].action();
								}
							break;

							case SDLK_RIGHT:
								if(menu_act != n && fonctions[menu_act].accepte_gd) {
									maj_requise = true;
									action = 1;
									cont = fonctions[menu_act].cont_menu;
									fonctions[menu_act].action();
								}
							break;

							default:
							break;
						}
					break;
				
					case SDL_KEYUP:
						if(evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
							SDL_WM_IconifyWindow();
						}
					break;

					default:
					break;
				}
			} while(SDL_PollEvent(&evenement));

		} while(!maj_requise);
	}

	return;
}

void affiche_menu(menu_elem* fonctions, Uint8 n, char* nom_retour) {
    menu_interne(fonctions,n,nom_retour,NULL);
}
#ifdef SV_XBOX
void affiche_menu_systeme(menu_elem* fonctions, Uint8 n) {
    menu_interne(fonctions,n,"Exit / System...",sv_system_menu);
}
#endif
