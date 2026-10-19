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

$Id: menu_jeu.c 270 2008-12-13 21:46:13Z mcmic $
*/

#include "slime.h"
#include "jeu.h"
#include "themes.h"

#ifdef NET_SUPPORT
	#define DERN_CONFIG 3 /* type DISTANT */
#else
	#define DERN_CONFIG 2 /* Type IA */
#endif

#define POS_GAUCHE 50
#define POS_DROITE 410
#define ESP_CURS 5

/* Pour le menu */
#define SUR_DEMARRER_G -2
#define SUR_DEMARRER_D -1
#define SUR_RETOUR_G NB_JOUEURS_T
#define SUR_RETOUR_D NB_JOUEURS_T + 1

Sint8 conf_clav_act[] = { -1, -1, -1, -1 };

void change_conf(Uint8 id) {
	/* Change la configuration d'un joueur. */

	Uint8 i, temp_id;
	Sint8 conf;

	if(id != 0 && id != NB_JOUEURS_G && tab_joueurs[id - 1].type == DESACTIVE) return;

	if(tab_joueurs[id].type == DESACTIVE
		|| (tab_joueurs[id].type == DERN_CONFIG && (id == 0 || id == NB_JOUEURS_G)
#ifdef SV_XBOX
			&& tab_joueurs[id].niveau == NB_NIVEAUX-1
#endif
		)) {
		/* Passage à la configuration clavier 0 */

		conf = -1;
		for(i = 0 ; i < 4 ; i++) {
			if(conf_clav_act[i] == -1) {
				conf = i;
				break;
			}
		}
		tab_joueurs[id].type = CLAVIER;
		if(conf == -1) { /* Toutes les configurations sont occupées, on prend la 0 */
			tab_joueurs[id].type++;
#ifdef SV_XBOX
			tab_joueurs[id].niveau = NOOB;
#endif
		} else {
			conf_clav_act[conf] = id;
			tab_joueurs[id].conf_clavier = conf;
		}

	} else if(tab_joueurs[id].type == CLAVIER && tab_joueurs[id].conf_clavier < 3) {
		/* Passage à la configuration clavier suivante */

		conf = tab_joueurs[id].conf_clavier;
		conf_clav_act[conf] = -1;
		for(i = conf+1 ; i < 4 ; i++) {
			if(conf_clav_act[i] == -1) {
				conf = i;
				break;
			}
		}
		if(conf == tab_joueurs[id].conf_clavier) {
			tab_joueurs[id].type++;
#ifdef SV_XBOX
			tab_joueurs[id].niveau = NOOB;
#endif
		} else {
			conf_clav_act[conf] = id;
			tab_joueurs[id].conf_clavier = conf;
		}

	} else if(tab_joueurs[id].type == INT_ART && tab_joueurs[id].niveau<NB_NIVEAUX-1) {
		tab_joueurs[id].niveau++;
	} else if(tab_joueurs[id].type == DERN_CONFIG) {
		/* On désactive le joueur (les joueurs non-désactivables sont pris en compte plus haut */

		tab_joueurs[id].type = DESACTIVE;

		if(id < NB_JOUEURS_G) { /* On décale éventuellement les joueurs suivants */
			temp_id = NB_JOUEURS_G;
		} else {
			temp_id = NB_JOUEURS_T;
		}

		for(i = id ; i < temp_id ; i++) {
			if(tab_joueurs[i].type == CLAVIER) conf_clav_act[tab_joueurs[i].conf_clavier] = -1;
			tab_joueurs[i].type = DESACTIVE;
		}

	} else { /* On passe à la configuration suivante */
		if(tab_joueurs[id].type == CLAVIER) conf_clav_act[tab_joueurs[id].conf_clavier] = -1;
		if(tab_joueurs[id].type == INT_ART) tab_joueurs[id].niveau = NOOB;
		tab_joueurs[id].type++;
#ifdef SV_XBOX
		if(tab_joueurs[id].type == INT_ART) tab_joueurs[id].niveau = NOOB;
#endif
	}

}

void menu_jeu(void) {
#ifdef SV_XBOX
	const char* noms_confs[] = { "Disabled", "Controller %d", "AI, level %d", "Remote" };
#else
	const char* noms_confs[] = { N_("Disabled"), N_("Local, key set %c"), N_("AI, level %d"), N_("Remote") };
#endif

	char tmp_chaine[40], tmp_chaine2[40];

	SDL_Rect dessin_rect;
	SDL_Rect curs_rect = { 0, 0, 16, 16 };
	Uint16 menu_y;
	SDL_Surface* s_texte;
	Sint8 id_act = SUR_DEMARRER_G;

	Uint8 i;

	bool maj_requise;

	conf_clav_act[0] = conf_clav_act[1] = conf_clav_act[2] = conf_clav_act[3] = -1;
	for(i = 0 ; i < NB_JOUEURS_T ; i++) {
		if(tab_joueurs[i].type == CLAVIER) {
			conf_clav_act[tab_joueurs[i].conf_clavier] = i;
		}
	}

	for(;;) {
		SDL_BlitSurface(fond, NULL, ecran, NULL);

		/* Lancement du jeu */
		dessin_rect.x = POS_GAUCHE;
		dessin_rect.y = menu_decalage;

		s_texte = TTF_RenderUTF8_Blended(police_menu, _("Start game"), coul_txt_menu);
		SDL_BlitSurface(s_texte, NULL, ecran, &dessin_rect);

		if(id_act < 0) {
			curs_rect.x = dessin_rect.x - ESP_CURS - icone_jg->w;
			curs_rect.y = dessin_rect.y + (s_texte->h - curs_rect.h) / 2 + 2;
			SDL_BlitSurface(icone_jg, NULL, ecran, &curs_rect);
		}

		dessin_rect.y += s_texte->h;
		SDL_FreeSurface(s_texte);

		/* Colonne de droite */
		dessin_rect.x = POS_DROITE;

		s_texte = TTF_RenderUTF8_Blended(police_menu, _("Right team:"), coul_txt_menu);
		SDL_BlitSurface(s_texte, NULL, ecran, &dessin_rect);
		SDL_FreeSurface(s_texte);

		/* Colonne de gauche */
		dessin_rect.x = POS_GAUCHE;

		s_texte = TTF_RenderUTF8_Blended(police_menu, _("Left team:"), coul_txt_menu);
		SDL_BlitSurface(s_texte, NULL, ecran, &dessin_rect);
		menu_y = dessin_rect.y += s_texte->h;
		SDL_FreeSurface(s_texte);

		for(i = 0 ; i < NB_JOUEURS_T ; i++) {
			if(i == NB_JOUEURS_G) {
				dessin_rect.x = POS_DROITE;
				dessin_rect.y = menu_y;
			}

			if(tab_joueurs[i].type == CLAVIER) {
				snprintf4(tmp_chaine2, 40, _(noms_confs[CLAVIER]),
#ifdef SV_XBOX
				tab_joueurs[i].conf_clavier + 1
#else
				'A' + tab_joueurs[i].conf_clavier
#endif
			);
				snprintf5(tmp_chaine, 40, _("#%d: %s"), (i < NB_JOUEURS_G ? i + 1 : i - NB_JOUEURS_G + 1), tmp_chaine2);
			} else if(tab_joueurs[i].type == INT_ART) {
				snprintf4(tmp_chaine2, 40, _(noms_confs[INT_ART]), tab_joueurs[i].niveau);
				snprintf5(tmp_chaine, 40, _("#%d: %s"), (i < NB_JOUEURS_G ? i + 1 : i - NB_JOUEURS_G + 1), tmp_chaine2);
			} else {
				snprintf5(tmp_chaine, 40, _("#%d: %s"), (i < NB_JOUEURS_G ? i + 1 : i - NB_JOUEURS_G + 1), _(noms_confs[tab_joueurs[i].type]));
			}

			s_texte = TTF_RenderUTF8_Blended(police, tmp_chaine, coul_txt_menu);
			SDL_BlitSurface(s_texte, NULL, ecran, &dessin_rect);

			if(i == id_act) {
				curs_rect.x = dessin_rect.x - ESP_CURS - icone_jg->w;
				curs_rect.y = dessin_rect.y + (s_texte->h - curs_rect.h) / 2 + 2;

				if(id_act < NB_JOUEURS_G) {
					SDL_BlitSurface(icone_jg, NULL, ecran, &curs_rect);
				} else {
					SDL_BlitSurface(icone_jd, NULL, ecran, &curs_rect);
				}
			}

			dessin_rect.y += s_texte->h;
			SDL_FreeSurface(s_texte);
		}

		/* Retour */
		dessin_rect.x = POS_GAUCHE;

		s_texte = TTF_RenderUTF8_Blended(police_menu, _("Return"), coul_txt_menu);
		SDL_BlitSurface(s_texte, NULL, ecran, &dessin_rect);

		if(id_act >= NB_JOUEURS_T) {
			curs_rect.x = dessin_rect.x - ESP_CURS - icone_jg->w;
			curs_rect.y = dessin_rect.y + (s_texte->h - curs_rect.h) / 2 + 2;
			SDL_BlitSurface(icone_jd, NULL, ecran, &curs_rect);
		}

		/* Aide */
		if(id_act < 0) {
			aff_aide(_("Start the game with the specified parameters"));
		} else if(id_act < NB_JOUEURS_T) {
			switch(tab_joueurs[id_act].type) {
				case CLAVIER:
#ifdef SV_XBOX
					aff_aide("Use the controller in the selected Xbox port.\nPress A to cycle player settings.");
#else
					aff_aide(_("This player will be controlled from the keyboard.\n"
						"You can change the key configurations in the Options menu."));
#endif
				break;
				case INT_ART:
#ifdef SV_XBOX
					aff_aide("AI controls this player.\nPress A to cycle player settings.");
#else
					aff_aide(_("This player will be controlled by the computer."));
#endif
				break;
				case DISTANT:
					aff_aide(_("This player will be controlled by a remote computer on local network.\n"
						"You will need to allow connections to your computer on port 2222."));
				break;
				default:
#ifdef SV_XBOX
					aff_aide("Press A to add this teammate.");
#endif
				break;
			}
		}

		SDL_Flip(ecran);

		maj_requise = false;

		do {
			SDL_WaitEvent(&evenement);

			do {
				switch(evenement.type) {
					case SDL_QUIT:
						return;
					break;
					case SDL_KEYDOWN:
						switch(evenement.key.keysym.sym) {
							case SDLK_ESCAPE:
								return;
							break;

							case SDLK_UP:
								maj_requise = true;

								switch(id_act) {
									case SUR_DEMARRER_G:
										id_act = SUR_RETOUR_G;
									break;

									case SUR_DEMARRER_D:
										id_act = SUR_RETOUR_D;
									break;

									case 0: /* En haut du menu de gauche */
										id_act = SUR_DEMARRER_G;
									break;

									case NB_JOUEURS_G: /* En haut du menu de droite */
										id_act = SUR_DEMARRER_D;
									break;

									case SUR_RETOUR_G:
										id_act = NB_JOUEURS_G - 1;
									break;

									case SUR_RETOUR_D:
										id_act = NB_JOUEURS_T - 1;
									break;

									default:
										id_act--;
									break;
								}
							break;

							case SDLK_DOWN:
								maj_requise = true;

								switch(id_act) {
									case SUR_DEMARRER_G:
										id_act = 0;
									break;

									case SUR_DEMARRER_D:
										id_act = NB_JOUEURS_G;
									break;

									case NB_JOUEURS_G - 1: /* En bas du menu de gauche */
										id_act = SUR_RETOUR_G;
									break;

									case NB_JOUEURS_T - 1: /* En bas du menu de droite */
										id_act = SUR_RETOUR_D;
									break;

									case SUR_RETOUR_G:
										id_act = SUR_DEMARRER_G;
									break;

									case SUR_RETOUR_D:
										id_act = SUR_DEMARRER_D;
									break;

									default:
										id_act++;
									break;
								}
							break;

							case SDLK_RIGHT:
							case SDLK_LEFT:
								if(0 <= id_act && id_act < NB_JOUEURS_G) {
									maj_requise = true;
									id_act += NB_JOUEURS_G;
								} else if(NB_JOUEURS_G <= id_act && id_act < NB_JOUEURS_T) {
									maj_requise = true;
									id_act -= NB_JOUEURS_G;
								}
							break;

							case SDLK_RETURN:
								maj_requise = true;
								switch(id_act) {
									case SUR_DEMARRER_G:
									case SUR_DEMARRER_D:
										jeu_srv();
										return;
									break;

									case SUR_RETOUR_G:
									case SUR_RETOUR_D:
										return;
									break;

									default:
										change_conf(id_act);
									break;
								}
							break;

							default:
							break;
						}
					break;

					case SDL_KEYUP:
						if(evenement.type == SDL_KEYUP && evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
							SDL_WM_IconifyWindow();
						}
					break;

					default:
					break;
				}
			} while(SDL_PollEvent(&evenement));

		} while(!maj_requise);
	}
}
