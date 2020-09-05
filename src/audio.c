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

$Id: audio.c 272 2008-12-23 08:44:22Z vinduv $
*/

#include <unistd.h>

#include <SDL.h>

#include "slime.h"
#include "audio.h"

SDL_AudioSpec format_obtenu;

slime_son* son_act;
Uint32 son_pos; /* La position actuelle si un son est en lecture */

void mixeur(void* userdata, Uint8* stream, int len) {
	/* Transfère les données sonores de la mémoire centrale vers la mémoire audio.
	Pour le moment, ne gère pas le mixage de plusieurs sons simultanés. (est-ce vraiment utile ?) */

	Sint32 long_copie;

	(void)userdata; /* unused parameter */

	if(son_act == NULL) {
		SDL_PauseAudio(1);
		return;
	}

	long_copie = son_act->longueur - son_pos;

	if(long_copie > len) {
		long_copie = len;
	}

	/* Copie des données audio (stream : adresse du buffer audio) */
	memcpy(stream, son_act->donnees + son_pos, long_copie);

	/* Mise à jour de la position de lecture */
	son_pos += long_copie;

	if(son_pos == son_act->longueur) { /* On a fini de jouer le son */
		son_act = NULL;
	}
 }

void init_son(void) {
	SDL_AudioSpec format_voulu;

	/* Son 16 bits stéréo - 44100 Hz */
	format_voulu.freq = 44100;
	format_voulu.format = 0;
	format_voulu.channels = 2;

	/* Taille de tampon audio (512 : bon compromis latence/fluidité) */
	format_voulu.samples = 512;

	format_voulu.callback = &mixeur;
	format_voulu.userdata = NULL;

	if(SDL_OpenAudio(&format_voulu, &format_obtenu) != 0) {
		fprintf(stderr, "Error opening audio device : %s\n"
		"Sound effects will be disabled.\n", SDL_GetError());
		audio_desact = true;
	} else {
		audio_desact = false;
	}
}

void conversion_son(SDL_AudioSpec format_son, Uint8 id) {
	SDL_AudioCVT cvt;

	if(audio_desact) return;

	/* Conversion du son vers le format de la carte audio */
	if(SDL_BuildAudioCVT(&cvt, format_son.format, format_son.channels, format_son.freq,
		format_obtenu.format, format_obtenu.channels, format_obtenu.freq) < 0) {
		fprintf(stderr, "Error initialising audio converter: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	cvt.buf = malloc(sons[id].longueur * cvt.len_mult);
	cvt.len = sons[id].longueur;
	memcpy(cvt.buf, sons[id].donnees, sons[id].longueur);

	if (SDL_ConvertAudio(&cvt) != 0) {
		fprintf(stderr, "Error converting sound: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_FreeWAV(sons[id].donnees); /* On efface les données audio originales */
	sons[id].donnees = malloc(cvt.len_cvt); /* Et on met les nouvelles à la place */
	memcpy(sons[id].donnees, cvt.buf, cvt.len_cvt);
	free(cvt.buf);
}

void joue_son(Sint8 id) {
	if(!son_active || audio_desact) return;
	
	if(id < 0 || id >= NB_SONS) {
		fprintf(stderr, "Unknown sound ID: %d\n", id);
		return;
	}
	
	/* Si un son est en lecture, il sera automatiquement coupé. */
	if(sons[id].longueur == 0) {
		if(id == SON_JD_MARQUE && sons[SON_JG_MARQUE].longueur > 0) {
			id = SON_JG_MARQUE;
		} else if(id == SON_JD_GAGNE && sons[SON_JG_GAGNE].longueur > 0) {
			id = SON_JG_GAGNE;
		} else {
			return;
		}
	}
	son_pos = 0;
	son_act = &sons[id]; /* On règle le son actuel... */

	SDL_PauseAudio(0); /* Et on lance la lecture. */
}

void stop_son(void) {
	/* Arrête la lecture audio en cours. */
	if(!son_active || audio_desact) return;

	SDL_PauseAudio(1);
	son_act = NULL;
}
