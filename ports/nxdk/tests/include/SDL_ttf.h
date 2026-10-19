/* Only declarations needed by core state/audio tests; no font backend is used. */
#include <SDL.h>
typedef struct _TTF_Font TTF_Font;
SDL_Surface *TTF_RenderUTF8_Blended(TTF_Font *,const char *,SDL_Color);
