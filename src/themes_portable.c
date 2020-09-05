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

$Id: themes_portable.c 257 2008-11-23 10:03:21Z vinduv $
*/

#include <stdio.h>
#include <unistd.h>

#include <SDL_ttf.h>
#include <SDL_image.h>

#include "slime.h"
#include "audio.h"
#include "themes.h"

Uint8 long_chemin_base, long_chemin_ext, long_chemin_thm;

SDL_RWops* _charge_ressource(FILE* fichier, char** donnees) {
	Uint32 taille;

	/* On suppose qu'au départ on est à la fin du fichier */
	fseek(fichier, -4, SEEK_CUR);

	if(fread(&taille, 4, 1, fichier) != 1) {
		fprintf(stderr, "_charge_ressource: ");
		perror("fread");
		exit(EXIT_FAILURE);
	}

	fseek(fichier, -taille - 4, SEEK_CUR);

	if(*donnees != NULL) free(*donnees);
	*donnees = (char*)malloc(taille * sizeof(char));
	if(*donnees == NULL) {
		fprintf(stderr, "_charge_ressource: mémoire insuffisante.");
		exit(EXIT_FAILURE);
	}

	if(fread(*donnees, taille, 1, fichier) != 1) {
		fprintf(stderr, "_charge_ressource: ");
		perror("fread");
		exit(EXIT_FAILURE);
	}

	fseek(fichier, -taille, SEEK_CUR);

	return SDL_RWFromMem(*donnees, taille);
}

void _charge_son(Uint8 id_son, SDL_RWops** rw_ops) {
	SDL_AudioSpec format_son;

	if(audio_desact) return;

	if(SDL_LoadWAV_RW(*rw_ops, true, &format_son, &sons[id_son].donnees, &sons[id_son].longueur) == NULL) {
		fprintf(stderr, "Error loading sound: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	conversion_son(format_son, id_son);
}

void _charge_commun(void) {
	FILE* moi;
	char* tampon = NULL;
	char* tampon_polices = NULL; /* Bogue de SDL_TTF, il faut conserver ce tampon */
	SDL_RWops* rwop;
	SDL_RWops* rwop2; /* Bogue de SDL_TTF, il faut recopier la RWops pour l'utiliser 2 fois */

	strncpy(nom_theme, "[Portable]", sizeof(nom_theme));
	strncpy(theme_act, "default", sizeof(theme_act));

	img_grand_jg = malloc(sizeof(SDL_Surface*) * 2);
	img_2J_jg = malloc(sizeof(SDL_Surface*) * 2);
	img_3J_jg = malloc(sizeof(SDL_Surface*) * 2);
	img_grand_jd = malloc(sizeof(SDL_Surface*) * 2);
	img_2J_jd = malloc(sizeof(SDL_Surface*) * 2);
	img_3J_jd = malloc(sizeof(SDL_Surface*) * 2);
	img_max_jg = img_max_jd = 1;

	moi = fopen(chemin_moi, "rb");
	if(moi == NULL) {
		perror("fopen(me)");
		exit(EXIT_FAILURE);
	}

	fseek(moi, -3 * 2 - 3 * sizeof(SDL_Color), SEEK_END);

	fread(&coul_txt_menu, sizeof(SDL_Color), 1, moi);
	fread(&coul_txt_dial, sizeof(SDL_Color), 1, moi);
	fread(&coul_txt_jeu, sizeof(SDL_Color), 1, moi);
	fread(&menu_decalage, 2, 1, moi);
	fread(&menu_t_police, 2, 1, moi);
	fread(&menu_ecart, 2, 1, moi);

	fseek(moi, -3 * 2 - 3 * sizeof(SDL_Color), SEEK_END);

	rwop = _charge_ressource(moi, &tampon); /* s_slime.wav */
	_charge_son(SON_RBD_SLIME, &rwop);
	rwop = _charge_ressource(moi, &tampon); /* s_mur.wav */
	_charge_son(SON_RBD_MUR, &rwop);
	rwop = _charge_ressource(moi, &tampon); /* s_jg_m.wav */
	_charge_son(SON_JG_MARQUE, &rwop);
	rwop = _charge_ressource(moi, &tampon); /* s_filet.wav */
	_charge_son(SON_RBD_FILET, &rwop);

	rwop = _charge_ressource(moi, &tampon_polices); /* police.ttf */
	rwop2 = (SDL_RWops*)malloc(sizeof(SDL_RWops));
	memcpy(rwop2, rwop, sizeof(SDL_RWops));
	police_menu = TTF_OpenFontRW(rwop, false, menu_t_police * ratio_police);
	police = TTF_OpenFontRW(rwop2, false, 25);

	rwop = _charge_ressource(moi, &tampon); /* slimeG.png */
	img_grand_jg[1] = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* slimeD.png */
	img_grand_jd[1] = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* slimeIAG.png */
	img_grand_jg[0] = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* slimeIAD.png */
	img_grand_jd[0] = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* oeil.png */
	oeil = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* menu.png */
	fond = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* jeu.png */
	fond_jeu = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* fleche.png */
	fleche = IMG_Load_RW(rwop, true);

	rwop = _charge_ressource(moi, &tampon); /* balle.png */
	balle_img = IMG_Load_RW(rwop, true);

	free(tampon);
	fclose(moi);

	menu_ecart *= ratio_police;

}

void _demarre_switcheur(void) {
	return;
}

void _arret_switcheur(void) {
	return;
}

void _theme_suivant(bool initial) {
	if(!initial) {
		/* Ne devrait pas se produire ! */
		fprintf(stderr, "BUG\n");
		exit(EXIT_FAILURE);
	}

	return;
}

bool _charge_theme_act(char* theme_select) {
	(void)theme_select;
	return true;
}
