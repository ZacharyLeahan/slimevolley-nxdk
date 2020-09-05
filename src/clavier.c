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

$Id: clavier.c 226 2008-08-25 19:20:38Z vinduv $
*/

#include <stdbool.h>

#include <SDL_image.h>

#include "slime.h"
#include "clavier.h"
#include "themes.h"

#define POS_HAUT 150
#define POS_BAS 360
#define POS_GAUCHE 60
#define POS_DROITE 420
#define INTERVALLE 40

void touches_vers_tab(Uint8 num_conf, SDLKey* tableau, Uint8 decalage) {
	tableau[decalage]     = touches[num_conf].haut;
	tableau[decalage + 1] = touches[num_conf].bas;
	tableau[decalage + 2] = touches[num_conf].gauche;
	tableau[decalage + 3] = touches[num_conf].droite;

	return;
}

void tab_vers_touches(Uint8 num_conf, SDLKey* tableau, Uint8 decalage) {
	touches[num_conf].haut		= tableau[decalage];
	touches[num_conf].bas		= tableau[decalage + 1];
	touches[num_conf].gauche	= tableau[decalage + 2];
	touches[num_conf].droite	= tableau[decalage + 3];

	return;
}

void configClavier(void) {
	bool continuer = true;

	const char* noms_touches[4] = { N_("Up"), N_("Down"), N_("Left"), N_("Right") };

	SDLKey touches_conf[4*4];
	SDL_Rect rect_curs;

	char a_afficher[40] = "\0";

	Uint8 i, pos_col;
	int pos_curseur = 0;
	bool edition = false;

	for(i = 0 ; i < 4 ; i++) {
		touches_vers_tab(i, touches_conf, 4 * i);
	}

	while(continuer) {
		SDL_WaitEvent(&evenement);
		switch(evenement.type) {
			case SDL_QUIT:
				continuer = false;
				quitter = true;
			break;
			case SDL_KEYDOWN:
				if(edition && evenement.key.keysym.sym != SDLK_ESCAPE) {
					for(i = 0 ; i < 16 ; i++) {
						if(touches_conf[i] == evenement.key.keysym.sym) touches_conf[i] = 0;
					}
					touches_conf[pos_curseur] = evenement.key.keysym.sym;
					edition = false;
				} else {
					switch(evenement.key.keysym.sym) {
						case SDLK_ESCAPE:
							continuer = false;
						break;

						case SDLK_UP:
							if(pos_curseur <= 0) {
								pos_curseur = 15;
							} else {
								pos_curseur--;
							}
						break;

						case SDLK_DOWN:
							if(pos_curseur >= 15) {
								pos_curseur = 0;
							} else {
								pos_curseur++;
							}
						break;

						case SDLK_LEFT:
						case SDLK_RIGHT:
							pos_curseur += (pos_curseur < 8 ? 8 : -8);
						break;

						case SDLK_RETURN:
							edition = true;
						break;

						default:
						break;
					}
				}
			break;
			default:
			break;
		}

		SDL_BlitSurface(fond, NULL, ecran, NULL);
		afficher(_("Keys set A:"), police, coul_txt_dial, POS_GAUCHE, POS_HAUT);
		afficher(_("Keys set B:"), police, coul_txt_dial, POS_GAUCHE, POS_BAS);
		afficher(_("Keys set C:"), police, coul_txt_dial, POS_DROITE, POS_HAUT);
		afficher(_("Keys set D:"), police, coul_txt_dial, POS_DROITE, POS_BAS);

		for(i = 0 ; i < 16 ; i++) {
			pos_col = i % 8; /* La position du curseur dans la colonne */

			if(pos_curseur == i) {
				rect_curs.x = (i < 8 ? POS_GAUCHE : POS_DROITE) - 20;
				rect_curs.y = (i % 8 < 4 ? POS_HAUT : POS_BAS) + INTERVALLE * (i % 4 + 1) + 5;
				SDL_BlitSurface(balle_img, NULL, ecran, &rect_curs);
			}

			if(pos_curseur == i && edition) {
				snprintf4(a_afficher, 40, _("%s: Press any key..."), _(noms_touches[i % 4]));
			} else if(touches_conf[i] == SDLK_UNKNOWN) {
				snprintf4(a_afficher, 40, _("%s: Unassigned"), _(noms_touches[i % 4]));
			} else {
				snprintf5(a_afficher, 40, _("%s: %s"), _(noms_touches[i % 4]), _(SDL_GetKeyName(touches_conf[i])));
			}

			afficher(a_afficher, police, coul_txt_dial, (i < 8 ? POS_GAUCHE : POS_DROITE), (i % 8 < 4 ? POS_HAUT : POS_BAS) + INTERVALLE * (i % 4 + 1));

		}
		SDL_Flip(ecran);
	}

	for(i = 0 ; i < 4 ; i++) {
		tab_vers_touches(i, touches_conf, 4 * i);
	}

	return;
}

