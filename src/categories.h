/* psp5 – persistent library category helpers. */
#ifndef CATEGORIES_H
#define CATEGORIES_H
#include "scan.h"
typedef enum { GAME_CATEGORY_PSP = 0, GAME_CATEGORY_PS1, GAME_CATEGORY_EMULATOR, GAME_CATEGORY_OTHER, GAME_CATEGORY_COUNT } GameCategory;
GameCategory categories_guess(const Entry *entry);
void categories_load(const char *file);
void categories_save(const char *file);
GameCategory categories_get(const Entry *entry);
int categories_set(const Entry *entry, GameCategory category);
void categories_forget_path(const char *path);
#endif
