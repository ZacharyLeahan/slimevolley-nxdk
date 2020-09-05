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

$Id: echelle.c 257 2008-11-23 10:03:21Z vinduv $
*/

#include <SDL.h>
#include <math.h>

/*#define UNSAFE_STRETCH 1 Ne fonctionne pas correctement */

#include "config.h"

#ifdef MACOS9
	#define rint(x) x
#endif

void put_pixel(SDL_Surface * surface, Uint16 x, Uint16 y, Uint32 color) {
	Uint8 bpp = surface->format->BytesPerPixel;
	Uint8 * p = ((Uint8 *)surface->pixels) + y * surface->pitch + x * bpp;
	switch(bpp) {
		case 1:
			*p = (Uint8) color;
		break;
		case 2:
			*(Uint16 *)p = (Uint16) color;
		break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				*(Uint16 *)p = ((color >> 8) & 0xff00) | ((color >> 8) & 0xff);
				*(p + 2) = color & 0xff;
			} else {
				*(Uint16 *)p = color & 0xffff;
				*(p + 2) = ((color >> 16) & 0xff) ;
			}
		break;
		case 4:
			*(Uint32 *)p = color;
		break;
    }
}

Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;


	switch(bpp) {
		case 1:
			return *p;
		break;

		 case 2:
			 return *(Uint16 *)p;
		break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				return p[0] << 16 | p[1] << 8 | p[2];
			} else {
				return p[0] | p[1] << 8 | p[2] << 16;
			}
		break;

		case 4:
			return *(Uint32 *)p;
		break;

		default:
			return 0; /* shouldn't happen, but avoids warnings */
		break;
	}
 }

#ifdef UNSAFE_STRETCH
	extern DECLSPEC int SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect,
		SDL_Surface *dst, SDL_Rect *dstrect);
#endif

SDL_Surface* echelle_surface(SDL_Surface* orig, float ratio) {
	SDL_Surface *resultat, *temp;

#ifdef UNSAFE_STRETCH
	SDL_Rect src_rect = { 0, 0, 0, 0 };
	SDL_Rect dst_rect = { 0, 0, 0, 0 };
#else
	Uint8 i, j, r, v, b, a;
#endif

	temp = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, rint(orig->w * ratio),  rint(orig->h * ratio), 32, orig->format->Rmask, orig->format->Gmask, orig->format->Bmask, orig->format->Amask);

	resultat = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

#ifdef UNSAFE_STRETCH
	src_rect.w = orig->w;
	src_rect.h = orig->h;
	dst_rect.w = resultat->w;
	dst_rect.h = resultat->h;
	SDL_SoftStretch(orig, &src_rect, resultat, &dst_rect);
#else

	for(i = 0 ; i < resultat->w ; i++) {
		for(j = 0 ; j < resultat->h ; j++) {
			SDL_GetRGBA(get_pixel(orig, rint((float)i / ratio), rint((float)j / ratio)), orig->format, &r, &v, &b, &a);
			put_pixel(resultat, i, j, SDL_MapRGBA(resultat->format, r, v, b, a));
		}
	}
#endif

	return resultat;
}
