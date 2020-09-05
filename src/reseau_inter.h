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

$Id: reseau_inter.h 202 2008-08-20 10:16:32Z vinduv $
*/

#ifdef NET_SUPPORT
#ifndef _RESEAU_INTER_H
#define _RESEAU_INTER_H 1

#include <stdbool.h>

#define MAX_CARACT 50

void prep_attente(const char* texte, const char* ligne2, const char* ligne3);
void attente(void);

void message_texte(const char* texte, const char* ligne2, const char* ligne3);

bool prompt_ip(const char* info_txt, char* entree);

void delai_controlle(Uint32 temps);

#endif
#endif
