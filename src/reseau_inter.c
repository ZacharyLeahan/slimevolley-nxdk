/*
This file is part of Slime Volley.

	Slime Volley is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Slime Volley is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Slime Volley. If not, see <http://www.gnu.org/licenses/>.

Copyright (c)MCMic, VinDuv.

$Id: reseau_inter.c 236 2008-11-01 09:45:35Z vinduv $
*/

/* Interfaces graphiques utilisées pour divers aspects réseau. */

#include <math.h>
#include <SDL.h>

#include "config.h"
#include "reseau_inter.h"
#include "slime.h"
#include "themes.h"

#ifdef NET_SUPPORT

#define MARGE_GAUCHE 50
#define MARGE_CENTRE 20
#define MARGE_DROITE 50

#define TOUR_MS 1000 /* Nombre de milisecondes pour que l'indicateur d'attente fasse un tour */
#define CURS_E_MS 200 /* Temps d'extinction du curseur en ms */
#define CURS_A_MS 800 /* Temps d'allumage du curseur en ms */

 /* Nombre de caractères maximal dans le champ de saisie */

/* Dialogue d'attente */
SDL_Rect raf_rect = { 0, 0, 20, 20 };
SDL_Rect slime_rect;

void prep_attente(const char* texte, const char* ligne2, const char* ligne3) {
	/* Prépare l'affichage du dialogue d'attente. */
	SDL_Rect dessin_rect;
	SDL_Surface* texte_rendu;

	SDL_BlitSurface(fond, NULL, ecran, NULL);

	/* Affichage du texte */
	texte_rendu = TTF_RenderUTF8_Blended(police_menu, texte, coul_txt_dial);
	dessin_rect.x = MARGE_GAUCHE + img_grand_jg[1]->w + MARGE_CENTRE;
	dessin_rect.y = (h_ecran - texte_rendu->h) / 2;
	SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);
	dessin_rect.y += texte_rendu->h;
	SDL_FreeSurface(texte_rendu);

	if(ligne2 != NULL) {
		texte_rendu = TTF_RenderUTF8_Blended(police, ligne2, coul_txt_dial);
		SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);
		dessin_rect.y += texte_rendu->h;
		SDL_FreeSurface(texte_rendu);

		if(ligne3 != NULL) {
			texte_rendu = TTF_RenderUTF8_Blended(police, ligne3, coul_txt_dial);
			SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);
			SDL_FreeSurface(texte_rendu);
		}
	}

	raf_rect.x = MARGE_GAUCHE + 59;
	raf_rect.y = (h_ecran - img_grand_jg[1]->h) / 2 + 14;

	slime_rect.x = MARGE_GAUCHE;
	slime_rect.y = (h_ecran - img_grand_jg[1]->h) / 2;
	slime_rect.w = img_grand_jg[1]->w;
	slime_rect.h = img_grand_jg[1]->h;
}

void attente(void) {
	SDL_Rect dessin_rect;
	float angle;

	/* Rafraichissement du fond sous l'oeil de la slime (nécessaire si il est transparent) */
	SDL_BlitSurface(fond, &raf_rect, ecran, &raf_rect);

	/* Affichage de la slime (taille par défaut) à gauche. */
	SDL_BlitSurface(img_grand_jg[1], NULL, ecran, &slime_rect);

	angle = ((float)(SDL_GetTicks() % TOUR_MS) / (float)TOUR_MS) * 2. * M_PI;

	dessin_rect.x = slime_rect.x + 69 - 5 - 4.0 * sin(angle);
	dessin_rect.y = slime_rect.y + 24 - 5 + 4.0 * cos(angle);

	SDL_BlitSurface(oeil, NULL, ecran, &dessin_rect);

	SDL_Flip(ecran);
}

void message_texte(const char* texte, const char* ligne2, const char* ligne3) {
	SDL_Rect dessin_rect;
	SDL_Surface* texte_rendu;

	SDL_BlitSurface(fond, NULL, ecran, NULL);

	/* Affichage de la slime (taille par défaut) à gauche. */
	dessin_rect.x = MARGE_GAUCHE;
	dessin_rect.y = (h_ecran - img_grand_jg[1]->h) / 2;

	SDL_BlitSurface(img_grand_jg[1], NULL, ecran, &dessin_rect);

	dessin_rect.x += 69 - 5;
	dessin_rect.y += 24 - 5;

	SDL_BlitSurface(oeil, NULL, ecran, &dessin_rect);

	/* Affichage du texte */
	texte_rendu = TTF_RenderUTF8_Blended(police_menu, texte, coul_txt_dial);
	dessin_rect.x = MARGE_GAUCHE + img_grand_jg[1]->w + MARGE_CENTRE;
	dessin_rect.y = (h_ecran - texte_rendu->h) / 2;
	SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);
	dessin_rect.y += texte_rendu->h;
	SDL_FreeSurface(texte_rendu);

	if(ligne2 != NULL) {
		texte_rendu = TTF_RenderUTF8_Blended(police, ligne2, coul_txt_dial);
		SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);
		dessin_rect.y += texte_rendu->h;
		SDL_FreeSurface(texte_rendu);

		if(ligne3 != NULL) {
			texte_rendu = TTF_RenderUTF8_Blended(police, ligne3, coul_txt_dial);
			SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);
			SDL_FreeSurface(texte_rendu);
		}
	}

	SDL_Flip(ecran);
}

bool prompt_ip(const char* info_txt, char* entree) {
	SDL_Rect dessin_rect;
	SDL_Surface* texte_rendu;
	const SDL_Color coul_texte = {0xFF, 0xFF, 0xFF, 0};

	int haut_texte;

	SDL_Rect rect_cadre_int, rect_texte;

	Uint8 pos;

	char code_touche;

	SDL_TimerID cont_timer;

	pos = strlen(entree);

	SDL_BlitSurface(fond, NULL, ecran, NULL);

	/* Affichage de la slime (taille par défaut) à gauche. */
	dessin_rect.x = MARGE_GAUCHE;
	dessin_rect.y = (h_ecran - img_grand_jg[1]->h) / 2;
	SDL_BlitSurface(img_grand_jg[1], NULL, ecran, &dessin_rect);

	dessin_rect.x += 69 - 5;
	dessin_rect.y += 24 - 5;
	SDL_BlitSurface(oeil, NULL, ecran, &dessin_rect);

	/* Affichage du texte */
	texte_rendu = TTF_RenderUTF8_Blended(police_menu, info_txt, coul_txt_dial);
	dessin_rect.x = MARGE_GAUCHE + img_grand_jg[1]->w + MARGE_CENTRE;
	dessin_rect.y = (h_ecran - texte_rendu->h) / 2;
	SDL_BlitSurface(texte_rendu, NULL, ecran, &dessin_rect);


	/* Calcul des dimensions des cadres */
	dessin_rect.x = MARGE_GAUCHE;
	dessin_rect.y += texte_rendu->h + 10; /* Utilisé pour le cadre extérieur */
	rect_cadre_int.x = dessin_rect.x + 1;
	rect_cadre_int.y = dessin_rect.y + 1;
	rect_texte.x = rect_cadre_int.x + 1;
	rect_texte.y = rect_cadre_int.y + 1;

	dessin_rect.w = LARG_ECRAN - MARGE_GAUCHE - MARGE_DROITE;
	rect_cadre_int.w = dessin_rect.w - 2;
	rect_texte.w = rect_cadre_int.w - 2;

	TTF_SizeUTF8(police, "tp", NULL, &haut_texte);

	rect_texte.h = haut_texte;
	rect_cadre_int.h = rect_texte.h + 2;
	dessin_rect.h = rect_cadre_int.h + 2;

	/*
	dessin_rect.h = haut_texte;
	rect_cadre_int.h = dessin_rect.h - 2;
	rect_texte.h = rect_cadre_int.h - 2;*/

	SDL_FreeSurface(texte_rendu);

	/* Dessin du cadre externe */
	SDL_FillRect(ecran, &dessin_rect, SDL_MapRGB(ecran->format, 0xFF, 0xFF, 0xFF));

	/* Préconfiguration du curseur */
	dessin_rect.y = rect_texte.y;
	dessin_rect.h = rect_texte.h;
	dessin_rect.w = dessin_rect.h / 2;

	SDL_EnableUNICODE(true);

	SDL_InitSubSystem(SDL_INIT_TIMER);
	cont_timer = SDL_AddTimer(200, ev_timer, NULL);

	for(;;) {
		texte_rendu = TTF_RenderUTF8_Blended(police, entree, coul_texte);

		/* Dessin du cadre interne */
		SDL_FillRect(ecran, &rect_cadre_int, SDL_MapRGB(ecran->format, 0x00, 0x00, 0x00));

		/* Dessin du curseur */
		if(SDL_GetTicks() % (CURS_E_MS + CURS_A_MS) > CURS_E_MS) {
			dessin_rect.x = rect_cadre_int.x + (texte_rendu == NULL ? 0 : texte_rendu->w) + 1;
			SDL_FillRect(ecran, &dessin_rect, SDL_MapRGB(ecran->format, 0xFF, 0xFF, 0xFF));
		}

		/* Dessin du texte */
		SDL_BlitSurface(texte_rendu, NULL, ecran, &rect_texte);

		SDL_FreeSurface(texte_rendu);

		SDL_Flip(ecran);

		SDL_WaitEvent(&evenement);

		switch(evenement.type) {
			case SDL_QUIT:
				SDL_RemoveTimer(cont_timer);
				exit(EXIT_SUCCESS);
			break;
			case SDL_KEYDOWN:
				switch(evenement.key.keysym.sym) {
					case SDLK_ESCAPE:
						SDL_RemoveTimer(cont_timer);
						SDL_QuitSubSystem(SDL_INIT_TIMER);
						return false;
					break;

					case SDLK_BACKSPACE:
						if(pos > 0) {
							entree[pos - 1] = (char)0;
							pos--;
						}
					break;

					case SDLK_RETURN:
						if(strlen(entree) >= 1) {
							SDL_RemoveTimer(cont_timer);
							SDL_QuitSubSystem(SDL_INIT_TIMER);
							return true;
						}
					break;

					default:
						if(evenement.key.keysym.unicode < 127 && pos < MAX_CARACT) {
							code_touche = evenement.key.keysym.unicode;
							if(code_touche == 45 								/* - */
								|| code_touche == 46							/* . */
								|| (48 <= code_touche && code_touche <= 57)		/* 0123...9 */
								|| (65 <= code_touche && code_touche <= 90)		/* ABCD...Z */
								|| code_touche == 99							/* _ */
								|| (97 <= code_touche && code_touche <= 122)) {	/* abcd...z */
								entree[pos] = (char)code_touche;
								entree[pos + 1] = (char)0;
								pos++;
							}
						}
					break;
				}
			break;
			default:
			break;
		}
	}
}

void delai_controlle(Uint32 temps) {
	SDL_TimerID cont_timer;
	while(SDL_PollEvent(&evenement)) {}

	temps += SDL_GetTicks();

	SDL_InitSubSystem(SDL_INIT_TIMER);
	cont_timer = SDL_AddTimer(200, ev_timer, NULL);

	for(;;) {
		SDL_WaitEvent(&evenement);

		if(evenement.type == SDL_QUIT) {
			SDL_RemoveTimer(cont_timer);
			exit(EXIT_SUCCESS);
		} else if(evenement.type == SDL_KEYDOWN || SDL_GetTicks() >= temps) {
			SDL_RemoveTimer(cont_timer);
			return;
		}
	}
}

#endif
