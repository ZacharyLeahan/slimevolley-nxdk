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

$Id: credits.c 296 2010-07-06 17:34:19Z vinduv $
*/

#include <stdio.h>
#include <math.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "slime.h"
#include "themes.h"

#define NUM_LIGNES 19

const char* credits_texte[NUM_LIGNES] = {
	"Slime Volley",
	"version 2.4.2",
	"",
	"Développement et graphismes",
	"Côme “MCMic” Bernigaud",
	"Vincent “VinDuv” Duvert",
	"",
	"Contributeurs (thèmes)",
	"Côme Bernigaud",
	"(thèmes Classic, Modern et Arash)",
	"Luc D. (thème Arash)",
	"Benoît “OrkSovaj” Taine (thème Modern)",
	"Vincent Duvert (thème Modern)",
	"",
	"Contributeurs (Slimes supplémentaires)",
	"Antoine Bernigaud",
	"Aratz",
	"Côme Bernigaud",
	"diodio13fr",
};

SDL_Surface* sf_lignes[NUM_LIGNES];
SDL_Surface *degrade_haut;
SDL_Surface *degrade_bas;
SDL_Rect r_haut = { 0, 0, LARG_ECRAN, 128 };
SDL_Rect r_bas = { 0, 0, LARG_ECRAN, 128 };
Uint16 h_texte;

bool defile(void) {
	Uint8 i;

	float decalage_init;

	Uint16 h_max;
	float pos = 0;
	SDL_Rect src_rect = { 0, 0, LARG_ECRAN, 0 };
	SDL_Rect dst_rect = { 0, 0, 0, 0 };

	Uint32 temps_prec;

	h_max = NUM_LIGNES * h_texte;

	decalage_init = h_ecran - menu_decalage; /* La première ligne doit rentrer en entier */

	temps_prec = SDL_GetTicks();
	while(pos < h_max) {
		SDL_BlitSurface(fond, NULL, ecran, NULL);

		/* Affichage (incomplet) du texte du haut */
		i = rint(pos) / h_texte;

		src_rect.y = (Uint16)rint(pos) % h_texte;
		src_rect.h = h_texte;
		dst_rect.y = menu_decalage + (Uint16)decalage_init;

		if(sf_lignes[i] != NULL) {
			dst_rect.x = (LARG_ECRAN - sf_lignes[i]->w) / 2;

			SDL_BlitSurface(sf_lignes[i], &src_rect, ecran, &dst_rect);
		}

		/* Affichage des textes du dessous, tant qu'il y a de la place */
		dst_rect.y -= src_rect.y;
		for(;;) {
			i++;
			dst_rect.y += h_texte;

			if(dst_rect.y > h_ecran - h_texte || i >= NUM_LIGNES) break;

			if(sf_lignes[i] == NULL) continue;
			dst_rect.x = (LARG_ECRAN - sf_lignes[i]->w) / 2;

			SDL_BlitSurface(sf_lignes[i], NULL, ecran, &dst_rect);
		}

		/* Affichage du texte en bas de l'écran */
		if(i < NUM_LIGNES && sf_lignes[i] != NULL) {
			src_rect.y = 0;
			src_rect.h = h_ecran - dst_rect.y;
			dst_rect.x = (LARG_ECRAN - sf_lignes[i]->w) / 2;
			SDL_BlitSurface(sf_lignes[i], &src_rect, ecran, &dst_rect);
		}

		SDL_BlitSurface(degrade_haut, NULL, ecran, &r_haut);
		SDL_BlitSurface(degrade_bas, NULL, ecran, &r_bas);

		SDL_Flip(ecran);

		if(SDL_PollEvent(&evenement)) {
			if(evenement.type == SDL_QUIT) {
				return true;
			} else if(evenement.type == SDL_KEYDOWN) {
				switch(evenement.key.keysym.sym) {
					case SDLK_ESCAPE:
					case SDLK_SPACE:
					case SDLK_RETURN:
						return false;
					break;
					default:
					break;
				}
			} else if(evenement.type == SDL_KEYUP && evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
				SDL_WM_IconifyWindow();
			}
		}

		if(decalage_init > 0) {
			decalage_init -= ((float)(SDL_GetTicks() - temps_prec) / 30.);
		} else {
			pos += ((float)(SDL_GetTicks() - temps_prec) / 30.);
		}

		temps_prec = SDL_GetTicks();
	}

	return false;
}

void def_alpha(SDL_Surface* surface, Uint16 y, Uint8 alpha) {
	Uint8 r, g, b, a;
	Uint8 *p, *p_f;

	p = ((Uint8 *)surface->pixels) + y * surface->pitch;
	p_f = ((Uint8 *)surface->pixels) + y * surface->pitch + surface->w * surface->format->BytesPerPixel;

	SDL_LockSurface(surface);
	while(p < p_f) {
		SDL_GetRGBA(*(Uint32 *)p, surface->format, &r, &g, &b, &a);
		*(Uint32 *)p = SDL_MapRGBA(surface->format, r, g, b, alpha);
		p += surface->format->BytesPerPixel;
	}
	SDL_UnlockSurface(surface);
}

void init_degrades(void) {
	Uint16 i;
	SDL_Surface* temp;

	r_haut.y = menu_decalage - 1;

	temp = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, r_haut.w, r_haut.h, 32, 0, 0, 0, 0);
	degrade_haut = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

	SDL_BlitSurface(fond, &r_haut, degrade_haut, NULL);

	temp = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, r_bas.w, r_bas.h, 32, 0, 0, 0, 0);
	degrade_bas = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

	SDL_BlitSurface(fond, &r_bas, degrade_bas, NULL);

	for(i = 0 ; i < r_haut.h ; i++) {
		def_alpha(degrade_haut, i, 255 - i*255/r_haut.h);
		def_alpha(degrade_bas, i, i*255/r_haut.h);
	}

}

void aff_credits(void) {
	Uint8 i;
	bool ret;

	r_bas.y = h_ecran - 128;

	init_degrades();

	h_texte = TTF_FontLineSkip(police_menu);

	for(i = 0 ; i < NUM_LIGNES ; i++) {
		sf_lignes[i] = TTF_RenderUTF8_Blended(police_menu, credits_texte[i], coul_txt_menu);
	}

	ret = defile();

	for(i = 0 ; i < NUM_LIGNES ; i++) {
		SDL_FreeSurface(sf_lignes[i]);
	}

	SDL_FreeSurface(degrade_haut);
	SDL_FreeSurface(degrade_bas);

	if(ret) exit(EXIT_SUCCESS);
}
