/* Minimal directory iterator for the unchanged theme loader. */
#ifndef SV_DIRENT_H
#define SV_DIRENT_H
#include <windows.h>
struct dirent { char d_name[MAX_PATH]; };
typedef struct { HANDLE handle; WIN32_FIND_DATAA data; struct dirent entry; int first; char pattern[MAX_PATH]; } DIR;
DIR *opendir(const char *path);
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);
void rewinddir(DIR *dir);
#endif
