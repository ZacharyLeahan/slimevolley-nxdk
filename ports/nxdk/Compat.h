/* GPL-3.0-or-later: Xbox-only SDL 1.2 surface/event compatibility. */
#ifndef SV_COMPAT_H
#define SV_COMPAT_H
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/config.h"
typedef SDL_Keycode SDLKey;
typedef struct { int wm_available, current_w, current_h; } SDL_VideoInfo;
#define SDL_FULLSCREEN SDL_WINDOW_FULLSCREEN
#define SDL_HWSURFACE 0
#define SDL_DOUBLEBUF 0
#define SDL_SRCALPHA 0
#define SDLK_LAST 0x20000000
SDL_Surface *sv_video(int w,int h,int bpp,Uint32 flags);
SDL_Surface *sv_display_alpha(SDL_Surface *s);
const SDL_VideoInfo *sv_video_info(void);
SDL_Rect **sv_modes(SDL_PixelFormat *f, Uint32 flags);
int sv_flip(SDL_Surface *s);
int sv_poll(SDL_Event *e);
int sv_wait(SDL_Event *e);
int sv_init(Uint32 flags);
void sv_quit(void);
void sv_close_audio(void);
void sv_free_surface(SDL_Surface *s);
int sv_open_audio(SDL_AudioSpec *wanted,SDL_AudioSpec *obtained);
int sv_main(int argc,char **argv);
int sv_access(const char *path,int mode);
FILE *sv_fopen(const char *path,const char *mode);
SDL_Surface *sv_image(const char *path);
TTF_Font *sv_font(const char *path,int size);
SDL_AudioSpec *sv_wav(const char *path,SDL_AudioSpec *spec,Uint8 **buf,Uint32 *len);
void sv_controls_help(void);
void sv_system_menu(void);
void sv_game_controls(void);
#ifndef R_OK
#define R_OK 4
#define X_OK 0
#define F_OK 0
#endif
#ifndef SV_COMPAT_IMPLEMENTATION
#define main sv_main
#define SDL_Init sv_init
#define SDL_Quit sv_quit
#define SDL_CloseAudio sv_close_audio
#define SDL_FreeSurface sv_free_surface
#define SDL_OpenAudio sv_open_audio
#define SDL_SetVideoMode sv_video
#define SDL_DisplayFormatAlpha sv_display_alpha
#define SDL_GetVideoInfo sv_video_info
#define SDL_ListModes sv_modes
#define SDL_VideoModeOK(w,h,b,f) 32
#define SDL_Flip sv_flip
#define SDL_PollEvent sv_poll
#define SDL_WaitEvent sv_wait
#define SDL_WM_SetCaption(a,b) ((void)0)
#define SDL_WM_SetIcon(a,b) ((void)0)
#define SDL_WM_IconifyWindow() 0
#define access sv_access
#define fopen sv_fopen
#define IMG_Load sv_image
#define TTF_OpenFont sv_font
#undef SDL_LoadWAV
#define SDL_LoadWAV sv_wav
#endif
#endif
