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

$Id: slime.c 257 2008-11-23 10:03:21Z vinduv $
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "config.h"
#include "slime.h"
#include "menu_princ.h"
#include "audio.h"
#include "themes.h"
#include "reseau.h"

#include "credits.h"

#ifdef WINDOWS
	#define CONFIG_NOM "slime.ini"
	#define SEP "\\"
	#define DOSS_PREFS "USERPROFILE"
#else
	#ifdef MACOS9
		#define CONFIG_NOM "Slime Volley Prefs"
	#else
		#define CONFIG_NOM ".slime"
		#define SEP "/"
		#define DOSS_PREFS "HOME"
	#endif
#endif

char conf_chemin[64] = CONFIG_NOM;

void afficher(char* chaine, TTF_Font* police_texte, SDL_Color coul_texte, Uint16 posX, Uint16 posY) {
	SDL_Surface* texte;
	SDL_Rect position;

	texte = TTF_RenderUTF8_Blended(police_texte, chaine, coul_texte);
	position.x = posX;
	position.y = posY;
	position.w = 0;
	position.h = 0;

	SDL_BlitSurface(texte, NULL, ecran, &position);

	SDL_FreeSurface(texte);
}

void aff_aide(const char* texte) {
	/* Formatte et affiche le texte d'aide (prise en compte des \n) */
	SDL_Surface* ligne;
	char *chaine, *ch_ptr;
	SDL_Rect position = { 0, 0, 0, 0 };

	position.y = h_ecran;

	chaine = (char*)malloc(strlen(texte) + 1);
	strcpy(chaine, texte);

	ch_ptr = chaine + strlen(chaine);

	for(;;) {
		ch_ptr--;
		if(ch_ptr == chaine) {
			ligne = TTF_RenderUTF8_Blended(police, ch_ptr, coul_txt_menu);
			position.x = (LARG_ECRAN - ligne->w)/2;
			position.y -= ligne->h;
			SDL_BlitSurface(ligne, NULL, ecran, &position);
			SDL_FreeSurface(ligne);
			break;
		} else if(*ch_ptr == '\n') {
			*ch_ptr = 0;

			ligne = TTF_RenderUTF8_Blended(police, ch_ptr + 1, coul_txt_menu);
			position.x = (LARG_ECRAN - ligne->w)/2;
			position.y -= ligne->h;
			SDL_BlitSurface(ligne, NULL, ecran, &position);
			SDL_FreeSurface(ligne);
		}
	}

	free(chaine);
}

/* Fonctions de préréglage des touches du clavier. */
bool non_assignee(SDLKey touche) {
	Uint8 i;

	for(i = 0 ; i < 4 ; i++) {
		if(touches[i].haut == touche) return false;
		if(touches[i].bas == touche) return false;
		if(touches[i].gauche == touche) return false;
		if(touches[i].droite == touche) return false;
	}
	return true;
}

void defauts_si_rien(Uint8 conf_id, SDLKey haut, SDLKey bas, SDLKey gauche, SDLKey droite) {
	if(touches[conf_id].haut == SDLK_UNKNOWN && non_assignee(haut)) touches[conf_id].haut = haut;
	if(touches[conf_id].bas == SDLK_UNKNOWN && non_assignee(bas)) touches[conf_id].bas = bas;
	if(touches[conf_id].gauche == SDLK_UNKNOWN && non_assignee(gauche)) touches[conf_id].gauche = gauche;
	if(touches[conf_id].droite == SDLK_UNKNOWN && non_assignee(droite)) touches[conf_id].droite = droite;
}

void fin_prog(void) {
	/* Appelée par atexit(3) à la fermeture de l'application. */
	FILE* config_fich;
	Uint8 i;

	Uint8 conf_args;

	SDL_FreeSurface(ecran);

	stop_son();
	SDL_CloseAudio();
	libere_themes();

#ifdef NET_SUPPORT
	libere_reseau();
#endif

	TTF_Quit();
	SDL_Quit();

	config_fich = fopen(conf_chemin, "wb");
	if(config_fich != NULL) {
		for(i = 0 ; i < 4 ; i++) {
			fprintf(config_fich,
				"%d-%d-%d-%d\n",
					(int)touches[i].haut, (int)touches[i].bas, (int)touches[i].gauche, (int)touches[i].droite
			);
		}
	
		conf_args = 0;
		conf_args |= (plein_ecran	? (1 << 0) : 0);
		conf_args |= (son_active	? (1 << 1) : 0);
		conf_args |= (aff_fps		? (1 << 2) : 0);
		conf_args |= (aff_sc_perm	? (1 << 3) : 0);
	
		fprintf(config_fich, "%s\n%d", theme_act, conf_args);
		fclose(config_fich);
	} else {
		perror(conf_chemin);
	}
}

void def_resolution(void) {
	/* Cherche la meilleure résolution de l'écran */

	SDL_VideoInfo* vid_info;
	SDL_Rect** vid_modes;

	vid_info = (SDL_VideoInfo*)SDL_GetVideoInfo();
	vid_modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	fen_ok = (vid_info->wm_available && vid_info->current_w > LARG_ECRAN
		&& vid_info->current_h > HAUT_ECRAN);

	decalage_haut = 0;
	decalage_gauche = 0;
	h_ecran = HAUT_ECRAN;
	l_ecran = LARG_ECRAN;
	ratio_police = 1;

	if(vid_modes == NULL || vid_modes == (SDL_Rect**)-1) { /* Pas d'infos */
		if(!SDL_VideoModeOK(LARG_ECRAN, HAUT_ECRAN, 32, SDL_HWSURFACE)) {
			fprintf(stderr, "No video mode available !");
			exit(EXIT_FAILURE);
		}

	} else if(vid_modes[0]->w < LARG_ECRAN) { /* Trop petit */
		fprintf(stderr, "Screen too small (less than %d pixels wide)\n", LARG_ECRAN);
		exit(EXIT_FAILURE);
	
	} else if(vid_modes[0]->w == LARG_ECRAN) { /* Mini écran, on vérifie la hauteur */
		if(vid_modes[0]->h < HAUT_ECRAN) {
			h_ecran = vid_modes[0]->h;
			decalage_haut = HAUT_ECRAN - h_ecran;
			ratio_police = (float)h_ecran / (float)HAUT_ECRAN;
		}
	
	/*} else if(SDL_VideoModeOK(HAUT_ECRAN * vid_modes[0]->w/vid_modes[0]->h,
		HAUT_ECRAN, 32, SDL_HWSURFACE)) {
		l_ecran = HAUT_ECRAN * vid_modes[0]->w/vid_modes[0]->h;
		decalage_gauche = (l_ecran - LARG_ECRAN) / 2;*/

	} else if(!SDL_VideoModeOK(LARG_ECRAN, HAUT_ECRAN, 32, SDL_HWSURFACE)) {
		fprintf(stderr, "No video mode available !\n");
		exit(EXIT_FAILURE);
	}

	/*printf("Windowed mode %s\n", (fen_ok ? "available" : "not available"));
	printf("Fullscreen resolution: %dx%d\n", l_ecran, h_ecran);*/
}

void conf_defaut(void) {
	/* Configuration par défaut */
	plein_ecran = false;
	son_active = true;
	aff_fps = false;
	aff_sc_perm = true;
	strncpy(theme_act, "default", sizeof(theme_act));
}

int main(int argc, char* argv[]) {
	Uint8 i;
	int temp_haut, temp_bas, temp_gauche, temp_droite;
	Uint8 conf_args;

	/* Configuration du clavier */
	FILE* config_fich;

	if(argc >= 1) {
		chemin_moi = argv[0];
	}

#ifndef MACOS9
	if(getenv(DOSS_PREFS) != NULL) {
		conf_chemin[0] = '\0';
		strncat(conf_chemin, getenv(DOSS_PREFS), sizeof(conf_chemin));
		strncat(conf_chemin, SEP, sizeof(conf_chemin));
		strncat(conf_chemin, CONFIG_NOM, sizeof(conf_chemin));
	}
#endif

	config_fich = fopen(conf_chemin, "rb");

	if(config_fich == NULL) {
		conf_defaut();
	} else {
		for(i = 0 ; i < 4 ; i++) {
			temp_haut = temp_bas = temp_gauche = temp_droite = 0;
			if(fscanf(config_fich, "%d-%d-%d-%d ", &temp_haut, &temp_bas, &temp_gauche, &temp_droite) == 4) {
				touches[i].haut		= temp_haut;
				touches[i].bas		= temp_bas;
				touches[i].gauche	= temp_gauche;
				touches[i].droite	= temp_droite;
			}
		}

		if(fscanf(config_fich, "%32[^\n] %hhd",
			theme_act, &conf_args) == 2) {
		
			plein_ecran	= conf_args & (1 << 0);
			son_active	= conf_args & (1 << 1);
			aff_fps		= conf_args & (1 << 2);
			aff_sc_perm	= conf_args & (1 << 3);
		} else {	
			conf_defaut();
		}
	
		fclose(config_fich);
	}

	defauts_si_rien(0, SDLK_z, SDLK_s, SDLK_q, SDLK_d);
	defauts_si_rien(1, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT);
	defauts_si_rien(2, SDLK_t, SDLK_g, SDLK_f, SDLK_h);
	defauts_si_rien(3, SDLK_i, SDLK_k, SDLK_j, SDLK_l);

	/* Configuration des paramètres du jeu */
	act_conf = 0;
	nb_balles = 1;

	/* Pour que ça marche sous Windows il faut que ces lignes soient ici */
	#if defined(I10N) && !defined(MACOSX) /* Dans Mac OS X c'est dans le SDLMain.m */
		setlocale(LC_ALL, "");
		bindtextdomain("slimevolley", I10N_DIR);
		bind_textdomain_codeset("slimevolley", "UTF-8");
		textdomain("slimevolley");
	#endif

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	def_resolution();
	if(!fen_ok) plein_ecran = true;
	TTF_Init();
	init_son();

	init_th_stage1();

#ifndef MACOSX
	if(icone_fen != NULL) {
		SDL_WM_SetIcon(icone_fen, NULL);
		SDL_FreeSurface(icone_fen);
	}
#endif


	ecran = SDL_SetVideoMode((plein_ecran ? l_ecran : LARG_ECRAN), (plein_ecran ? h_ecran : HAUT_ECRAN), 32, SDL_HWSURFACE | SDL_DOUBLEBUF | (plein_ecran ? SDL_FULLSCREEN : 0));
	init_th_stage2();
	place_rects();

	SDL_WM_SetCaption("Slime Volley", "Slime Volley");
	SDL_ShowCursor(SDL_DISABLE);

	atexit(fin_prog);

#ifdef NET_SUPPORT
	init_reseau();
#endif

	/* Préréglage des configurations joueurs */
	tab_joueurs[0].type = CLAVIER;
	tab_joueurs[0].conf_clavier = 0;
	tab_joueurs[NB_JOUEURS_G].type = CLAVIER;
	tab_joueurs[NB_JOUEURS_G].conf_clavier = 1;

	menu_princ();

	exit(EXIT_SUCCESS);

#ifdef MACOS9
	return 0; /* Supprime un warning de MPW qui ne sait pas que exit équivaut à return dans un main() */
#endif
}

Uint32 ev_timer(Uint32 intervalle, void* param) {
	SDL_Event ev_envoye;
	(void)param; /* unused parameter */

	SDL_PushEvent(&ev_envoye);

	return intervalle;
}
