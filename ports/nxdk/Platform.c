/* SPDX-License-Identifier: GPL-3.0-or-later */
#define SV_COMPAT_IMPLEMENTATION
#include "Compat.h"
#include "Controller.h"
#include "slime.h"
#include "themes.h"
#include <windows.h>
#include <direct.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <hal/fileio.h>
#include <nxdk/path.h>
#include <nxdk/mount.h>
#include <xboxkrnl/xboxkrnl.h>
#include <dirent.h>

static SDL_Window *window;
static SDL_Joystick *pads[4];
static unsigned held[4], previous[4];
static SDL_Event pending[64];
static unsigned head,tail;
static Uint32 last_stats;
static volatile LONG startup_complete, shutting_down;
static volatile LONG stage;
static DWORD shutdown_started;
static unsigned frames;
enum { SYSTEM_LAUNCHER=1, SYSTEM_GAME, SYSTEM_REBOOT, SYSTEM_OFF };
static int exit_action = SYSTEM_LAUNCHER;
static char game_xbe[MAX_PATH];
static const char *launcher = "E:\\Dashboard\\default.xbe";

static const SDL_VideoInfo info = { 0,800,600 };
static SDL_Rect mode = { 0,0,800,600 };
static SDL_Rect *modes[] = { &mode,NULL };

static void native_path(char *out,const char *in,size_t size) {
    size_t i; snprintf(out,size,"%s%s",strchr(in,':')?"":"D:\\",in);
    for(i=0;out[i];++i) if(out[i]=='/') out[i]='\\';
}
FILE *sv_fopen(const char *path,const char *mode) {
    char p[MAX_PATH];native_path(p,path,sizeof(p));return fopen(p,mode);
}
SDL_Surface *sv_image(const char *path) {
    char p[MAX_PATH];native_path(p,path,sizeof(p));printf("image %s\n",p);return IMG_Load(p);
}
TTF_Font *sv_font(const char *path,int size) {
    char p[MAX_PATH];native_path(p,path,sizeof(p));printf("font %s size=%d\n",p,size);return TTF_OpenFont(p,size);
}
SDL_AudioSpec *sv_wav(const char *path,SDL_AudioSpec *spec,Uint8 **buf,Uint32 *len) {
    char p[MAX_PATH];native_path(p,path,sizeof(p));return SDL_LoadWAV(p,spec,buf,len);
}
int sv_access(const char *path,int mode_unused) {
    char pathbuf[MAX_PATH]; (void)mode_unused;
    native_path(pathbuf,path,sizeof(pathbuf));
    return GetFileAttributesA(pathbuf)==INVALID_FILE_ATTRIBUTES ? -1:0;
}
DIR *opendir(const char *path) {
    DIR *d=calloc(1,sizeof(*d)); if(!d) return NULL;
    native_path(d->pattern,path,sizeof(d->pattern));
    strncat(d->pattern,"\\*",sizeof(d->pattern)-strlen(d->pattern)-1);
    d->handle=FindFirstFileA(d->pattern,&d->data); d->first=1;
    if(d->handle==INVALID_HANDLE_VALUE) { free(d);return NULL; } return d;
}
struct dirent *readdir(DIR *d) {
    if(!d) return NULL;
    if(!d->first && !FindNextFileA(d->handle,&d->data)) return NULL;
    d->first=0; snprintf(d->entry.d_name,sizeof(d->entry.d_name),"%s",d->data.cFileName);
    return &d->entry;
}
int closedir(DIR *d) { if(d) {FindClose(d->handle);free(d);} return 0; }
void rewinddir(DIR *d) {
    FindClose(d->handle);d->handle=FindFirstFileA(d->pattern,&d->data); d->first=1;
}
const SDL_VideoInfo *sv_video_info(void) { return &info; }
SDL_Rect **sv_modes(SDL_PixelFormat *f,Uint32 flags) { (void)f;(void)flags;return modes; }
SDL_Surface *sv_video(int w,int h,int bpp,Uint32 flags) {
    (void)bpp;(void)flags;
    if(!window) window=SDL_CreateWindow("Slime Volley",0,0,640,480,SDL_WINDOW_SHOWN);
    if(!window) {fprintf(stderr,"window: %s\n",SDL_GetError());exit(1);}
    SDL_Surface *s=SDL_CreateRGBSurfaceWithFormat(0,w,h,32,SDL_PIXELFORMAT_ARGB8888);
    if(!s) {fprintf(stderr,"framebuffer: %s\n",SDL_GetError());exit(1);} return s;
}
SDL_Surface *sv_display_alpha(SDL_Surface *s) {
    return SDL_ConvertSurfaceFormat(s,SDL_PIXELFORMAT_ARGB8888,0);
}
int sv_flip(SDL_Surface *s) {
    stage=6;
    SDL_Surface *out=SDL_GetWindowSurface(window);
    if(!out) return -1;
    SDL_SetSurfaceBlendMode(s,SDL_BLENDMODE_NONE);
    if(SDL_BlitScaled(s,NULL,out,NULL)<0) return -1;
    int result=SDL_UpdateWindowSurface(window);
    if(result==0 && !startup_complete) {startup_complete=1;printf("first frame presented\n");}
    ++frames; Uint32 now=SDL_GetTicks();
    if(now-last_stats>=5000) {
        MM_STATISTICS memory = {0}; memory.Length=sizeof(memory); MmQueryStatistics(&memory);
        unsigned rate=frames*10000/(now-last_stats);
        printf("stats ticks=%lu fps=%u.%u available_kib=%lu total_kib=%lu\n",(unsigned long)now,
            rate/10,rate%10,(unsigned long)(memory.AvailablePages*4),(unsigned long)(memory.TotalPhysicalPages*4));
        for(int i=0;i<NB_JOUEURS_T;i++) if(tab_joueurs[i].type!=DESACTIVE)
            printf("player=%d type=%d port=%u x=%d y=%d\n",i+1,tab_joueurs[i].type,
                tab_joueurs[i].conf_clavier+1,(int)tab_joueurs[i].x,(int)tab_joueurs[i].y);
        fflush(stdout);last_stats=now;frames=0;
    }
    return result;
}
static void key(SDL_Keycode code,int down) {
    if(tail-head>=64) return;
    SDL_Event *e=&pending[tail++%64]; memset(e,0,sizeof(*e));
    e->type=down?SDL_KEYDOWN:SDL_KEYUP; e->key.state=down?SDL_PRESSED:SDL_RELEASED;
    e->key.keysym.sym=code;
}
static void controllers(void) {
    static Uint32 reset_since[4];
    SDL_JoystickUpdate();
    for(int i=0;i<4;i++) {
        if(pads[i]&&!SDL_JoystickGetAttached(pads[i])) {
            printf("pad disconnected slot=%d\n",i+1);SDL_JoystickClose(pads[i]);pads[i]=NULL;
        }
    }
    for(int n=0;n<SDL_NumJoysticks();n++) {
        SDL_JoystickID id=SDL_JoystickGetDeviceInstanceID(n);int found=0;
        for(int i=0;i<4;i++) if(pads[i]&&SDL_JoystickInstanceID(pads[i])==id) found=1;
        if(found) continue;
        SDL_Joystick *pad=SDL_JoystickOpen(n);if(!pad) continue;
        int slot=SDL_JoystickGetPlayerIndex(pad)-1;
        if(slot<0||slot>=4||pads[slot]) {SDL_JoystickClose(pad);continue;}
        pads[slot]=pad;printf("pad connected slot=%d instance=%ld\n",slot+1,(long)id);fflush(stdout);
    }
    static const SDL_Keycode codes[8]={SDLK_UP,SDLK_DOWN,SDLK_LEFT,SDLK_RIGHT,SDLK_RETURN,SDLK_ESCAPE,SDLK_SPACE,SDLK_x};
    for(int i=0;i<4;i++) {
        unsigned b=0; SDL_Joystick *p=pads[i];
        SvPadState state={0};
        if(p) {
            state.hat=SDL_JoystickGetHat(p,0);
            for(int axis=0;axis<6;axis++) state.axes[axis]=SDL_JoystickGetAxis(p,axis);
            for(int button=0;button<10;button++)
                if(SDL_JoystickGetButton(p,button)) state.buttons|=1u<<button;
            b=sv_pad_mask(&state);
        }
        held[i]=b;
        if(sv_pad_recovery(&state)) {
            if(!reset_since[i]) reset_since[i]=SDL_GetTicks();
            else if(SDL_GetTicks()-reset_since[i]>=2000) {printf("Back+Start recovery\n");fflush(NULL);XReboot();}
        } else reset_since[i]=0;
        for(int k=0;k<8;k++) if((b^previous[i])&(1u<<k)) key(codes[k],!!(b&(1u<<k)));
        if(b!=previous[i]) printf("input slot=%d mask=%02x\n",i+1,b);
        previous[i]=b;
    }
}
void sv_game_controls(void) {
    for(int i=0;i<NB_JOUEURS_T;i++) if(tab_joueurs[i].type==CLAVIER) {
        unsigned b=held[tab_joueurs[i].conf_clavier%4];
        tab_joueurs[i].t_haut=!!(b&17);tab_joueurs[i].t_bas=!!(b&130);
        tab_joueurs[i].t_gauche=!!(b&4);tab_joueurs[i].t_droite=!!(b&8);
    }
}
int sv_poll(SDL_Event *e) {
    SDL_Event raw;
    /* Pump and drain native joystick events; synthesize only legacy menu keys. */
    while(SDL_PollEvent(&raw)) if(raw.type==SDL_QUIT||raw.type==SDL_KEYDOWN||raw.type==SDL_KEYUP) {
        if(tail-head<64) pending[tail++%64]=raw;
    }
    controllers();sv_game_controls();
    if(head==tail) return 0;
    if(e) *e=pending[head%64]; ++head;return 1;
}
int sv_wait(SDL_Event *e) { while(!sv_poll(e)) SDL_Delay(5);return 1; }
int sv_init(Uint32 flags) {
    stage=2;printf("init SDL\n");fflush(stdout);
    int r=SDL_Init(flags|SDL_INIT_JOYSTICK|SDL_INIT_TIMER);
    /* Unique action codes keep synthesized navigation keys from controlling another pad. */
    for(int i=0;i<4;i++) { touches[i].haut=0x10000+i*4;touches[i].bas=0x10001+i*4;
        touches[i].gauche=0x10002+i*4;touches[i].droite=0x10003+i*4; }
    printf("SDL init result=%d error=%s\n",r,SDL_GetError());fflush(stdout);return r;
}
void sv_free_surface(SDL_Surface *s) {
    if(s&&s==ecran) {shutdown_started=GetTickCount();shutting_down=1;}
    SDL_FreeSurface(s);
}
int sv_open_audio(SDL_AudioSpec *wanted,SDL_AudioSpec *obtained) {
    stage=3;printf("opening audio\n");int r=SDL_OpenAudio(wanted,obtained);
    printf("audio result=%d frequency=%d format=%u channels=%u\n",r,
        r==0?obtained->freq:0,r==0?obtained->format:0,r==0?obtained->channels:0);
    stage=4;return r;
}
void sv_close_audio(void) {
    shutdown_started=GetTickCount();shutting_down=1;
    printf("closing audio\n");SDL_CloseAudio();printf("audio closed\n");
}
void sv_quit(void) {
    for(int i=0;i<4;i++) if(pads[i]) {SDL_JoystickClose(pads[i]);pads[i]=NULL;}
    SDL_DestroyWindow(window);window=NULL;SDL_Quit();printf("SDL shutdown complete\n");fflush(stdout);
}
void sv_controls_help(void) {
    SDL_BlitSurface(fond,NULL,ecran,NULL);
    const char *lines[]={"Xbox controls","Move: D-pad / either stick / triggers","Jump: A / Up on D-pad or either stick","Skin: X / Down on D-pad or either stick","Menus: A selects, B / Back returns","Pause / resume: Start","Players use controller ports 1-4","Main menu: Back opens Exit / System","Press B to return"};
    for(unsigned i=0;i<sizeof(lines)/sizeof(lines[0]);i++)
        afficher((char*)lines[i],police,coul_txt_menu,70,110+i*40);
    sv_flip(ecran);SDL_Event e;
    while(sv_wait(&e)) if(e.type==SDL_QUIT||(e.type==SDL_KEYDOWN&&e.key.keysym.sym==SDLK_ESCAPE)) break;
}
/* Default to Cancel in both the system menu and its confirmations. */
static int system_choose(const char *title,const char **items,const char **help,int count) {
    int selected=0,redraw=1; SDL_Event e;
    for(;;) {
        if(redraw) {
            SDL_BlitSurface(fond,NULL,ecran,NULL);
            afficher((char*)title,police_menu,coul_txt_menu,70,115);
            for(int i=0;i<count;i++) {
                char line[96];snprintf(line,sizeof(line),"%s%s",i==selected?"> ":"  ",items[i]);
                afficher(line,police,coul_txt_menu,80,195+i*42);
            }
            aff_aide((char*)help[selected]);sv_flip(ecran);redraw=0;
        }
        sv_wait(&e);
        if(e.type==SDL_QUIT) return 0;
        if(e.type!=SDL_KEYDOWN) continue;
        switch(e.key.keysym.sym) {
            case SDLK_ESCAPE:return 0;
            case SDLK_UP:selected=(selected+count-1)%count;redraw=1;break;
            case SDLK_DOWN:selected=(selected+1)%count;redraw=1;break;
            case SDLK_RETURN:return selected;
            default:break;
        }
    }
}
void sv_system_menu(void) {
    const char *items[]={"Cancel","Return to launcher","Restart game","Reboot Xbox","Power off Xbox"};
    const char *help[]={"Return to the game menu.","Close Slime Volley and open your dashboard.",
        "Reload Slime Volley from the beginning.","Restart the Xbox through its normal boot process.","Save settings and turn the Xbox off."};
    int selected=system_choose("Exit / System",items,help,5);
    if(!selected) return;
    if(selected==SYSTEM_LAUNCHER && GetFileAttributesA(launcher)==INVALID_FILE_ATTRIBUTES) {
        printf("launcher unavailable path=%s error=%lu\n",launcher,(unsigned long)GetLastError());
        const char *notice[]={"Return"},*explanation[]={"Launcher not found. No system action was taken."};
        system_choose("Launcher unavailable",notice,explanation,1);return;
    }
    const char *confirm[]={"Cancel",items[selected]};
    const char *details[]={"Keep Slime Volley running.",help[selected]};
    if(!system_choose("Confirm system action",confirm,details,2)) return;
    exit_action=selected;printf("system action=%d\n",selected);exit(EXIT_SUCCESS);
}
static DWORD WINAPI watchdog(LPVOID unused) {
    (void)unused;DWORD began=GetTickCount();
    for(;;) {
        Sleep(100);
        int startup_timeout=!startup_complete&&GetTickCount()-began>30000;
        int exit_timeout=shutting_down&&GetTickCount()-shutdown_started>15000;
        if(startup_timeout||exit_timeout) {
            HANDLE f=CreateFileA("D:\\watchdog.log",GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            if(f!=INVALID_HANDLE_VALUE) {
                char msg[128];DWORD n;int len=snprintf(msg,sizeof(msg),"timeout startup=%d shutdown=%d stage=%ld\n",startup_timeout,exit_timeout,(long)stage);
                WriteFile(f,msg,len,&n,NULL);CloseHandle(f);
            }
            XReboot();return 0;
        }
    }
}
static void dashboard(void) {
    fflush(NULL);shutting_down=0;
    FILE *history=fopen("D:\\system-actions.log","ab");
    if(history) {
        fprintf(history,"action=%d cleanup=complete target=%s\n",exit_action,
            exit_action==SYSTEM_GAME?game_xbe:exit_action==SYSTEM_LAUNCHER?launcher:"system");
        fclose(history);
    }
    if(exit_action==SYSTEM_LAUNCHER) XLaunchXBE(launcher);
    else if(exit_action==SYSTEM_GAME) XLaunchXBE(game_xbe);
    else if(exit_action==SYSTEM_OFF) {
        HalWriteSMBusValue(0x20,0x02,FALSE,0x80);
        for(;;) Sleep(1000);
    }
    XReboot();
}
int main(void) {
    nxGetCurrentXbeNtPath(game_xbe);
    if(!nxIsDriveMounted('E')) nxMountDrive('E',"\\Device\\Harddisk0\\Partition1");
    _chdir("D:\\");
    freopen("D:\\slime.log","w",stdout);freopen("D:\\slime-error.log","w",stderr);
    setvbuf(stdout,NULL,_IONBF,0);setvbuf(stderr,NULL,_IONBF,0);
    printf("Slime Volley nxdk: four slots, 800x600 logical / 640x480 output\n");
    stage=1;HANDLE guard=CreateThread(NULL,0,watchdog,NULL,0,NULL);
    if(!guard) {fprintf(stderr,"Cannot start watchdog\n");return 1;}CloseHandle(guard);
    XVideoSetMode(640,480,32,REFRESH_DEFAULT);
    atexit(dashboard);
    char *argv[]={"D:\\default.xbe",NULL};return sv_main(1,argv);
}
