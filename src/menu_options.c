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

$Id: menu_options.c 241 2008-11-04 21:40:04Z vinduv $
*/

#include "slime.h"
#include "menu.h"
#include "themes.h"
#include "jeu.h"
#include "reseau.h"
#include "audio.h"

char tmp_chaine[64];

void m_touches_act(void) {
	configClavier();
}

char* m_ecran_aff(void) {
	if(plein_ecran) {
		return _("Fullscreen: Enabled");
	} else {
		return _("Fullscreen: Disabled");
	}
}

void m_ecran_act(void) {
	if(!fen_ok) return;
	plein_ecran = !plein_ecran;
	SDL_FreeSurface(ecran);
	ecran = SDL_SetVideoMode((plein_ecran ? l_ecran : LARG_ECRAN), (plein_ecran ? h_ecran : HAUT_ECRAN), 32, SDL_HWSURFACE | SDL_DOUBLEBUF | (plein_ecran ? SDL_FULLSCREEN : 0));
}

char* m_sons_aff(void) {
	if(audio_desact) {
		return _("[Audio unavailable]");
	} else if(son_active) {
		return _("Sounds: Enabled");
	} else {
		return _("Sounds: Disabled");
	}
}

void m_sons_act(void) {
	son_active = !son_active;
}

char* m_score_aff(void) {
	if(aff_sc_perm) {
		return _("Score display: Enabled");
	} else {
		return _("Score display: Disabled");
	}
}

void m_score_act(void) {
	aff_sc_perm = !aff_sc_perm;
}

char* m_fps_aff(void) {
	if(aff_fps) {
		return _("FPS counter: Enabled");
	} else {
		return _("FPS counter: Disabled");
	}
}

void m_fps_act(void) {
	aff_fps = !aff_fps;
}

char* m_theme_aff(void) {
	snprintf4(tmp_chaine, 64, _("Theme: %s"), nom_theme);
	return tmp_chaine;
}

void m_theme_act(void) {
	prep_anim(CGSCube, CGSLeft);
	theme_suivant();
	decalage = 32767; /* Déclenche le recalcul du décalage */
}

#define INFO_SCORE N_("If enabled, the current score will be shown permanently.")

void menu_options(void) {
	menu_elem options_menu[] = {
		/* Texte				Aide									Action			rec. G/D 	Cont. */
		{ NULL, N_("Set keys"),	NULL, N_("Define game key controls."),	&m_touches_act,	true,		true },
		{ &m_ecran_aff, NULL,	NULL, NULL,								&m_ecran_act,	false,		true },
		{ &m_sons_aff, NULL,	NULL, NULL,								&m_sons_act,	false,		true },
		{ &m_score_aff, NULL,	NULL, INFO_SCORE,						&m_score_act,	false,		true },
		{ &m_fps_aff, NULL,		NULL, NULL,								&m_fps_act,		false,		true },

#ifdef SV_PORTABLE
#define MAX_OPTS 5
#else
#define MAX_OPTS 6
		{ &m_theme_aff, NULL,	NULL, N_("The theme change the menu and game look."),								&m_theme_act,	false,		true },
#endif
	};

	prep_anim(CGSFlip, CGSLeft);
	affiche_menu(options_menu, MAX_OPTS, "Return");
	prep_anim(CGSFlip, CGSRight);

	return;
}
