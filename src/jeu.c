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

$Id: jeu.c 272 2008-12-23 08:44:22Z vinduv $
*/

#include <stdbool.h>
#include <math.h>

#include <SDL.h>

#include "slime.h"
#include "IA.h"
#include "jeu.h"
#include "objet.h"
#include "themes.h"
#ifdef NET_SUPPORT
#include "reseau.h"
#include "reseau_inter.h"
#endif

#define FURIE 3
#define VITESSE 1.1

enum { EN_COURS, GAGNE_GAUCHE, GAGNE_DROITE, MATCH_NUL,	ANNULATION, QUITTE };

Uint8 num_droite, num_gauche;
Uint8 score_droite, score_gauche;

char score_chaine[15] = "\0";

Sint8 son_id; /* ID du son à jouer à chaque image, -1 si aucun. */
Sint8 son_prec = -1;
Uint8 son_tps = 0;

float vitesse = VITESSE;

const config_slime configs[NUM_CONFIGS] = {
	/* V. saut Y, Xg, Xd	Dep. j. X	V. b. rbd	grav.	coeff */
	{ -300, -150, 150,	0.15,		500,		400,	0 },	/* Normal */
	{ -600, -300, 300,	0.30,		1000,		1000,	0 },	/* Turbo */
	{ -300, -150, 150,	0.15,		500,		400,	0.07 },	/* Sprint */
	{ -300, -150, 150,	0.15,		500,		400,	0 },	/* Furie */
};

void multiplie_config(void) {
	conf.saut_vy		+= (configs[act_conf].saut_vy) * conf.coefficient;
	conf.saut_g_vx		+= (configs[act_conf].saut_g_vx) * conf.coefficient;
	conf.saut_d_vx		+= (configs[act_conf].saut_d_vx) * conf.coefficient;
	conf.dep_x			+= (configs[act_conf].dep_x) * conf.coefficient;
	conf.v_balle_rbd	+= (configs[act_conf].v_balle_rbd) * conf.coefficient;
	conf.gravite 		+= (configs[act_conf].gravite) * conf.coefficient;
	if((act_conf==FURIE)&&(vitesse<4))
		vitesse += 0.1;
}

void raz_controles(void) {
	Uint8 i;

	for(i = 0 ; i < NB_JOUEURS_T ; i++) {
		tab_joueurs[i].t_haut = false;
		tab_joueurs[i].t_gauche = false;
		tab_joueurs[i].t_droite = false;
		tab_joueurs[i].t_bas = false;
	}
}

void init_jeu(void) {
	/* Prépare le jeu. */
	Uint8 i;
	SDL_Surface** temp_skins;

	if(tab_joueurs[2].type != DESACTIVE) { /* Mode 3 joueurs à gauche */
		num_gauche = 3;
		temp_skins = img_3J_jg;

	} else if(tab_joueurs[1].type != DESACTIVE) { /* Mode 2 joueurs à gauche */
		num_gauche = 2;
		temp_skins = img_2J_jg;

	} else { /* Mode 1 joueur à gauche */
		num_gauche = 1;
		temp_skins = img_grand_jg;
	}

	for(i = 0 ; i < NB_JOUEURS_G ; i++) {
		tab_joueurs[i].sens = GAUCHE;
		tab_joueurs[i].skin_tab = temp_skins;
		if(tab_joueurs[i].type == INT_ART) {
			tab_joueurs[i].skin_num = 0;
		} else if(tab_joueurs[i].skin_num == 0) {
			tab_joueurs[i].skin_num = 1;
		}
		tab_joueurs[i].taille = temp_skins[0]->h;
		tab_joueurs[i].lim_bas = h_ecran - HAUT_SOL - temp_skins[0]->h;
		tab_joueurs[i].lim_gauche = 0;
		tab_joueurs[i].lim_droite = FILET_GAUCHE - temp_skins[0]->w;
	}

	if(tab_joueurs[5].type != DESACTIVE) { /* Mode 3 joueurs à droite */
		num_droite = 3;
		temp_skins = img_3J_jd;

	} else if(tab_joueurs[4].type != DESACTIVE) { /* Mode 2 joueurs à droite */
		num_droite = 2;
		temp_skins = img_2J_jd;

	} else { /* Mode 1 joueur à droite */
		num_droite = 1;
		temp_skins = img_grand_jd;
	}

	for(i = NB_JOUEURS_G ; i < NB_JOUEURS_T ; i++) {
		tab_joueurs[i].sens = DROITE;
		tab_joueurs[i].skin_tab = temp_skins;
		if(tab_joueurs[i].type == INT_ART) {
			tab_joueurs[i].skin_num = 0;
		} else if(tab_joueurs[i].skin_num == 0) {
			tab_joueurs[i].skin_num = 1;
		}
		tab_joueurs[i].taille = temp_skins[0]->h;
		tab_joueurs[i].lim_bas = h_ecran - HAUT_SOL - temp_skins[0]->h;
		tab_joueurs[i].lim_gauche = FILET_DROITE;
		tab_joueurs[i].lim_droite = LARG_ECRAN - temp_skins[0]->w;
	}

	score_droite = score_gauche = 0;
	snprintf5(score_chaine, 15, _("Score: %d - %d"), score_gauche, score_droite);
}

void place_joueurs_et_balles(bool service_a_droite) {
	Uint8 i, balle_id;
	Uint16 espace, ecart, balle_y;

	/* Placement à gauche... */
	espace = (FILET_GAUCHE - num_gauche * tab_joueurs[0].taille * 2) / (num_gauche + 1);
	ecart = tab_joueurs[0].taille * 2 + espace;

	for(i = 0 ; i < num_gauche ; i++) {
		tab_joueurs[i].p_x = tab_joueurs[i].x = espace + ecart * i;
		tab_joueurs[i].p_y = tab_joueurs[i].y = tab_joueurs[i].lim_bas;
		tab_joueurs[i].vx = tab_joueurs[i].vy = 0;
	}

	/* ...et à droite. */
	espace = (LARG_ECRAN - FILET_DROITE - num_droite * tab_joueurs[NB_JOUEURS_G].taille * 2) / (num_droite + 1);
	ecart = tab_joueurs[NB_JOUEURS_G].taille * 2 + espace;

	for(i = NB_JOUEURS_G ; i < NB_JOUEURS_G + num_droite ; i++) {
		tab_joueurs[i].p_x = tab_joueurs[i].x = FILET_DROITE + espace + ecart * (i - NB_JOUEURS_G);
		tab_joueurs[i].p_y = tab_joueurs[i].y = tab_joueurs[i].lim_bas;
		tab_joueurs[i].vx = tab_joueurs[i].vy = 0;
	}

	balle_id = 0;
	balle_y = h_ecran / 2;

	while(balle_id < nb_balles) {
		if(service_a_droite) {
			for(i = NB_JOUEURS_T - 1; i >= NB_JOUEURS_G  ; i--) {
				if(tab_joueurs[i].type == DESACTIVE) continue;
				if(balle_id == nb_balles) break;
				tab_balles[balle_id].p_x = tab_balles[balle_id].x = (float)tab_joueurs[i].x + (float)tab_joueurs[i].taille - 8.0;
				tab_balles[balle_id].p_y = tab_balles[balle_id].y = balle_y;
				tab_balles[balle_id].vx = tab_balles[balle_id].vy = 0;
				balle_id++;
			}
		} else {
			for(i = 0 ; i < NB_JOUEURS_G ; i++) {
				if(tab_joueurs[i].type == DESACTIVE) continue;
				if(balle_id == nb_balles) break;
				tab_balles[balle_id].p_x = tab_balles[balle_id].x = (float)tab_joueurs[i].x + (float)tab_joueurs[i].taille - 8.0;
				tab_balles[balle_id].p_y = tab_balles[balle_id].y = balle_y;
				tab_balles[balle_id].vx = tab_balles[balle_id].vy = 0;
				balle_id++;
			}
		}

		balle_y -= 20;
	}
}

void ctrls_clavier(Uint8 id, touches_joueur t, SDLKey touche_act, bool enfoncee) {
	if(t.haut == touche_act) {
		tab_joueurs[id].t_haut = enfoncee;

	} else if(t.bas == touche_act) {
		tab_joueurs[id].t_bas = enfoncee; /* Si true, sera remise automatiquement à false */

	} else if(t.gauche == touche_act) {
		tab_joueurs[id].t_gauche = enfoncee;

	} else if(t.droite == touche_act) {
		tab_joueurs[id].t_droite = enfoncee;

	}
}

bool commande_joueur(Uint8 id, Uint32 ecart) {

	if(tab_joueurs[id].y == tab_joueurs[id].lim_bas) { /* Le joueur est en bas */
		if(tab_joueurs[id].t_haut) { /* Demande de saut */
			tab_joueurs[id].vy = conf.saut_vy;

			if(tab_joueurs[id].t_gauche) {
				tab_joueurs[id].vx = conf.saut_g_vx;
			} else if(tab_joueurs[id].t_droite) {
				tab_joueurs[id].vx = conf.saut_d_vx;
			} else {
				tab_joueurs[id].vx = 0;
			}
		} else if(tab_joueurs[id].t_gauche) {
			tab_joueurs[id].x -= conf.dep_x * (float)ecart;
		} else if(tab_joueurs[id].t_droite) {
			tab_joueurs[id].x += conf.dep_x * (float)ecart;
		} else {
			tab_joueurs[id].vx = 0;
		}
	}

	if(tab_joueurs[id].t_bas && tab_joueurs[id].dern_chg_coul < SDL_GetTicks() - 500) { /* Demande de changement de couleur */
		tab_joueurs[id].t_bas = false;
		tab_joueurs[id].dern_chg_coul = SDL_GetTicks();
		return true;
	}

	return false;
}

void replace_balle(Uint8 id) {
	/* Détection de collision avec le filet */
	int y = tab_balles[id].y + 8;
	int x = tab_balles[id].x + 8;
	int filet = h_ecran - HAUT_SOL - FILET_HAUT;
	if((y>filet)&&(x>FILET_GAUCHE - 8)&&(x<FILET_DROITE+8)) {
		if(tab_balles[id].p_x + 16 <= FILET_GAUCHE) {
			tab_balles[id].vx = -tab_balles[id].vx;
			tab_balles[id].x = FILET_GAUCHE - 16;
			son_prec = son_id = SON_RBD_FILET;
		} else if(tab_balles[id].p_x >= FILET_DROITE) {
			tab_balles[id].vx = -tab_balles[id].vx;
			tab_balles[id].x = FILET_DROITE;
			son_prec = son_id = SON_RBD_FILET;
		} else {
			printf("Erreur : bug de collision balle/filet\n");
		}
	} else if((y>filet - 8)&&(x>FILET_GAUCHE)&&(x<FILET_DROITE)) {
		tab_balles[id].vy = -abs(tab_balles[id].vy);
		tab_balles[id].y = h_ecran - HAUT_SOL - FILET_HAUT - 16;
		son_prec = son_id = SON_RBD_FILET;
	} else if( sqrt( (x - FILET_GAUCHE)*(x - FILET_GAUCHE) + (filet - y) * (filet - y)) < 8) { /* gestion coin gauche */
		tab_balles[id].vx = (x - FILET_GAUCHE) * conf.v_balle_rbd / sqrt( (x - FILET_GAUCHE)*(x - FILET_GAUCHE) + (filet - y) * (filet - y));
		tab_balles[id].vy = (filet - y) * conf.v_balle_rbd / sqrt( (x - FILET_GAUCHE)*(x - FILET_GAUCHE) + (filet - y) * (filet - y));
		son_prec = son_id = SON_RBD_FILET;
	} else if( sqrt( (x - FILET_DROITE)*(x - FILET_DROITE) + (filet - y) * (filet - y)) < 8) { /* gestion coin droit */
		tab_balles[id].vx = (x - FILET_DROITE) * conf.v_balle_rbd / sqrt( (x - FILET_DROITE)*(x - FILET_DROITE) + (filet - y) * (filet - y));
		tab_balles[id].vy = (filet - y) * conf.v_balle_rbd / sqrt( (x - FILET_DROITE)*(x - FILET_DROITE) + (filet - y) * (filet - y));
		son_prec = son_id = SON_RBD_FILET;
	}
	/*if(tab_balles[id].y + 16 > h_ecran - HAUT_SOL - FILET_HAUT) {
		if(tab_balles[id].p_x + 16 <= FILET_GAUCHE && tab_balles[id].x + 16 > FILET_GAUCHE) {
			tab_balles[id].vx = -tab_balles[id].vx;
			tab_balles[id].x = FILET_GAUCHE - 16;
			son_id = SON_RBD_FILET;
		} else if(tab_balles[id].p_x >= FILET_DROITE && tab_balles[id].x < FILET_DROITE) {
			tab_balles[id].vx = -tab_balles[id].vx;
			tab_balles[id].x = FILET_DROITE;
			son_id = SON_RBD_FILET;
		} else if(tab_balles[id].x + 16 > FILET_GAUCHE && tab_balles[id].x < FILET_DROITE) {
			tab_balles[id].vy = -abs(tab_balles[id].vy);
			tab_balles[id].y = h_ecran - HAUT_SOL - FILET_HAUT - 16;
			son_id = SON_RBD_FILET;
		}
	}*/

	/* Et avec les bords du terrain */
	if(tab_balles[id].x <= 0) {
		tab_balles[id].x = 0;
		tab_balles[id].vx = abs(tab_balles[id].vx);
		son_prec = son_id = SON_RBD_MUR;
	} else if(tab_balles[id].x >= LARG_ECRAN - balle_img->w) {
		tab_balles[id].x = LARG_ECRAN - balle_img->w;
		tab_balles[id].vx = -abs(tab_balles[id].vx);
		son_prec = son_id = SON_RBD_MUR;
	}

	if(tab_balles[id].y >= h_ecran - HAUT_SOL - balle_img->h) {
		tab_balles[id].y = h_ecran - HAUT_SOL - balle_img->h;
	}
}

void replace_oeil(Uint8 j_id, Uint8 b_id) {
	float dist_oeil_x, dist_oeil_y, dist_oeil;
	float ratio = (tab_joueurs[j_id].taille/50.0);
	if(tab_joueurs[j_id].sens == GAUCHE) {
		dist_oeil_x = tab_joueurs[j_id].x + 69 * ratio - (tab_balles[b_id].x + 8);
		dist_oeil_y = tab_joueurs[j_id].y + 24 * ratio - (tab_balles[b_id].y + 8);
		dist_oeil = sqrt(dist_oeil_x * dist_oeil_x + dist_oeil_y * dist_oeil_y);
		tab_joueurs[j_id].oeil_x = tab_joueurs[j_id].x + 69 * ratio - 5 - (10 * ratio - 5) * dist_oeil_x/dist_oeil;
		tab_joueurs[j_id].oeil_y = tab_joueurs[j_id].y + 24 * ratio - 5 - (10 * ratio - 5) * dist_oeil_y/dist_oeil;
	} else {
		dist_oeil_x = tab_joueurs[j_id].x + 31 * ratio - (tab_balles[b_id].x + 8);
		dist_oeil_y = tab_joueurs[j_id].y + 24 * ratio - (tab_balles[b_id].y + 8);
		dist_oeil = sqrt(dist_oeil_x * dist_oeil_x + dist_oeil_y * dist_oeil_y);
		tab_joueurs[j_id].oeil_x = tab_joueurs[j_id].x + 31 * ratio - 5 - (10 * ratio - 5) * dist_oeil_x/dist_oeil;
		tab_joueurs[j_id].oeil_y = tab_joueurs[j_id].y + 24 * ratio - 5 - (10 * ratio - 5) * dist_oeil_y/dist_oeil;
	}
}

void collision_j_b(Uint8 j_id, Uint8 b_id) {
	/*ici, necessité de replacer la balle à l'endroit de la collision grâce à balle_pre_x et balle_pre_y*/
	Sint16 dist_j_x, dist_j_y;
	float dist_j;

	float xa, ya, xb, yb, a, b, c, delta, x1, x2, x, y, r_slime;

	xa = tab_balles[b_id].x + 8 - (tab_joueurs[j_id].x + tab_joueurs[j_id].taille);
	ya = tab_balles[b_id].y + 8 - (tab_joueurs[j_id].y + tab_joueurs[j_id].taille);
	xb = tab_balles[b_id].p_x + 8 - (tab_joueurs[j_id].x + tab_joueurs[j_id].taille);
	yb = tab_balles[b_id].p_y + 8 - (tab_joueurs[j_id].y + tab_joueurs[j_id].taille);

	/* Les points de la balle sont sur une droite ax + by + c = 0 */
	if(xa == xb) {
		a = 1;
		b = 0;
		c = -xb; /* Soit x = xb */
	} else {
		a = (yb - ya)/(xb - xa);
		b = -1;
		c = ya - a * xa;
	}
	delta = 4 * a*a * c*c - 4 * (b*b + a*a) * (c*c - b*b * (tab_joueurs[j_id].taille + 8)*(tab_joueurs[j_id].taille + 8));
	if(delta < 0) {
		/* On devrait probablement crasher dans ce cas là, mais bon... */
		return;
	}
	x1 = (-(2 * a * c) - sqrt(delta)) / (2 * (b*b + a*a));
	x2 = (-(2 * a * c) + sqrt(delta)) / (2 * (b*b + a*a));

	if(abs(x1 - xa) < abs(x2 - xa)) {
		x = x1;
	} else {
		x = x2;
	}

	r_slime = tab_joueurs[j_id].taille + 8;

	if(x > r_slime) {
		x = r_slime;
	} else if(-x > r_slime) {
		x = -r_slime;
	}

	y = sqrt(r_slime * r_slime - x * x);
	tab_balles[b_id].x = x + tab_joueurs[j_id].x + tab_joueurs[j_id].taille - 8;
	tab_balles[b_id].y = - y + tab_joueurs[j_id].y + tab_joueurs[j_id].taille - 8;

	if(tab_balles[b_id].y >= h_ecran - HAUT_SOL - balle_img->h) {
		tab_balles[b_id].y = h_ecran - HAUT_SOL - balle_img->h;
	}

	dist_j_x = tab_balles[b_id].x - tab_joueurs[j_id].x - tab_joueurs[j_id].taille + 8;
	dist_j_y = tab_balles[b_id].y - tab_joueurs[j_id].y - tab_joueurs[j_id].taille + 8;
	dist_j = sqrt(dist_j_x * dist_j_x + dist_j_y * dist_j_y);
	tab_balles[b_id].vx = dist_j_x * conf.v_balle_rbd / dist_j;
	tab_balles[b_id].vy = dist_j_y * conf.v_balle_rbd / dist_j;
}

Uint8 fonc_set(void) {
	/* Fonction où se déroule un set du jeu. Valeurs retournées : Voir enum plus haut. */
	Uint8 i, j;

	bool pause = false;
	SDL_Surface* pause_aff;

	/* Gestion du clavier */
	SDLKey t_act;
	bool t_enf = false;

#ifdef NET_SUPPORT
	/* Gestion du réseau */
	char reseau_rec[6];
#endif

	/* Gestion du temps */
	Uint32 temps_act, temps_prec, ecart;

	/* Gestion du jeu */
	Sint16 dist_j_x, dist_j_y, dist_j;
	Uint8 balle_act;
	Uint8 ret; /* Valeur de retour */

	/* Gestion du dessin */
	SDL_Rect dessin_rect;

	/* Gestion de l'affichage hypothétique du score et des FPS */
	Uint32 temps_fps;
	Uint8 cpt_fps;

	SDL_Surface* tmp_aff;

	char fps_chaine[10] = "\0";
	SDL_Rect score_rect = { 0, 0, 0, 0 };
	SDL_Rect fps_rect = { 0, 0, 0, 0 };

	raz_controles();

	ret = EN_COURS;
	temps_prec = temps_act = SDL_GetTicks();
	temps_fps = 0;
	cpt_fps = 0;

	son_prec = -1;

	for(;;) {
		temps_act = SDL_GetTicks();
		ecart = vitesse * (temps_act - temps_prec);
		son_id = -1;

		t_act = SDLK_UNKNOWN;
		if(SDL_PollEvent(&evenement)) {
			switch(evenement.type) {
				case SDL_QUIT:
#ifdef NET_SUPPORT
					if(nb_reseau != 0) {
						serveur_quitte();
						deconnecte_serveur();
					}
#endif
					return QUITTE;
				break;

				case SDL_KEYDOWN:
					if(evenement.key.keysym.sym == SDLK_ESCAPE) { /* Echap */
#ifdef NET_SUPPORT
						if(nb_reseau != 0) {
							serveur_quitte();
							deconnecte_serveur();
						}
#endif
						return ANNULATION;
#ifdef NET_SUPPORT
					} else if(evenement.key.keysym.sym == SDLK_SPACE && nb_reseau == 0) {
#else
					} else if(evenement.key.keysym.sym == SDLK_SPACE) {
#endif
						if(pause) {
							pause = false;
							affiche_fond(ecran);
						} else {
							pause = true;
						}
					} else if(!pause) {
						t_act = evenement.key.keysym.sym;
						t_enf = true;
					}
				break;

				case SDL_KEYUP:
					t_act = evenement.key.keysym.sym;
					t_enf = false;
					if(t_act == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
						SDL_WM_IconifyWindow();
					}
				break;

				default:
					t_act = SDLK_UNKNOWN;
				break;
			}
		}

		for(j = 0 ; j < NB_JOUEURS_T ; j++) {
			tab_joueurs[j].p_x = tab_joueurs[j].x;
			tab_joueurs[j].p_y = tab_joueurs[j].y;
		}

		/* Gestion des commandes des joueurs locaux et IA */
		for(i = 0 ; i < NB_JOUEURS_T ; i++) {
			if(tab_joueurs[i].type == CLAVIER && t_act != SDLK_UNKNOWN) {
				ctrls_clavier(i, touches[tab_joueurs[i].conf_clavier], t_act, t_enf);
			} else if(tab_joueurs[i].type == INT_ART) {
				ctrls_ia(i);
			}
		}

#ifdef NET_SUPPORT
		if(nb_reseau != 0) {
			while(reseau__srv_rec(reseau_rec, 6)) {
				if(reseau_rec[0] == 'T') {

					tab_joueurs[(Uint8)reseau_rec[1]].t_haut = reseau_rec[2];
					tab_joueurs[(Uint8)reseau_rec[1]].t_bas = reseau_rec[3];
					tab_joueurs[(Uint8)reseau_rec[1]].t_gauche = reseau_rec[4];
					tab_joueurs[(Uint8)reseau_rec[1]].t_droite = reseau_rec[5];
				} else if(reseau_rec[0] == 'Q') {
					serveur_quitte();
					deconnecte_serveur();
					return ANNULATION;
				}
			}
		}
#endif

		if(!pause) {
			/* Exécution des commandes... */
			for(i = 0 ; i < NB_JOUEURS_T ; i++) {
				if(tab_joueurs[i].type == DESACTIVE) continue;
				if(commande_joueur(i, ecart)) { /* Le joueur a demandé le changement de son skin */
					tab_joueurs[i].skin_num++;

					if((tab_joueurs[i].sens == GAUCHE && tab_joueurs[i].skin_num > img_max_jg) ||
						(tab_joueurs[i].sens == DROITE && tab_joueurs[i].skin_num > img_max_jd)) {
						tab_joueurs[i].skin_num = 1;
					}
				}
			}

			/* Déplacement... */
			for(i = 0 ; i < nb_balles; i++) {
				tab_balles[i].p_x = tab_balles[i].x;
				tab_balles[i].p_y = tab_balles[i].y;
				deplace_balle(i, ecart);
			}

			for(j = 0 ; j < NB_JOUEURS_T ; j++) {
				if(tab_joueurs[j].type == DESACTIVE) continue;
				deplace_joueur(j, ecart);
				balle_act = 0;
				for(i = 1 ; i < nb_balles; i++) {
					if (abs(tab_balles[i].x - tab_joueurs[j].x) < abs(tab_balles[balle_act].x - tab_joueurs[j].x)) {
						balle_act = i;
					}
				}
				replace_oeil(j, balle_act);
			}

			for(i = 0 ; i < nb_balles ; i++) {
				replace_balle(i);
				for(j = 0 ; j < NB_JOUEURS_T ; j++) {
					if(tab_joueurs[j].type != DESACTIVE) {
						int x = tab_balles[i].x + 8;
						int y = tab_balles[i].y + 8;
						int Jx = tab_joueurs[j].x + tab_joueurs[j].taille;
						int Jy = tab_joueurs[j].y + tab_joueurs[j].taille;
						bool collision = false;
						dist_j_x = x - (Jx);
						dist_j_y = y - (Jy);
						dist_j = sqrt(dist_j_x * dist_j_x + dist_j_y * dist_j_y);
						if(dist_j <= tab_joueurs[j].taille + 8 && y < Jy) {
							collision_j_b(j, i);

							collision = true;
						} else if((tab_joueurs[j].x < x)&&(x<Jx + tab_joueurs[j].taille)&&(y>Jy)&&(y<Jy+8)) { /* collision par en dessous */
							tab_balles[i].y = Jy;
							tab_balles[i].vy = -tab_balles[i].vy; /* remplacable par abs non? */

							collision = true;
						} else if( sqrt( (tab_joueurs[j].x - x)*(tab_joueurs[j].x - x) + dist_j_y * dist_j_y) < 8) { /* gestion coin gauche */
							dist_j_x = x - tab_joueurs[j].x;
							dist_j_y = y - Jy;
							dist_j = sqrt(dist_j_x * dist_j_x + dist_j_y * dist_j_y);
							tab_balles[i].vx = dist_j_x * conf.v_balle_rbd / dist_j;
							tab_balles[i].vy = dist_j_y * conf.v_balle_rbd / dist_j;

							collision = true;
						} else if( sqrt( (Jx + tab_joueurs[j].taille - x)*(Jx + tab_joueurs[j].taille - x) + dist_j_y * dist_j_y) < 8) { /* gestion coin droit */
							dist_j_x = x - (Jx + tab_joueurs[j].taille);
							dist_j_y = y - Jy;
							dist_j = sqrt(dist_j_x * dist_j_x + dist_j_y * dist_j_y);
							tab_balles[i].vx = dist_j_x * conf.v_balle_rbd / dist_j;
							tab_balles[i].vy = dist_j_y * conf.v_balle_rbd / dist_j;

							collision = true;
						}
						if(collision) {
							son_prec = son_id = SON_RBD_SLIME;
							multiplie_config();
						}
					}
				}
			}

			/* On vérifie si des points sont marqués */
			for(i = 0 ; i < nb_balles ; i++) {
				if(tab_balles[i].y >= h_ecran - HAUT_SOL - 16) {
					if(tab_balles[i].x <= FILET_GAUCHE - 16) { /* La balle est tombée à gauche */
						ret = (ret == GAGNE_GAUCHE || ret == MATCH_NUL ? MATCH_NUL : GAGNE_DROITE);
					} else if(tab_balles[i].x >= FILET_DROITE) {
						ret = (ret == GAGNE_DROITE || ret == MATCH_NUL ? MATCH_NUL : GAGNE_GAUCHE);
					}
				}
			}
		}

		if(ret == GAGNE_GAUCHE) {
			son_prec = son_id = SON_JG_MARQUE;
			son_tps = 0;
		} else if(ret == GAGNE_DROITE) {
			son_prec = son_id = SON_JD_MARQUE;
			son_tps = 0;
		}

#ifdef NET_SUPPORT

		if(nb_reseau != 0) {
			paquet->len = 0;
			ECRIT_8(ret == EN_COURS ? 'E' : 'S');
			
			ECRIT_8(son_tps);
			if(son_tps < 127) son_tps++;
			ECRIT_8(son_prec);

			for(i = 0 ; i < NB_JOUEURS_T ; i++) {
				if(tab_joueurs[i].type == DESACTIVE) continue;
				ECRIT_16((Sint16)tab_joueurs[i].x);
				ECRIT_16((Sint16)tab_joueurs[i].y);
				ECRIT_8((Sint16)tab_joueurs[i].skin_num);
				ECRIT_16((Sint16)tab_joueurs[i].oeil_x);
				ECRIT_16((Sint16)tab_joueurs[i].oeil_y);
			}

			for(i = 0 ; i < nb_balles ; i++) {
				ECRIT_16((Sint16)tab_balles[i].x);
				ECRIT_16((Sint16)tab_balles[i].y);
			}

			if(ret != EN_COURS) {
				ECRIT_8(score_gauche + (ret == GAGNE_GAUCHE ? 1 : 0));
				ECRIT_8(score_droite + (ret == GAGNE_DROITE ? 1 : 0));
			}

			reseau_srv_env();
		}
#endif

		/* Dessin ! */
		for(i = 0 ; i < NB_JOUEURS_T ; i++) {
			if(tab_joueurs[i].type == DESACTIVE) continue;
			fond_partiel(ecran, tab_joueurs[i].p_x, tab_joueurs[i].p_y, tab_joueurs[i].taille * 2, tab_joueurs[i].taille);
		}

		for(i = 0 ; i < nb_balles ; i++) {
			fond_partiel(ecran, tab_balles[i].p_x, (tab_balles[i].p_y < 0 ? 0 : tab_balles[i].p_y), 16, 16);
		}

		if(aff_fps && fps_rect.w != 0) {
			fond_partiel(ecran, fps_rect.x, fps_rect.y, fps_rect.w, fps_rect.h);
		}

		if(aff_sc_perm && score_rect.w != 0) {
			fond_partiel(ecran, score_rect.x, score_rect.y, score_rect.w, score_rect.h);
		}

		for(i = 0 ; i < NB_JOUEURS_T ; i++) {
			if(tab_joueurs[i].type == DESACTIVE) continue;
			dessin_rect.x = tab_joueurs[i].x;
			dessin_rect.y = tab_joueurs[i].y;
			SDL_BlitSurface(tab_joueurs[i].skin_tab[tab_joueurs[i].skin_num], NULL, ecran, &dessin_rect);
			dessin_rect.x = tab_joueurs[i].oeil_x;
			dessin_rect.y = tab_joueurs[i].oeil_y;
			SDL_BlitSurface(oeil, NULL, ecran, &dessin_rect);
		}

		for(i = 0 ; i < nb_balles ; i++) {
			if(tab_balles[i].y < -balle_img->h) {
				dessin_rect.x = tab_balles[i].x;
				dessin_rect.y = 0;
				SDL_BlitSurface(fleche, NULL, ecran, &dessin_rect);
			} else {
				dessin_rect.x = tab_balles[i].x;
				dessin_rect.y = tab_balles[i].y;
				SDL_BlitSurface(balle_img, NULL, ecran, &dessin_rect);
			}
		}

		if(pause) {
			pause_aff = TTF_RenderUTF8_Blended(police_menu, _("Paused"), coul_txt_jeu);

			dessin_rect.x = (LARG_ECRAN - pause_aff->w) / 2;
			dessin_rect.y = (h_ecran - pause_aff->h) / 2;
			dessin_rect.w = pause_aff->w;
			dessin_rect.h = pause_aff->h;

			SDL_BlitSurface(pause_aff, NULL, ecran, &dessin_rect);
			SDL_FreeSurface(pause_aff);
		}

		if(son_id >= 0) {
			joue_son(son_id);
			son_tps = 0;
		}

		if(ret != EN_COURS) return ret; /* Le SDL_Flip sera appelé ultérieurement */

		if(aff_fps && fps_chaine[0] != '\0') {
			tmp_aff = TTF_RenderUTF8_Blended(police, fps_chaine, coul_txt_jeu);
			fps_rect.w = tmp_aff->w;
			fps_rect.h = tmp_aff->h;
			SDL_BlitSurface(tmp_aff, NULL, ecran, &fps_rect);
			SDL_FreeSurface(tmp_aff);
		}

		if(aff_sc_perm) {
			tmp_aff = TTF_RenderUTF8_Blended(police, score_chaine, coul_txt_jeu);
			score_rect.w = tmp_aff->w;
			score_rect.h = tmp_aff->h;
			score_rect.x = LARG_ECRAN - tmp_aff->w - 10;
			SDL_BlitSurface(tmp_aff, NULL, ecran, &score_rect);
			SDL_FreeSurface(tmp_aff);
		}

		SDL_Flip(ecran);

		if(SDL_GetTicks() - temps_prec < (2000/MAX_FPS)) { /* Il faut attendre un peu */
			SDL_Delay((2000/MAX_FPS) - (SDL_GetTicks() - temps_prec));
		}

		temps_prec = temps_act;
		cpt_fps++;
		if((temps_act - temps_fps) >= 1000) {
			if(cpt_fps > 1) sprintf(fps_chaine, "FPS : %d", cpt_fps);
			cpt_fps = 0;
			temps_fps = temps_act;
		}
	}

	return EN_COURS; /* Jamais atteint */
}

void aff_score(void) {
	SDL_Surface* score_aff;
	SDL_Rect pos_centre;

	snprintf5(score_chaine, 15, _("Score: %d - %d"), score_gauche, score_droite);

	score_aff = TTF_RenderUTF8_Blended(police_menu, score_chaine, coul_txt_jeu);

	pos_centre.x = (LARG_ECRAN - score_aff->w) / 2;
	pos_centre.y = (h_ecran - score_aff->h) / 2;
	pos_centre.w = score_aff->w;
	pos_centre.h = score_aff->h;

	SDL_BlitSurface(score_aff, NULL, ecran, &pos_centre);
	SDL_FreeSurface(score_aff);

	SDL_Flip(ecran);
}

void fin_jeu(bool gagne_droite) {
	/* Est appelée à la fin du match, anime les joueurs (gagnants). */

	Uint8 i, debut, fin;

	/* Gestion du temps */
	Uint32 temps_act, temps_prec, ecart;

	/* Gestion du dessin */
	SDL_Rect dessin_rect;

#ifdef NET_SUPPORT
	/* Gestion du réseau */
	char reseau_rec[5];

#endif

	debut = (gagne_droite ? NB_JOUEURS_G : 0);
	fin = (gagne_droite ? NB_JOUEURS_T : NB_JOUEURS_G);

	for(i = debut ; i < fin ; i++) {
		tab_joueurs[i].vx = 0;
	}

	joue_son(son_id);
	son_tps = 0;

	temps_prec = SDL_GetTicks();
	for(;;) {
		temps_act = SDL_GetTicks();
		ecart = temps_act - temps_prec;

		if(SDL_PollEvent(&evenement)) {
			switch(evenement.type) {
				case SDL_QUIT:
#ifdef NET_SUPPORT
					if(nb_reseau != 0) {
						serveur_quitte();
						deconnecte_serveur();
					}
#endif
					exit(EXIT_SUCCESS);
				break;

				case SDL_KEYDOWN:
					if((evenement.key.keysym.sym == SDLK_ESCAPE)||(evenement.key.keysym.sym == SDLK_RETURN)||(evenement.key.keysym.sym == SDLK_SPACE)) { /* Echap */
#ifdef NET_SUPPORT
						if(nb_reseau != 0) {
							serveur_quitte();
							deconnecte_serveur();
						}
#endif
						return;
					}
				break;

				default:
				break;
			}
		}

#ifdef NET_SUPPORT
		if(nb_reseau != 0) {
			while(reseau__srv_rec(reseau_rec, 5)) {
				if(reseau_rec[0] == 'Q') {
					serveur_quitte();
					deconnecte_serveur();
					return;
				}
			}
		}
#endif

		/* Sauvegarde des positions et configuration des sauts */
		for(i = debut ; i < fin ; i++) {
			if(tab_joueurs[i].type == DESACTIVE) continue;
			tab_joueurs[i].p_x = tab_joueurs[i].x;
			tab_joueurs[i].p_y = tab_joueurs[i].y;

			if(tab_joueurs[i].y >= tab_joueurs[i].lim_bas) {
				tab_joueurs[i].y = tab_joueurs[i].lim_bas;
				tab_joueurs[i].vy = conf.saut_vy * (1 + .5 * rand()/(float)RAND_MAX);
			}

			deplace_joueur(i, ecart);

			if(tab_joueurs[i].sens == GAUCHE) {
				tab_joueurs[i].oeil_x = tab_joueurs[i].x + 69 * tab_joueurs[i].taille/50 - 5;
			} else {
				tab_joueurs[i].oeil_x = tab_joueurs[i].x + 31 * tab_joueurs[i].taille/50 - 5;
			}

			tab_joueurs[i].oeil_y = tab_joueurs[i].y + 24.0 * (float)tab_joueurs[i].taille/50.0 - 5;
		}

#ifdef NET_SUPPORT

		if(nb_reseau != 0) {
			paquet->len = 0;
			ECRIT_8('S');
			ECRIT_8(son_tps);			
			if(son_tps < 127) son_tps++;
			ECRIT_8(son_prec);
			for(i = 0 ; i < NB_JOUEURS_T ; i++) {
				if(tab_joueurs[i].type == DESACTIVE) continue;
				ECRIT_16((Sint16)tab_joueurs[i].x);
				ECRIT_16((Sint16)tab_joueurs[i].y);
				ECRIT_8((Sint16)tab_joueurs[i].skin_num);
				ECRIT_16((Sint16)tab_joueurs[i].oeil_x);
				ECRIT_16((Sint16)tab_joueurs[i].oeil_y);
			}

			for(i = 0 ; i < nb_balles ; i++) {
				ECRIT_16((Sint16)tab_balles[i].x);
				ECRIT_16((Sint16)tab_balles[i].y);
			}

			ECRIT_8(score_gauche);
			ECRIT_8(score_droite);

			reseau_srv_env();
		}
#endif

		/* Dessin ! Copier/coller de la fonction principale... */
		for(i = 0 ; i < NB_JOUEURS_T ; i++) {
			if(tab_joueurs[i].type == DESACTIVE) continue;
			fond_partiel(ecran, tab_joueurs[i].p_x, tab_joueurs[i].p_y, tab_joueurs[i].taille * 2, tab_joueurs[i].taille);
		}

		for(i = 0 ; i < nb_balles ; i++) {
			fond_partiel(ecran, tab_balles[i].p_x, (tab_balles[i].p_y < 0 ? 0 : tab_balles[i].p_y), 16, 16);
		}


		for(i = 0 ; i < NB_JOUEURS_T ; i++) {
			if(tab_joueurs[i].type == DESACTIVE) continue;
			dessin_rect.x = tab_joueurs[i].x;
			dessin_rect.y = tab_joueurs[i].y;
			SDL_BlitSurface(tab_joueurs[i].skin_tab[tab_joueurs[i].skin_num], NULL, ecran, &dessin_rect);
			dessin_rect.x = tab_joueurs[i].oeil_x;
			dessin_rect.y = tab_joueurs[i].oeil_y;
			SDL_BlitSurface(oeil, NULL, ecran, &dessin_rect);
		}

		for(i = 0 ; i < nb_balles ; i++) {
			if(tab_balles[i].y < -balle_img->h) {
				dessin_rect.x = tab_balles[i].x;
				dessin_rect.y = 0;
				SDL_BlitSurface(fleche, NULL, ecran, &dessin_rect);
			} else {
				dessin_rect.x = tab_balles[i].x;
				dessin_rect.y = tab_balles[i].y;
				SDL_BlitSurface(balle_img, NULL, ecran, &dessin_rect);
			}
		}

		aff_score();

		SDL_Flip(ecran);

		if(SDL_GetTicks() - temps_prec < (2000/MAX_FPS)) { /* Il faut attendre un peu */
			SDL_Delay((2000/MAX_FPS) - (SDL_GetTicks() - temps_prec));
		}
		temps_prec = temps_act;
	}
}

void jeu_srv(void) {
	/* Fonction d'exécution du jeu en local ou en tant que serveur. */
	Uint8 retour;
	bool cote_service;

	init_jeu();

#ifdef NET_SUPPORT
		nb_reseau = 0;
		if(!connecte_joueurs()) return;
#endif

	retour = 0;
	evenement.type = 0;

	cote_service = (rand() < ((float)RAND_MAX / 2.0) ? true : false);

	for(;;) {
		memcpy(&conf, &configs[act_conf], sizeof(config_slime)); /* On règle la configuration */
		vitesse = VITESSE;
		place_joueurs_et_balles(cote_service);
		affiche_fond(ecran);

		retour = fonc_set();

		switch(retour) {
			case GAGNE_GAUCHE:
				score_gauche++;
				cote_service = false; /* A gauche */
			break;

			case GAGNE_DROITE:
				score_droite++;
				cote_service = true; /* A droite */
			break;

			case MATCH_NUL:
			break;

			case ANNULATION:
				return;
			break;

			case QUITTE:
				exit(EXIT_SUCCESS);
			break;

			default: /* ????? */
				fprintf(stderr, "ERR : Incorrect set return...\n");
				exit(EXIT_FAILURE);
			break;
		}

		aff_score();
		son_prec = -1;

		if(score_gauche > 9) {
			son_prec = son_id = SON_JG_GAGNE;
			fin_jeu(false);
			return;
		} else if(score_droite > 9) {
			son_prec = son_id = SON_JD_GAGNE;
			fin_jeu(true);
			return;
		}

		SDL_Delay(1000);
	}
}

#ifdef NET_SUPPORT
void rst_conf(void) {
	/* Après le jeu client, les configurations joueur sont incorrectes. Il faut les rétablir. */
	Uint8 i;

	for(i = 1 ; i < NB_JOUEURS_T ; i++) {
		tab_joueurs[i].type = DESACTIVE;
	}

	tab_joueurs[0].type = CLAVIER;
	tab_joueurs[0].conf_clavier = 0;
	tab_joueurs[NB_JOUEURS_G].type = CLAVIER;
	tab_joueurs[NB_JOUEURS_G].conf_clavier = 1;
}

void jeu_clt(void) {
	/* Fonction d'exécution du jeu sur un client. */

	static char ip_chaine[50] = "\0";
	char rec_chaine[128];
	Uint8 i, pos, skin_num;
	bool raf_tout;

	/* Gestion du clavier */
	SDLKey t_act;
	bool t_enf = false;

	/* Gestion du dessin */
	SDL_Rect dessin_rect;

	SDL_Rect score_rect = { 0, 0, 0, 0 };
	SDL_Surface* tmp_aff;

	if(!prompt_ip(_("Server address:"), ip_chaine)) {
		return;
	}

	nb_balles = connecte_client(ip_chaine);

	if(nb_balles == 0) {
		nb_balles = 1;
		return;
	}

	raf_tout = true;
	snprintf5(score_chaine, 15, _("Score: %d - %d"), score_gauche, score_droite);

	for(;;) {
		SDL_Delay(20);

		t_act = SDLK_UNKNOWN;
		while(SDL_PollEvent(&evenement)) {
			switch(evenement.type) {
				case SDL_QUIT:
					client_quitte();
					deconnecte_client();
					rst_conf();
					nb_balles = 1;
					exit(EXIT_SUCCESS);
				break;

				case SDL_KEYDOWN:
					if(evenement.key.keysym.sym == SDLK_ESCAPE) { /* Echap */
						client_quitte();
						deconnecte_client();
						rst_conf();
						nb_balles = 1;
						return;
					}

					t_act = evenement.key.keysym.sym;
					t_enf = true;
				break;

				case SDL_KEYUP:
					t_act = evenement.key.keysym.sym;
					t_enf = false;
				break;

				default:
					t_act = SDLK_UNKNOWN;
				break;
			}
			ctrls_clavier(0, touches[0], t_act, t_enf);
		}

		paquet->len = 0;
		ECRIT_8('T');
		ECRIT_8(clt_id_joueur);
		ECRIT_8(tab_joueurs[0].t_haut);
		ECRIT_8(tab_joueurs[0].t_bas);
		ECRIT_8(tab_joueurs[0].t_gauche);
		ECRIT_8(tab_joueurs[0].t_droite);
		reseau_clt_env();

		if(!reseau__clt_rec(rec_chaine, 128)) continue;

		while(reseau__clt_rec(rec_chaine, 128)) {} /* Evite l'accumulation de paquets. */

		if(rec_chaine[0] == 'Q') break;
		if(rec_chaine[0] != 'E' && rec_chaine[0] != 'S') continue;

		if(raf_tout) {
			raf_tout = false;
			affiche_fond(ecran);
		} else {
			for(i = 0 ; i < NB_JOUEURS_T ; i++) {
				if(tab_joueurs[i].type == DESACTIVE) continue;
				fond_partiel(ecran, tab_joueurs[i].p_x, tab_joueurs[i].p_y, tab_joueurs[i].taille * 2, tab_joueurs[i].taille);
			}

			for(i = 0 ; i < nb_balles ; i++) {
				fond_partiel(ecran, tab_balles[i].p_x, (tab_balles[i].p_y < -balle_img->h ? 0 : tab_balles[i].p_y), 16, 16);
			}
		}

		if(aff_sc_perm && score_rect.w != 0) {
			fond_partiel(ecran, score_rect.x, score_rect.y, score_rect.w, score_rect.h);
		}

		if((Uint8)rec_chaine[1] < son_tps && (Sint8)rec_chaine[2] >= 0) joue_son((Sint8)rec_chaine[2]);
		son_tps = rec_chaine[1];

		pos = 3;
		for(i = 0 ; i < nb_total ; i++) {
			dessin_rect.x = tab_joueurs[i].p_x = (Sint16)SDLNet_Read16(rec_chaine + pos);
			pos += 2;

			dessin_rect.y = tab_joueurs[i].p_y = (Sint16)SDLNet_Read16(rec_chaine + pos);
			pos += 2;			

			skin_num = rec_chaine[pos++];

			if(tab_joueurs[i].sens == GAUCHE) {
				if(tab_joueurs[i].taille == 50) {
					SDL_BlitSurface(img_grand_jg[(skin_num <= img_max_jg ? skin_num : 0)], NULL, ecran, &dessin_rect);
				} else if(tab_joueurs[i].taille == 40) { /* Ratio .8 */
					SDL_BlitSurface(img_2J_jg[(skin_num <= img_max_jg ? skin_num : 0)], NULL, ecran, &dessin_rect);
				} else {
					SDL_BlitSurface(img_3J_jg[(skin_num <= img_max_jg ? skin_num : 0)], NULL, ecran, &dessin_rect);
				}
			} else {
				if(tab_joueurs[i].taille == 50) {
					SDL_BlitSurface(img_grand_jd[(skin_num <= img_max_jd ? skin_num : 0)], NULL, ecran, &dessin_rect);
				} else if(tab_joueurs[i].taille == 40) { /* Ratio .8 */
					SDL_BlitSurface(img_2J_jd[(skin_num <= img_max_jd ? skin_num : 0)], NULL, ecran, &dessin_rect);
				} else {
					SDL_BlitSurface(img_3J_jd[(skin_num <= img_max_jd ? skin_num : 0)], NULL, ecran, &dessin_rect);
				}
			}

			dessin_rect.x = (Sint16)SDLNet_Read16(rec_chaine + pos);
			pos += 2;

			dessin_rect.y = (Sint16)SDLNet_Read16(rec_chaine + pos);
			pos += 2;

			SDL_BlitSurface(oeil, NULL, ecran, &dessin_rect);
		}

		for(i = 0 ; i < nb_balles ; i++) {
			dessin_rect.x = tab_balles[i].p_x = (Sint16)SDLNet_Read16(rec_chaine + pos);
			pos += 2;

			dessin_rect.y = tab_balles[i].p_y = (Sint16)SDLNet_Read16(rec_chaine + pos);
			pos += 2;
			if(dessin_rect.y < -balle_img->h) {
				dessin_rect.y = 0;
				SDL_BlitSurface(fleche, NULL, ecran, &dessin_rect);
			} else {
				SDL_BlitSurface(balle_img, NULL, ecran, &dessin_rect);
			}
		}

		if(rec_chaine[0] == 'S') {
			score_gauche = rec_chaine[pos++];
			score_droite = rec_chaine[pos];
			aff_score();
			raf_tout = true;
		} else if(aff_sc_perm) {
			tmp_aff = TTF_RenderUTF8_Blended(police, score_chaine, coul_txt_jeu);
			score_rect.w = tmp_aff->w;
			score_rect.h = tmp_aff->h;
			score_rect.x = LARG_ECRAN - tmp_aff->w - 10;
			SDL_BlitSurface(tmp_aff, NULL, ecran, &score_rect);
			SDL_FreeSurface(tmp_aff);
		}

		SDL_Flip(ecran);
	}

	deconnecte_client();
	rst_conf();
	nb_balles = 1;
}
#endif
