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

$Id: reseau.h 203 2008-08-20 21:37:55Z vinduv $
*/

#ifdef NET_SUPPORT
#ifndef _RESEAU_H
#define _RESEAU_H 1

#include <SDL_types.h>
#include <SDL_net.h>

#define ECRIT_8(x) paquet->data[paquet->len++] = (x)
#define ECRIT_16(x) SDLNet_Write16((Uint16)(x), (Uint16*)(paquet->data + paquet->len)); paquet->len += 2

void init_reseau(void);

void deconnecte_serveur(void);

bool connecte_joueurs(void);

void libere_reseau(void);

bool reseau__clt_rec(char* chaine, Uint8 n);
void reseau_clt_env(void);
bool reseau__srv_rec(char* chaine, Uint8 n);
void reseau_srv_env(void);

void client_quitte(void);
void serveur_quitte(void);

Uint8 connecte_client(char* addr_ip);
void deconnecte_client(void);

Uint8 nb_reseau; /* Nombre de joueurs réseau */
Uint8 nb_total; /* Nombre de joueurs au total */

UDPpacket* paquet;

Uint8 clt_id_joueur;

#endif
#endif
