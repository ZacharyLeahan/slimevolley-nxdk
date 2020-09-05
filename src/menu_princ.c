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

$Id: menu_princ.c 234 2008-09-01 19:53:57Z vinduv $
*/

#include "credits.h"
#include "jeu.h"
#include "menu_jeu.h"
#include "menu_options.h"
#include "menu.h"
#include "slime.h"
#include "themes.h"

char tmp_chaine[64];

char* m_vitesse_aff(void) {
	const char* noms_confs[] = { N_("normal"), N_("turbo"), N_("sprint") , N_("fury") };

	snprintf4(tmp_chaine, 64, _("Speed: %s"), _(noms_confs[act_conf]));
	return tmp_chaine;
}

void m_vitesse_act(void) {
	if(action < 0) {
		act_conf = (act_conf > 0 ? act_conf - 1 : NUM_CONFIGS - 1);
	} else {
		act_conf = (act_conf < (NUM_CONFIGS - 1) ? act_conf + 1 : 0);
	}
}

char* m_vitesse_aide(void) {
	switch(act_conf) {
		case 0:
			return _("The default speed.");
		break;
		case 1:
			return _("In the turbo mode, everything goes twice as highter\n and twice as faster.");
		break;
		case 2:
			return _("In the sprint mode, everything goes faster and highter\n for each bounce.");
		break;
		case 3:
			return _("In the fury mode, the speed is increased for each bounce.");
		break;
		default:
			return "";
		break;
	}
}


char* m_balles_aff(void) {
	snprintf4(tmp_chaine, 64, _("Balls: %d"), nb_balles);
	return tmp_chaine;
}

void m_balles_act(void) {
	if(action < 0) {
		if(--nb_balles <= 0) nb_balles = MAX_BALLES;
	} else {
		if(++nb_balles > MAX_BALLES) nb_balles = 1;
	}
}

void menu_princ(void) {
	menu_elem princ_menu[] = {
		/* Texte							Aide													Action			rec. G/D	Cont. */
		{ NULL, N_("Play…"),				NULL, N_("Starts a new game."),							&menu_jeu, 		false,		true },
#ifdef NET_SUPPORT
#define MAX_OPTS 6
		{ NULL, N_("Connect to server…"),	NULL, N_("Connect to a distant server."),				&jeu_clt, 		false,		true },
#else
#define MAX_OPTS 5
#endif
		{ &m_vitesse_aff, NULL,				&m_vitesse_aide, NULL,									&m_vitesse_act,	true,		true },
		{ &m_balles_aff, NULL,				NULL, N_("Choose the number of balls."),				&m_balles_act,	true,		true },
		{ NULL, N_("Options…"),				NULL, N_("Configure sound, keys, screen and theme."),	&menu_options,	false,		true },
		{ NULL, N_("Credits"),				NULL, N_("Who made this game?"),						&aff_credits,	false,		true },
	};

	affiche_menu(princ_menu, MAX_OPTS, N_("Quit"));

	return;
}
