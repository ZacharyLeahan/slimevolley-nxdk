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

$Id: packer.c 242 2008-11-08 13:40:13Z vinduv $
*/

/* Ce programme permet d’ajouter une série de fichiers à la fin d’un autre fichier.
*/

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "config.h"

#ifdef WINDOWS
	const char files_path[] = "../data/themes/default/%s";
	const char target_file[] = "slimevolley.exe";
#else
	#ifdef MACOS9
		const char files_path[] = ":data:themes:default:%s";
		const char target_file[] = "Slime Volley";
	#else
		const char files_path[] = "data/themes/default/%s";
		const char target_file[] = "slimevolley";
	#endif
#endif

#ifdef MACOS9
	#define X_OK 1
	#define R_OK 4
	#define M_PI	3.14159265358979323846264338327950288
	#define snprintf4(a, b, c, d) sprintf(a, c, d)
	#define snprintf5(a, b, c, d, e) sprintf(a, c, d, e)
#else
	#define snprintf4(a, b, c, d) snprintf(a, b, c, d)
	#define snprintf5(a, b, c, d, e) snprintf(a, b, c, d, e)
#endif

#define FILES_NUM 14
const char* files[FILES_NUM] = { "balle.png", "fleche.png", "jeu.png", "menu.png", "oeil.png", "slimeIAD.png",
						"slimeIAG.png", "slimeD.png", "slimeG.png", "police.ttf", "s_filet.wav",
						"s_jg_m.wav", "s_mur.wav", "s_slime.wav" };

int main(int argc, char* argv[]) {
	Uint16 i;
	Uint32 size;
	char path[40];
	char* buf;
	FILE* source;
	FILE* target;

	char nom_theme[32];
	int c_menu_r, c_menu_v, c_menu_b, c_txt_d_r, c_txt_d_v, c_txt_d_b,
		c_txt_j_r, c_txt_j_v, c_txt_j_b,
		menu_decalage, menu_t_police, menu_ecart;

	SDL_Color couleurs[3];
	Uint16 nombres[3];

	(void)argc; /* unused */
	(void)argv; /* unused */

	printf("Starting packer...\n");

	target = fopen(target_file, "a");

	if(target == NULL) {
		perror("Unable to open target: fopen");
		exit(EXIT_FAILURE);
	}

	for(i = 0 ; i < FILES_NUM ; i++) {
		snprintf4(path, 40, files_path, files[i]);
		source = fopen(path, "r");

		if(source == NULL) {
			fprintf(stderr, "Unable to open %s: ", path);
			perror("fopen");
			exit(EXIT_FAILURE);
		}

		fseek(source, 0, SEEK_END);
		size = ftell(source);
		rewind(source);
		printf("Copying %s (%d byte(s))...", path, size);
		fflush(stdout);

		buf = (char*)malloc(size * sizeof(char));
		if(fread(buf, size, 1, source) != 1) {
			fprintf(stderr, "Unable to read %s: ", path);
			perror("fread");
			exit(EXIT_FAILURE);
		}

		fwrite(buf, size, 1, target);
		fwrite(&size, 4, 1, target);

		free(buf);

		printf("Done.\n");

		fclose(source);
	}

	printf("Copying theme infos...");

	snprintf4(path, 40, files_path, "theme.txt");
	source = fopen(path, "r");

	if(source == NULL) {
		fprintf(stderr, "Unable to open %s: ", path);
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if(fscanf(source,
		"%[^\n] %i %i %i %i %i %i %i %i %i %d %d %d",
			nom_theme, &c_menu_r, &c_menu_v, &c_menu_b,
				&c_txt_d_r, &c_txt_d_v, &c_txt_d_b,
					&c_txt_j_r, &c_txt_j_v, &c_txt_j_b,
						&menu_decalage, &menu_t_police, &menu_ecart) < 13) {
			fprintf(stderr, "Incorrect theme.txt file.\n");
			exit(EXIT_FAILURE);
	}
	fclose(source);

	couleurs[0].r = c_menu_r;
	couleurs[0].g = c_menu_v;
	couleurs[0].b = c_menu_b;

	couleurs[1].r = c_txt_d_r;
	couleurs[1].g = c_txt_d_v;
	couleurs[1].b = c_txt_d_b;

	couleurs[2].r = c_txt_j_r;
	couleurs[2].g = c_txt_j_v;
	couleurs[2].b = c_txt_j_b;

	nombres[0] = (Uint16)menu_decalage;
	nombres[1] = (Uint16)menu_t_police;
	nombres[2] = (Uint16)menu_ecart;

	fwrite(couleurs, sizeof(SDL_Color), 3, target);
	fwrite(nombres, 2, 3, target);
	printf("Done.\n");

	fclose(target);

	exit(EXIT_SUCCESS);
}
