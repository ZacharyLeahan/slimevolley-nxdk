/*
This file is part of Slime Volley.

	Slime Volley is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Slime Volley is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Slime Volley. If not, see <http://www.gnu.org/licenses/>.

Copyright (c)MCMic, VinDuv.

$Id: reseau.c 272 2008-12-23 08:44:22Z vinduv $
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "config.h"
#include "slime.h"
#include "reseau.h"
#include "reseau_inter.h"

#ifdef NET_SUPPORT

#define PORT 2222 /* A changer aussi dans les messages */
#define HANDSHAKE "HelloSV241"

UDPsocket res_socket;
IPaddress j_adresses[NB_JOUEURS_T];

/* Pour le client */
IPaddress clt_dist_adresse;

void init_reseau(void) {
	/* Initialisation de SDL_net */
	if(SDLNet_Init() < 0) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	paquet = SDLNet_AllocPacket(256);
	if(!paquet)	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
}

bool connecte_joueurs(void) {
	/* Au lancement d'une partie, attend que tous les joueurs réseau soient connectés */
	Uint8 i, cpt;

	Uint32 temps;

	char chaine_connectes[32];

	nb_total = 0;
	nb_reseau = 0;
	for(i = 0 ; i < 6 ; i++) {
		if(tab_joueurs[i].type != DESACTIVE) nb_total++;
		if(tab_joueurs[i].type == DISTANT) nb_reseau++;
	}

	if(nb_reseau == 0) return true;

	/* Initialisation du socket */
	res_socket = SDLNet_UDP_Open(PORT);
	if(!res_socket) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	cpt = 0;
	for(i = 0 ; i < 6 ; i++) {
		if(tab_joueurs[i].type != DISTANT) continue;

		/* Et on attend la connexion... */
		sprintf(chaine_connectes, _("Connected: %d/%d"), cpt, nb_reseau);
		prep_attente(_("Waiting for remote players..."), chaine_connectes, _("Press Escape to cancel."));
		for(;;) {
			attente();

			if(SDL_PollEvent(&evenement)) {
				if(evenement.type == SDL_QUIT) {
					deconnecte_serveur();
					exit(EXIT_SUCCESS);
				} else if(evenement.type == SDL_KEYDOWN && evenement.key.keysym.sym == SDLK_ESCAPE) {
					deconnecte_serveur();
					return false;
				} else if(evenement.type == SDL_KEYUP && evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
					SDL_WM_IconifyWindow();
				}
			}

			SDLNet_UDP_Unbind(res_socket, 0);

			if(!SDLNet_UDP_Recv(res_socket, paquet)) continue;

			if(paquet->len == 1 && paquet->data[0] == 'Q') {
				serveur_quitte();
				deconnecte_serveur();
				return false;
			}

			if(strncmp((char *)paquet->data, HANDSHAKE, strlen(HANDSHAKE))) {
				/* Le client n'est pas compatible... on le jette */
				printf("Client incompatible rejeté : %s\n", (char *)paquet->data);
				continue;
			}

			memcpy(&j_adresses[i], &paquet->address, sizeof(IPaddress));

			if(SDLNet_UDP_Bind(res_socket, (i < NB_JOUEURS_G ? 1 : 2), &j_adresses[i]) == -1) {
				fprintf(stderr, "SDLNet_UDP_Bind: %s\n",SDLNet_GetError());
				exit(EXIT_FAILURE);
			}

			if(SDLNet_UDP_Bind(res_socket, 3, &j_adresses[i]) == -1) {
				fprintf(stderr, "SDLNet_UDP_Bind: %s\n",SDLNet_GetError());
				exit(EXIT_FAILURE);
			}

			paquet->len = 0;
			ECRIT_8('H');
			ECRIT_8(i);
			if(SDLNet_UDP_Send(res_socket, 3, paquet) == 0) {
				fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
			}

			SDLNet_UDP_Unbind(res_socket, 3);

			break;
		}

		cpt++;
	}

	/* Délai avant le début de la partie */
	temps = SDL_GetTicks() + 5000;

	sprintf(chaine_connectes, _("Connected: %d/%d"), cpt, nb_reseau);
	prep_attente(_("Get ready..."), chaine_connectes, NULL);

	while(SDL_GetTicks() < temps) {
		attente();
		if(SDL_PollEvent(&evenement)) {
			if(evenement.type == SDL_QUIT) {
				deconnecte_serveur();
				exit(EXIT_SUCCESS);
			} else if(evenement.type == SDL_KEYDOWN && evenement.key.keysym.sym == SDLK_ESCAPE) {
				deconnecte_serveur();
				return false;
			} else if(evenement.type == SDL_KEYUP && evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
				SDL_WM_IconifyWindow();
			}
		}
		SDL_Delay(100);
	}

	paquet->len = 0;
	ECRIT_8('N');
	ECRIT_8(nb_total);
	for(i = 0 ; i < 6 ; i++) {
		if(tab_joueurs[i].type == DESACTIVE) continue;

		ECRIT_8(tab_joueurs[i].sens);
		ECRIT_8(tab_joueurs[i].taille);
	}
	ECRIT_8(nb_balles);

	reseau_srv_env();

	return true;
}

void deconnecte_serveur(void) {
	if(res_socket != NULL) {
		SDLNet_UDP_Unbind(res_socket, 1);
		SDLNet_UDP_Unbind(res_socket, 2);
		SDLNet_UDP_Unbind(res_socket, 3);
		SDLNet_UDP_Close(res_socket);
		res_socket = NULL;
	}
}

bool reseau__clt_rec(char* chaine, Uint8 n) {
	if(SDLNet_UDP_Recv(res_socket, paquet) && (paquet->channel == 1 || paquet->channel == 2) && paquet->len <= n) {
		memcpy(chaine, paquet->data, paquet->len);

		return true;
	} else {
		return false;
	}
}

void reseau_clt_env(void) {
	if(SDLNet_UDP_Send(res_socket, 0, paquet) == 0) {
		fprintf(stderr, "reseau_clt_env():SDLNet_UDP_Send: %s\n", SDLNet_GetError());
	}
}

bool reseau__srv_rec(char* chaine, Uint8 n) {
	if(SDLNet_UDP_Recv(res_socket, paquet) && (paquet->channel == 1 || paquet->channel == 2) && paquet->len <= n) {
		memcpy(chaine, paquet->data, paquet->len);
		return true;
	}
	return false;
}

void reseau_srv_env(void) {
	SDLNet_UDP_Send(res_socket, 1, paquet);
	SDLNet_UDP_Send(res_socket, 2, paquet);

	return;
}

Uint8 config_client(void) {
	Uint8 i, pos;

	/* Est appelée après que le client ait été connecté au serveur. */
	while(!SDLNet_UDP_Recv(res_socket, paquet) || paquet->data[0] != 'N') {
		attente();
		if(SDL_PollEvent(&evenement)) {
			if(evenement.type == SDL_QUIT) {
				client_quitte();
				exit(EXIT_SUCCESS);
			} else if(evenement.type == SDL_KEYDOWN && evenement.key.keysym.sym == SDLK_ESCAPE) {
				client_quitte();
				deconnecte_client();
				return 0;
			} else if(evenement.type == SDL_KEYUP && evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
				SDL_WM_IconifyWindow();
			}
		}
		SDL_Delay(50);
	}

	nb_total = paquet->data[1];
	pos = 2;
	for(i = 0 ; i < nb_total ; i++) {
		tab_joueurs[i].type = DISTANT;
		tab_joueurs[i].sens = paquet->data[pos++];
		tab_joueurs[i].taille = paquet->data[pos++];
	}

	for(i = nb_total ; i < 6 ; i++) {
		tab_joueurs[i].type = DESACTIVE;
	}

	return (Uint8)paquet->data[pos];
}

Uint8 connecte_client(char* addr_ip) {
	char msg_chaine[45];

	Uint32 temps = 0;

	/* Utilisé sur chaque client, pour établir la connexion au serveur. */
	if(SDLNet_ResolveHost(&clt_dist_adresse, addr_ip, PORT)) {
		message_texte(_("Name resolving error."), _("Please check the hostname you provided."), NULL);
		delai_controlle(5000);
		return 0;
	}

	res_socket = SDLNet_UDP_Open(0);
	if(!res_socket) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	if(SDLNet_UDP_Bind(res_socket, 0, &clt_dist_adresse) == -1) {
	    fprintf(stderr, "SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
	    exit(EXIT_FAILURE);
	}

	prep_attente(_("Connecting to server..."), _("Press Escape to cancel."), NULL);

	strcpy((char*)paquet->data, HANDSHAKE);
	paquet->len = strlen(HANDSHAKE) ;

	for(;;) {
		if(temps < SDL_GetTicks()) {
			if(SDLNet_UDP_Send(res_socket, 0, paquet) == 0) {
				fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
			}

			temps = SDL_GetTicks() + 1000;
		}

		attente();

		if(SDLNet_UDP_Recv(res_socket, paquet) && paquet->data[0] == 'H') {
			clt_id_joueur = paquet->data[1];
			if(clt_id_joueur < NB_JOUEURS_G) {
				snprintf4(msg_chaine, 45, _("You will be the left player #%d."), clt_id_joueur + 1);
			} else {
				snprintf4(msg_chaine, 45, _("You will be the right player #%d."), clt_id_joueur - NB_JOUEURS_G + 1);
			}
		
			prep_attente(_("Connected. Waiting for server..."), msg_chaine, _("Press Escape to cancel."));

			if(SDLNet_UDP_Bind(res_socket, (clt_id_joueur < NB_JOUEURS_G ? 1 : 2), &clt_dist_adresse) == -1) {
		    		fprintf(stderr, "SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
		    		exit(EXIT_FAILURE);
			}
		
			return config_client();
		}

		if(SDL_PollEvent(&evenement)) {
			if(evenement.type == SDL_QUIT) {
				exit(EXIT_SUCCESS);
			} else if(evenement.type == SDL_KEYDOWN && evenement.key.keysym.sym == SDLK_ESCAPE) {
				deconnecte_client();
				return 0;
			} else if(evenement.type == SDL_KEYUP && evenement.key.keysym.sym == SDLK_TAB && (SDL_GetModState() & KMOD_LALT) && plein_ecran) {
				SDL_WM_IconifyWindow();
			}
		}
		SDL_Delay(50);
	}
}

void client_quitte(void) {
	paquet->data[0] = 'Q';
	paquet->len = 1;
	if(SDLNet_UDP_Send(res_socket, 0, paquet) == 0) {
		fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
	}
}

void serveur_quitte(void) {
	paquet->data[0] = 'Q';
	paquet->len = 1;
	reseau_srv_env();
}

void deconnecte_client(void) {
	SDLNet_UDP_Unbind(res_socket, 0);
	SDLNet_UDP_Close(res_socket);
}

void libere_reseau(void) {
	SDLNet_FreePacket(paquet);
}

#endif
