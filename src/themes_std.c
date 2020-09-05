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

$Id: themes_std.c 288 2010-04-21 07:38:04Z vinduv $
*/

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include <SDL_ttf.h>
#include <SDL_image.h>

#include "slime.h"
#include "audio.h"
#include "themes.h"

#define DATA_LOCAL "data"
#define DOSS_THEMES "themes"
#define DOSS_SLIMES "slimes"
#define SLIME_D_NOM "slimeJD%d.png"
#define SLIME_G_NOM "slimeJG%d.png"
#define FICH_THEME "theme.txt"
#define ICONE "icone.png"

#ifdef WINDOWS
	#define SEP "\\"
#else
	#ifdef MACOS9
		#define SEP ":"
	#else
		#define SEP "/"
	#endif
#endif

char chemin[255];

const char* son_noms[NB_SONS] = {
	"s_slime.wav",
	"s_mur.wav",
	"s_filet.wav",
	"s_jg_m.wav",
	"s_jd_m.wav",
	"s_jg_g.wav",
	"s_jd_g.wav"
};

Uint8 long_chemin_base, long_chemin_ext, long_chemin_thm;

DIR* themes_d;
FILE* theme_f;

void _charge_commun(void) {
	/* Etape 1 : On sélectionne le dossier de données */

	char* var_env = getenv("SLIME_DATADIR");
	Uint8 i;

	chemin[0] = '\0';

	if(var_env == NULL) {
		if(access(DATA_LOCAL, R_OK | X_OK) == 0) {
			strncpy(chemin, DATA_LOCAL, sizeof(chemin) - 1);
		
		} else if(access(DATA_DIR, R_OK | X_OK) == 0) {
			strncpy(chemin, DATA_DIR, sizeof(chemin) - 1);
		}
	
	} else if(access(var_env, R_OK | X_OK) == 0) {
		strncpy(chemin, var_env, sizeof(chemin) - 1);
	
	}

	if(chemin[0] == '\0') {
		fprintf(stderr, _("Unable to access the data directory.\n"));
		exit(EXIT_FAILURE);
	}

	strncat(chemin, SEP, sizeof(chemin) - 1);
	long_chemin_base = strlen(chemin);

	/* Etape 2 : On charge l'icône de la fenêtre */
	
	strncat(chemin, ICONE, sizeof(chemin) - 1);
	icone_fen = IMG_Load(chemin);
	chemin[long_chemin_base] = '\0';

	/* Etape 3 : On charge les images des slimes */

	strncat(chemin, DOSS_SLIMES, sizeof(chemin) - 1);
	strncat(chemin, SEP, sizeof(chemin) - 1);
	long_chemin_ext = strlen(chemin);

	img_max_jg = 0;
	do {
		img_max_jg++;
		snprintf(chemin + long_chemin_ext, sizeof(chemin) - long_chemin_ext - 1, SLIME_G_NOM, img_max_jg);
	} while(access(chemin, F_OK) == 0);

	img_grand_jg = (SDL_Surface**)malloc((img_max_jg + 1) * sizeof(SDL_Surface*));

	for(i = 1 ; i < img_max_jg ; i++) {
		snprintf(chemin + long_chemin_ext, sizeof(chemin) - long_chemin_ext - 1, SLIME_G_NOM, i);
		img_grand_jg[i + 1] = IMG_Load(chemin);
		if(img_grand_jg[i + 1] == NULL) {
			fprintf(stderr, _("Error loading slime skins: %s\n"), SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	img_max_jd = 0;
	do {
		img_max_jd++;
		snprintf(chemin + long_chemin_ext, sizeof(chemin) - long_chemin_ext - 1, SLIME_D_NOM, img_max_jd);
	} while(access(chemin, F_OK) == 0);

	img_grand_jd = (SDL_Surface**)malloc((img_max_jd + 1) * sizeof(SDL_Surface*));

	for(i = 1 ; i < img_max_jd ; i++) {
		snprintf(chemin + long_chemin_ext, sizeof(chemin) - long_chemin_ext - 1, SLIME_D_NOM, i);
		img_grand_jd[i + 1] = IMG_Load(chemin);
		if(img_grand_jd[i + 1] == NULL) {
			fprintf(stderr, _("Error loading slime skins: %s\n"), SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}
}

void _demarre_switcheur(void) {
	chemin[long_chemin_base] = '\0';
	strncat(chemin, DOSS_THEMES, sizeof(chemin) - 1);
	strncat(chemin, SEP, sizeof(chemin) - 1);
	long_chemin_ext = strlen(chemin);

	themes_d = opendir(chemin);
	if(themes_d == NULL) {
		fprintf(stderr, _("Unable to access themes directory %s: "), chemin);
		perror("opendir");
		exit(EXIT_FAILURE);
	}
}

void _arret_switcheur(void) {
	closedir(themes_d);
}

SDL_Surface* _charge_image(const char* nom_img) {
	SDL_Surface* img;

	chemin[long_chemin_thm] = '\0';

	strncat(chemin, nom_img, sizeof(chemin) - 1);
	img = IMG_Load(chemin);
	if(img == NULL) {
		fprintf(stderr, _("Error loading image from theme %s: %s\n"), theme_act, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	return img;
}

TTF_Font*  _charge_police(const char* nom_police, Uint8 taille) {
	TTF_Font* police;

	chemin[long_chemin_thm] = '\0';

	strncat(chemin, nom_police, sizeof(chemin) - 1);
	police = TTF_OpenFont(chemin, taille);
	if(police == NULL) {
		fprintf(stderr, _("Error loading font from theme %s: %s\n"), theme_act, TTF_GetError());
		exit(EXIT_FAILURE);
	}

	return police;
}

void _charge_son(Uint8 id_son) {
	SDL_AudioSpec format_son;

	if(audio_desact) return;

	chemin[long_chemin_thm] = '\0';
	strncat(chemin, son_noms[id_son], sizeof(chemin) - 1);


	if(access(chemin, R_OK) != 0) {
		sons[id_son].longueur = 0;
		return;
	}

	if(SDL_LoadWAV(chemin, &format_son, &sons[id_son].donnees, &sons[id_son].longueur) == NULL) {
		fprintf(stderr, _("Error loading sound %s from theme %s: %s\n"), son_noms[id_son], theme_act, SDL_GetError());
		exit(EXIT_FAILURE);
	}

	conversion_son(format_son, id_son);
}

void _theme_suivant(bool initial) {
	struct dirent* fichier;

	while(true) {
		fichier = readdir(themes_d);
		if(fichier == NULL) break;
		if(strncmp(fichier->d_name, theme_act, sizeof(theme_act)) == 0) continue;
	
		if(_charge_theme_act(fichier->d_name)) {
			strncpy(theme_act, fichier->d_name, sizeof(theme_act));
			return;
		}
	}

	if(!initial) { /* On vous donne une seconde chance */
		rewinddir(themes_d); /* On recommence du début */

		while(true) {
			fichier = readdir(themes_d);
			if(fichier == NULL) break;
	
			if(_charge_theme_act(fichier->d_name)) {
				strncpy(theme_act, fichier->d_name, sizeof(theme_act));
				return;
			}
		}
	}

	fprintf(stderr, _("Unable to find any themes in the themes directory !\n"));
	exit(EXIT_FAILURE);
}

bool _charge_theme_act(char* theme_select) {
	/* Si le dossier pointé par theme_act est valide, effectue le chargement. */
	Uint8 i;

	bool theme_classique;

	if(theme_select[0] == '\0') return false;

	chemin[long_chemin_ext] = '\0';
	strncat(chemin, theme_select, sizeof(chemin) - 1);
	strncat(chemin, SEP, sizeof(chemin) - 1);
	long_chemin_thm = strlen(chemin);
	strncat(chemin, FICH_THEME, sizeof(chemin) - 1);

	theme_f = fopen(chemin, "r");
	if(theme_f == NULL) return false;

	if(fscanf(theme_f, /* Pour un thème normal */
		"%31[^\n] %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %d %d %d",
			nom_theme,
			&coul_fond.r, &coul_fond.g, &coul_fond.b,
			&coul_filet.r, &coul_filet.g, &coul_filet.b,
			&coul_sol.r, &coul_sol.g, &coul_sol.b,
			&coul_txt_menu.r, &coul_txt_menu.g, &coul_txt_menu.b,
			&coul_txt_dial.r, &coul_txt_dial.g, &coul_txt_dial.b,
			&coul_txt_jeu.r, &coul_txt_jeu.g, &coul_txt_jeu.b,
			&menu_decalage, &menu_t_police, &menu_ecart
			) == 22) {
			theme_classique = true;
	
	} else {
		rewind(theme_f);
		if(fscanf(theme_f, /* Pour un thème avec fond */
			"%31[^\n] %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %hhi %d %d %d",
				nom_theme,
				&coul_txt_menu.r, &coul_txt_menu.g, &coul_txt_menu.b,
				&coul_txt_dial.r, &coul_txt_dial.g, &coul_txt_dial.b,
				&coul_txt_jeu.r, &coul_txt_jeu.g, &coul_txt_jeu.b,
				&menu_decalage, &menu_t_police, &menu_ecart
				) == 13) {
			theme_classique = false;
		} else {
			fprintf(stderr, _("The theme %s is corrupt.\n"), nom_theme);
			fclose(theme_f);
			return false;
		}
	}

	fclose(theme_f);

	/* Les données du thème sont valide, on tente de charger les images. */

	if(theme_classique) {
		fond_jeu = NULL;
	} else {
		fond_jeu = _charge_image("jeu.png");
	}

	fond = _charge_image("menu.png");

	img_grand_jg[0] = _charge_image("slimeIAG.png");
	img_grand_jg[1] = _charge_image("slimeG.png");
	img_grand_jd[0] = _charge_image("slimeIAD.png");
	img_grand_jd[1] = _charge_image("slimeD.png");

	balle_img = _charge_image("balle.png");
	fleche = _charge_image("fleche.png");
	oeil = _charge_image("oeil.png");

	police = _charge_police("police.ttf", 25);
	police_menu = _charge_police("police.ttf", menu_t_police * ratio_police);

	for(i = 0 ; i < NB_SONS ; i++) {
		_charge_son(i);
	}

	return true;
}
