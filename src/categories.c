/* psp5 – category inference and persistent manual overrides. */
#include <pspkernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "categories.h"

#define CATEGORY_MAX_OVERRIDES 512
#define CATEGORY_PATH_MAX 256

typedef struct {
    char path[CATEGORY_PATH_MAX];
    unsigned char category;
} CategoryOverride;

static CategoryOverride s_overrides[CATEGORY_MAX_OVERRIDES];
static int s_override_count;
static char s_storage_file[256];

static int path_has_component(const char *path, const char *component)
{
    size_t n = strlen(component);
    const char *p = path;
    while (*p) {
        if ((p == path || p[-1] == '/' || p[-1] == ':') &&
            strncasecmp(p, component, n) == 0 &&
            (p[n] == '/' || p[n] == '\\' || p[n] == 0))
            return 1;
        p++;
    }
    return 0;
}

GameCategory categories_guess(const Entry *entry)
{
    if (!entry) return GAME_CATEGORY_OTHER;
    if (entry->type == ENTRY_POPS) return GAME_CATEGORY_PS1;
    if (path_has_component(entry->path, "Emulators") ||
        path_has_component(entry->path, "Emulator"))
        return GAME_CATEGORY_EMULATOR;
    if (entry->type == ENTRY_ISO || entry->type == ENTRY_UMD ||
        entry->type == ENTRY_PSN || entry->type == ENTRY_HOMEBREW)
        return GAME_CATEGORY_PSP;
    return GAME_CATEGORY_OTHER;
}

void categories_load(const char *file)
{
    s_override_count = 0;
    if (!file || !*file) return;
    strncpy(s_storage_file, file, sizeof(s_storage_file) - 1);
    s_storage_file[sizeof(s_storage_file) - 1] = 0;
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
    if (fd < 0) return;
    char line[CATEGORY_PATH_MAX + 16];
    while (s_override_count < CATEGORY_MAX_OVERRIDES) {
        int n = 0, got = 0;
        char c;
        while (sceIoRead(fd, &c, 1) == 1) {
            got = 1;
            if (c == '\n') break;
            if (n < (int)sizeof(line) - 1) line[n++] = c;
        }
        if (!got) break;
        line[n] = 0;
        char *tab = strrchr(line, '\t');
        if (!tab) continue;
        *tab++ = 0;
        int category = atoi(tab);
        if (!line[0] || category < 0 || category >= GAME_CATEGORY_COUNT) continue;
        CategoryOverride *o = &s_overrides[s_override_count++];
        strncpy(o->path, line, sizeof(o->path) - 1);
        o->path[sizeof(o->path) - 1] = 0;
        o->category = (unsigned char)category;
    }
    sceIoClose(fd);
}

void categories_save(const char *file)
{
    if (!file || !*file) file = s_storage_file;
    if (!file || !*file) return;
    SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (fd < 0) return;
    for (int i = 0; i < s_override_count; i++) {
        char line[CATEGORY_PATH_MAX + 16];
        int n = snprintf(line, sizeof(line), "%s\t%u\n",
                         s_overrides[i].path, (unsigned)s_overrides[i].category);
        if (n > 0 && n < (int)sizeof(line)) sceIoWrite(fd, line, n);
    }
    sceIoClose(fd);
}

GameCategory categories_get(const Entry *entry)
{
    if (!entry) return GAME_CATEGORY_OTHER;
    for (int i = 0; i < s_override_count; i++)
        if (!strcmp(s_overrides[i].path, entry->path))
            return (GameCategory)s_overrides[i].category;
    return categories_guess(entry);
}

int categories_set(const Entry *entry, GameCategory category)
{
    if (!entry || category < 0 || category >= GAME_CATEGORY_COUNT || !entry->path[0])
        return 0;
    for (int i = 0; i < s_override_count; i++) {
        if (!strcmp(s_overrides[i].path, entry->path)) {
            s_overrides[i].category = (unsigned char)category;
            return 1;
        }
    }
    if (s_override_count >= CATEGORY_MAX_OVERRIDES) return 0;
    CategoryOverride *o = &s_overrides[s_override_count++];
    strncpy(o->path, entry->path, sizeof(o->path) - 1);
    o->path[sizeof(o->path) - 1] = 0;
    o->category = (unsigned char)category;
    return 1;
}

void categories_forget_path(const char *path)
{
    if (!path) return;
    for (int i = 0; i < s_override_count; ) {
        if (!strcmp(s_overrides[i].path, path)) {
            memmove(&s_overrides[i], &s_overrides[i + 1],
                    (s_override_count - i - 1) * sizeof(s_overrides[0]));
            s_override_count--;
        } else i++;
    }
}
