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

$Id: IA.c 294 2010-05-15 14:08:10Z vinduv $
*/

#include <stdbool.h>
#include <math.h>

#include "slime.h"
#include "jeu.h"

#define DELTA(i, sol) ((tab_balles[(i)].vy * tab_balles[(i)].vy) - 2 * conf.gravite * (tab_balles[(i)].y - (sol)))
#define T(i, delta) ((-tab_balles[(i)].vy + sqrt((delta))) / conf.gravite)
#define T2(i, filet) (((filet) - tab_balles[(i)].x)/tab_balles[(i)].vx)
//#define NIVEAU 2/4 // Niveau NAZE = 0, FACILE = 1/4, DOUÉE = 1/2, INFAILLIBLE = 1
#define SEUIL 0.45

void ctrls_ia_gauche(Uint8 id, float t, int bal_min, Sint16 px, int nb_slimes);

/* y = g * t * t / 2  + vy  * t + y
 *	y = lim_bas <=> g*t *t/2 + vy * t + (y - lim_bas) = 0
 *	delta = vy * vy  - 2 * g * (y - lim_bas)
 *	t = (-vy + sqrt(delta))/g
 *	x = vx  * t + x  */

const float niv_ai[NB_NIVEAUX] = {
 0,
 1./4.,
 1./2.,
 1
};

void ctrls_ia(Uint8 id) {
	int i,j = 0,delta;
	Sint16 px;
	int id_balles[MAX_BALLES]; /* tableau des id des balles arrivant de notre coté, trié par ordre d'arrivée au sol */
	float t_balles[MAX_BALLES]; /* tableau du temps avant impact avec le sol pour chaque balle */
	Sint16 px_balles[MAX_BALLES]; /* le tableau des px des balles */
	int balles = 0; /* nombre de balles qui tombent de notre coté */
	bool fin; /* pour le tri-bulles/bubble-sort */
	int nb_slimes = 0; /* nombre de slimes dans l'équipe */
	int bal_min; /* bal_min = balle traitée par la slime actuelle */
	int place = 0; /* la place sert à définir la position par rapport à l'équipe */
	float t;
	int debut=(tab_joueurs[id].sens==GAUCHE?0:NB_JOUEURS_G); /* debut est l'id de la première slime de l'équipe */

	for (i = 0;i < nb_balles;i++) { /* le nb_balles est le nombre de balles, défini dans slime.h */
		delta = DELTA(i, tab_joueurs[id].lim_bas + tab_joueurs[id].taille/4);
		t = (tab_balles[i].y>(tab_joueurs[id].lim_bas + tab_joueurs[id].taille/4)?0:T(i, delta));
		/*printf("t : %.2f\n",t);*/
		t_balles[i] = (t>SEUIL?SEUIL + niv_ai[tab_joueurs[id].niveau]*(t-SEUIL):t);
	}

	for (i = 0; i < nb_balles; i++) {
		px = tab_balles[i].x + tab_balles[i].vx * t_balles[i];
		px = (px>0?px:-px);
		px = (px<LARG_ECRAN?px:2*LARG_ECRAN - px); /* on gère les rebonds contre les murs */
		if((((px<FILET_DROITE)&&(tab_joueurs[id].sens==DROITE))||
		(((px+16)>FILET_GAUCHE)&&(tab_joueurs[id].sens==GAUCHE)))&&(tab_balles[i].vx != 0)) { /* le px est en face */
			t = T2(i,(tab_joueurs[id].sens==DROITE?FILET_DROITE:FILET_GAUCHE));
			if(t>0) {
				j = tab_balles[i].y + tab_balles[i].vy * t + t * (t/2) * conf.gravite; /* y qu'aura la balle au niveau du filet */
				if(j+16>HAUT_ECRAN - HAUT_SOL - FILET_HAUT) /* la balle rebondi contre le filet */
					px = 2*FILET_DROITE - px;
			}
		}
		px_balles[i] = px;
	}

	for(i=debut;i<debut+NB_JOUEURS_G;i++) {/* on compte le nombre de slimes de notre équipe */
		nb_slimes += (tab_joueurs[i].type!=DESACTIVE);
	}

	j = 0;
	for (i = 0;i < nb_balles;i++) {
		px = px_balles[i];
		if ( /* attention : le code ci dessous ne marchera pas quand la balle arrive de derrière pour rebondir sur le coté du filet */
		((px>FILET_DROITE)&&(tab_joueurs[id].sens==DROITE))||
		(((px+16)<FILET_GAUCHE)&&(tab_joueurs[id].sens==GAUCHE)) /* on utilise FILET_GAUCHE car px est l'extrémité gauche de la balle */
		) { /* si la balle va tomber de notre coté */
			id_balles[j] = i;
			j++;
		}
	} /* on obtient id_balles mais pas encore trié */

	balles = j;
	fin = false;
	if(balles > 1) {

	while (!fin) {
		fin = true;
		for(i = 0; i < balles-1;i++) {
			if(t_balles[id_balles[i]]>t_balles[id_balles[i+1]]) {
				j = id_balles[i];
				id_balles[i] = id_balles[i+1];
				id_balles[i+1] = j;
				fin = false;
			}
		}
	} /* id_balles est trié par tps avant impact */

	fin = false;
	while (!fin) {
		fin = true;
		for(i = 0; i < (balles<nb_slimes?balles-1:nb_slimes-1);i++) {
			if(px_balles[id_balles[i]]>px_balles[id_balles[i+1]]) {
				j = id_balles[i];
				id_balles[i] = id_balles[i+1];
				id_balles[i+1] = j;
				fin = false;
			}
		}
	}/* les nb_slimes premières balles à tomber sont triées par px */
	}

	for (i=0;i<nb_slimes;i++) {
		if((tab_joueurs[debut+i].x < tab_joueurs[id].x)||((tab_joueurs[debut+i].x == tab_joueurs[id].x)&&(debut+i < id)))
			place++;
	}

	bal_min = (balles>0? id_balles[(place>=balles?balles-1:place)] :0);
	t  = t_balles[bal_min];
	px = px_balles[bal_min];

	tab_joueurs[id].t_haut = false;
	tab_joueurs[id].t_droite = false;
	tab_joueurs[id].t_gauche = false;

	if(tab_joueurs[id].sens == GAUCHE) {
		ctrls_ia_gauche(id,t,bal_min,px,nb_slimes);
		return;
	}


	if(px > FILET_GAUCHE) { /* la balle va tomber de notre coté */
		if((tab_balles[bal_min].x > FILET_GAUCHE) && (tab_balles[bal_min].vy > 0) && ((fabs((tab_joueurs[id].x + tab_joueurs[id].taille) - (px + 8)) - tab_joueurs[id].taille - 8) > (1000 * t * conf.dep_x))) { /* la balle est de notre coté && elle est en train de tomber && on ne l'atteindra pas à temps sans sauter */
			tab_joueurs[id].t_haut = true;
			tab_joueurs[id].t_gauche = (px < tab_joueurs[id].x ? true : false);
			tab_joueurs[id].t_droite = (px + tab_joueurs[id].taille > tab_joueurs[id].x ? true : false);
		} else if(
			(px + 8 <= tab_joueurs[id].x + tab_joueurs[id].taille/4)/* on est pas dessous */
			||((tab_joueurs[id].x >= LARG_ECRAN - tab_joueurs[id].taille * 2 - 4) && (abs(tab_balles[bal_min].vx) < 3))/* La balle est bloquée au dessus de notre tête, on avance un peu*/
			) { /* Gauche */
				tab_joueurs[id].t_gauche = true;
		} else if(
		(px + 16 > tab_joueurs[id].x + tab_joueurs[id].taille) /* on est pas dessous */
		) { /* Droite */
			tab_joueurs[id].t_droite = true;
		}
	} else { /* la balle est en face, on se replace */
		if((tab_joueurs[id].x >= LARG_ECRAN - tab_joueurs[id].taille * 2 - 2) && (abs(tab_balles[bal_min].vx) < 3)) {
			tab_joueurs[id].t_gauche = true;
		} else if((id%NB_JOUEURS_G+1.0)*FILET_GAUCHE/(nb_slimes+1) + FILET_GAUCHE - tab_joueurs[id].taille * 2> tab_joueurs[id].x) {
			tab_joueurs[id].t_droite = true;
		}
	}
}

void ctrls_ia_gauche(Uint8 id, float t, int bal_min, Sint16 px, int nb_slimes) {

	if(px < FILET_DROITE) { /* la balle va tomber de notre coté */
		if((tab_balles[bal_min].x < FILET_DROITE) && (tab_balles[bal_min].vy > 0) && ((fabs((tab_joueurs[id].x + tab_joueurs[id].taille) - (px + 8)) - tab_joueurs[id].taille - 8) > (1000 * t * conf.dep_x))) { /* la balle est de notre coté && elle est en train de tomber && on ne l'atteindra pas à temps sans sauter */
			tab_joueurs[id].t_haut = true;
			tab_joueurs[id].t_gauche = (px < tab_joueurs[id].x ? true : false);
			tab_joueurs[id].t_droite = (px + tab_joueurs[id].taille > tab_joueurs[id].x ? true : false);
		} else if(
			(px + 8 > tab_joueurs[id].x + (7.0/4)*tab_joueurs[id].taille ) /* = j.x +2*j.taille - j.taille/4 */ /* on est pas dessous */
			|| ((tab_joueurs[id].x < 4) && (abs(tab_balles[bal_min].vx) < 3)) /* La balle est bloquée au dessus de notre tête, on avance un peu*/
			){ /* Droite */
				tab_joueurs[id].t_droite = true;
		} else if (
			(px < tab_joueurs[id].x + tab_joueurs[id].taille) /* on est pas dessous */
			){ /* Gauche */
				tab_joueurs[id].t_gauche = true;

		}
	} else {/* la balle est en face, on se replace au milieu */
		if((id%NB_JOUEURS_G+1.0)*FILET_DROITE/(nb_slimes+1) + tab_joueurs[id].taille< tab_joueurs[id].x) {
			tab_joueurs[id].t_gauche = true;
		} else if((px > FILET_DROITE )&&((id%NB_JOUEURS_G+1.0)*FILET_DROITE/(nb_slimes + 1)> tab_joueurs[id].x)) {
			tab_joueurs[id].t_droite = true;
		}
	}

}
