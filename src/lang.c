/* psp5 – made by Mille Weber */
/* UI strings are loaded from ms0:/PSP5/languages/<code>.json.
 * English is loaded first and is the fallback for missing translations. */
#include <psputility.h>
#include <pspiofilemgr.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lang.h"

#define TEXT_CAP 256
#define JSON_CAP 65536
#define MAX_LANGUAGES 32
#define CODE_CAP 16
#define NAME_CAP 48
unsigned char g_lang_setting = LANG_AUTO;
static int s_lang = 1;
static int s_lang_count = 1;
static char s_codes[MAX_LANGUAGES][CODE_CAP];
static char s_names[MAX_LANGUAGES][NAME_CAP];
static char s_text[S_COUNT][TEXT_CAP];

static int key_to_id(const char *key);
static void load_json(const char *code);

int lang_count(void) { return s_lang_count; }

static void add_language(const char *code)
{
    if (!code || !*code || s_lang_count >= MAX_LANGUAGES) return;
    if (!strcmp(code, "auto")) return;
    for (int i = 1; i < s_lang_count; i++)
        if (!strcmp(s_codes[i], code)) return;
    int id = s_lang_count++;
    strncpy(s_codes[id], code, CODE_CAP - 1);
    s_codes[id][CODE_CAP - 1] = '\0';
    strncpy(s_names[id], code, NAME_CAP - 1);
    s_names[id][NAME_CAP - 1] = '\0';
}

static void scan_languages(void)
{
    SceUID dir = sceIoDopen("ms0:/PSP5/languages/");
    s_lang_count = 1;
    memset(s_codes, 0, sizeof(s_codes));
    memset(s_names, 0, sizeof(s_names));
    if (dir >= 0) {
        SceIoDirent ent;
        memset(&ent, 0, sizeof(ent));
        while (sceIoDread(dir, &ent) > 0) {
            const char *name = ent.d_name;
            size_t n = strlen(name);
            if (n > 5 && !strcmp(name + n - 5, ".json") && n - 5 < CODE_CAP) {
                char code[CODE_CAP];
                memcpy(code, name, n - 5);
                code[n - 5] = '\0';
                if (FIO_S_ISDIR(ent.d_stat.st_mode)) continue;
                add_language(code);
            }
            memset(&ent, 0, sizeof(ent));
        }
        sceIoDclose(dir);
    }
    /* The default must exist even if the directory scan fails. */
    int has_en = 0;
    for (int i = 1; i < s_lang_count; i++)
        if (!strcmp(s_codes[i], "en")) has_en = 1;
    if (!has_en) add_language("en");
}

static const char *skip_ws(const char *p)
{
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    return p;
}

/* Decode a JSON string into UTF-8. Literal UTF-8 and standard JSON escapes
 * are supported, including \\uXXXX for custom translations. */
static const char *json_string(const char *p, char *out, size_t cap)
{
    size_t n = 0;
    if (*p != '"') return NULL;
    p++;
    while (*p && *p != '"') {
        unsigned char c = (unsigned char)*p++;
        if (c == '\\') {
            c = (unsigned char)*p++;
            switch (c) {
            case '"': case '\\': case '/': break;
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case 't': c = '\t'; break;
            case 'u': {
                unsigned int cp = 0;
                for (int i = 0; i < 4 && *p; i++) {
                    unsigned char h = (unsigned char)*p++;
                    cp <<= 4;
                    if (h >= '0' && h <= '9') cp |= h - '0';
                    else if (h >= 'a' && h <= 'f') cp |= h - 'a' + 10;
                    else if (h >= 'A' && h <= 'F') cp |= h - 'A' + 10;
                }
                if (cp < 0x80) c = (unsigned char)cp;
                else {
                    if (cap && n + 3 < cap) {
                        if (cp < 0x800) {
                            out[n++] = (char)(0xC0 | (cp >> 6));
                            out[n++] = (char)(0x80 | (cp & 0x3F));
                        } else {
                            out[n++] = (char)(0xE0 | (cp >> 12));
                            out[n++] = (char)(0x80 | ((cp >> 6) & 0x3F));
                            out[n++] = (char)(0x80 | (cp & 0x3F));
                        }
                    }
                    continue;
                }
                break;
            }
            default: break;
            }
        }
        if (n + 1 < cap) out[n++] = (char)c;
    }
    if (*p != '"') return NULL;
    if (cap) out[n] = '\0';
    return p + 1;
}

static int key_to_id(const char *key)
{
#define KEY(name) if (!strcmp(key, #name)) return name
    KEY(S_TAB_GAMES);     KEY(S_LAUNCH);     KEY(S_SETTINGS);     KEY(S_NO_GAMES);     KEY(S_HINT_GAMES);
     KEY(S_HINT_APPS);     KEY(S_NO_ARK);     KEY(S_LOADING);     KEY(S_SAVED);     KEY(S_LAUNCH_FAILED);
     KEY(S_USB_MODE);     KEY(S_USB_CONNECTED);     KEY(S_USB_WAITING);     KEY(S_USB_EJECT);     KEY(S_USB_EXIT);
     KEY(S_BACK);     KEY(S_YES);     KEY(S_NO);     KEY(S_OFF);     KEY(S_AUTO);
     KEY(S_STANDARD);     KEY(S_ALL);     KEY(S_CAT_PERF);     KEY(S_CAT_SYSTEM);     KEY(S_CAT_PLUGINS);
     KEY(S_CAT_POWER);     KEY(S_DESC_PSP5);     KEY(S_DESC_PERF);     KEY(S_DESC_PLUGINS);     KEY(S_BOOT_PSP5);
     KEY(S_SOUNDS);     KEY(S_LANGUAGE);     KEY(S_IS_LAUNCHER);     KEY(S_CLEAR_CACHE);     KEY(S_VERSION);
     KEY(S_CPU_GAME);     KEY(S_CPU_XMB);     KEY(S_EXTRA_MEM);     KEY(S_MS_CACHE);     KEY(S_ISO_CACHE);
     KEY(S_USB_CHARGE);     KEY(S_WPA2);     KEY(S_SKIP_LOGOS);     KEY(S_HIDE_PICS);     KEY(S_HIDE_MAC);
     KEY(S_HIDE_DLC);     KEY(S_NO_LED);     KEY(S_NO_UMD);     KEY(S_NO_ANALOG);     KEY(S_VITA_MUTE);
     KEY(S_QA_FLAGS);     KEY(S_UMD_REGION);     KEY(S_XMB_REGION);     KEY(S_NO_PLUGINS);     KEY(S_TO_XMB);
     KEY(S_SLEEP);     KEY(S_RESTART);     KEY(S_SHUTDOWN);     KEY(S_RECOVERY);     KEY(S_LOGOS_GAME);
     KEY(S_LOGOS_BOOT);     KEY(S_R_JAPAN);     KEY(S_R_AMERICA);     KEY(S_R_EUROPE);     KEY(S_R_KOREA);
     KEY(S_R_UK);     KEY(S_R_LATAM);     KEY(S_R_AUSTRALIA);     KEY(S_R_HONGKONG);     KEY(S_R_TAIWAN);
     KEY(S_R_RUSSIA);     KEY(S_R_CHINA);     KEY(S_RL_PSP);     KEY(S_RL_APPS);     KEY(S_LANG_AUTO);
     KEY(S_TAB_LIBRARY);     KEY(S_LIB_ALL);     KEY(S_SORT);     KEY(S_SORT_NAME);     KEY(S_SORT_RECENT);
     KEY(S_BOOT_ANIM);     KEY(S_BATTERY_PCT);     KEY(S_HEALTH);     KEY(S_TAB_MUSIC);     KEY(S_TAB_EMULATORS);     KEY(S_NO_MUSIC);
     KEY(S_HINT_MUSIC);     KEY(S_NOW_PLAYING);     KEY(S_NOTHING_PLAYING);     KEY(S_UNKNOWN_ARTIST);     KEY(S_LOADING_MUSIC);
     KEY(S_SAFE_START);     KEY(S_NO_PAUSE);     KEY(S_HIB_BLOCK);     KEY(S_CAT_THEME);     KEY(S_DESC_THEME);
     KEY(S_THEME_ART);     KEY(S_HOME_ADD);     KEY(S_HOME_REMOVE);     KEY(S_HOME_PIN);     KEY(S_HOME_UNPIN);
     KEY(S_HOME_ADDED);     KEY(S_HOME_REMOVED);     KEY(S_HOME_PINNED);     KEY(S_HOME_UNPINNED);     KEY(S_HOME_PIN_LIMIT);
     KEY(S_HOME_REMOVE_ASK);     KEY(S_MOVE_TO);     KEY(S_MOVED_TO_CATEGORY);     KEY(S_MOVE_EMULATORS);     KEY(S_OPTIONS);     KEY(S_FILES);     KEY(S_F_OPEN);     KEY(S_F_MARK);
     KEY(S_F_COPY);     KEY(S_F_MOVE);     KEY(S_F_PASTE);     KEY(S_F_RENAME);     KEY(S_F_DELETE);
     KEY(S_F_NEWDIR);     KEY(S_F_CANCEL);     KEY(S_F_EMPTY);     KEY(S_F_ITEMS);     KEY(S_F_COPYING);
     KEY(S_F_MOVING);     KEY(S_F_DELETING);     KEY(S_F_DONE);     KEY(S_F_FAILED);     KEY(S_F_NO_SPACE);
     KEY(S_F_CANCELLED);     KEY(S_F_FREE);     KEY(S_F_MS);     KEY(S_F_INTERNAL);     KEY(S_F_BAD_NAME);
     KEY(S_INFO);     KEY(S_I_TYPE);     KEY(S_I_LOCATION);     KEY(S_I_FOLDER);     KEY(S_I_FILE);
     KEY(S_I_SIZE);     KEY(S_I_CONTENTS);     KEY(S_I_SIZE_FILES);     KEY(S_I_MODIFIED);     KEY(S_I_UNCOMPRESSED);
     KEY(S_I_COMPRESSION);     KEY(S_I_VOLUME);     KEY(S_I_IMAGE);     KEY(S_I_HOMEBREW);     KEY(S_I_PSN);
     KEY(S_I_POPS);     KEY(S_I_UMD);     KEY(S_I_UMD_DRIVE);     KEY(S_DEL_FILE);     KEY(S_DEL_DIR);
     KEY(S_DEL_FOREVER);     KEY(S_DELETED);     KEY(S_SAVES);     KEY(S_SV_SELECT_ALL);     KEY(S_SV_DESELECT_ALL);
     KEY(S_SV_NONE);     KEY(S_SV_NOT_INST);     KEY(S_SV_FREE_AFTER);     KEY(S_SV_SELECTED);     KEY(S_SV_PROTECTED);
     KEY(S_SV_DEL_ASK);     KEY(S_SV_UPDATED);     KEY(S_POWER_RESTARTING);     KEY(S_POWER_SHUTTING_DOWN);
     KEY(S_POWER_SLEEPING);     KEY(S_POWER_WAKE);
#undef KEY
    return -1;
}

static void load_json(const char *code)
{
    char path[96], *data;
    SceUID fd;
    snprintf(path, sizeof(path), "ms0:/PSP5/languages/%s.json", code);
    fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (fd < 0) return;
    data = (char *)malloc(JSON_CAP);
    if (!data) { sceIoClose(fd); return; }
    int size = sceIoRead(fd, data, JSON_CAP - 1);
    sceIoClose(fd);
    if (size <= 0) { free(data); return; }
    data[size] = '\0';
    const char *p = skip_ws(data);
    if (*p++ != '{') { free(data); return; }
    while (*(p = skip_ws(p)) && *p != '}') {
        char key[64], value[TEXT_CAP];
        const char *next = json_string(p, key, sizeof(key));
        if (!next) break;
        p = skip_ws(next);
        if (*p++ != ':') break;
        p = skip_ws(p);
        next = json_string(p, value, sizeof(value));
        if (!next) break;
        if (!strncmp(key, "S_LANGNAME_", 11)) {
            char upper[CODE_CAP];
            size_t n = strlen(code);
            if (n < sizeof(upper)) {
                for (size_t i = 0; i <= n; i++) {
                    char ch = code[i];
                    upper[i] = (ch >= 'a' && ch <= 'z') ? (char)(ch - 'a' + 'A') : ch;
                }
                if (!strcmp(key + 11, upper)) {
                    int lang_id = lang_from_code(code);
                    if (lang_id > 0 && lang_id < s_lang_count) {
                        strncpy(s_names[lang_id], value, NAME_CAP - 1);
                        s_names[lang_id][NAME_CAP - 1] = '\0';
                    }
                }
            }
        }
        int id = key_to_id(key);
        if (id >= 0 && id < S_COUNT && value[0]) {
            strncpy(s_text[id], value, TEXT_CAP - 1);
            s_text[id][TEXT_CAP - 1] = '\0';
        }
        p = skip_ws(next);
        if (*p == ',') { p++; continue; }
        if (*p != '}') break;
    }
    free(data);
}

static void load_language(void)
{
    memset(s_text, 0, sizeof(s_text));
    load_json("en");
    if (s_lang > 0 && s_lang < s_lang_count && strcmp(s_codes[s_lang], "en"))
        load_json(s_codes[s_lang]);
}

static const char *system_code(void)
{
    int sys = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
    if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &sys) < 0)
        return "en";
    switch (sys) {
    case PSP_SYSTEMPARAM_LANGUAGE_JAPANESE:   return "ja";
    case PSP_SYSTEMPARAM_LANGUAGE_FRENCH:      return "fr";
    case PSP_SYSTEMPARAM_LANGUAGE_SPANISH:     return "es";
    case PSP_SYSTEMPARAM_LANGUAGE_GERMAN:      return "de";
    case PSP_SYSTEMPARAM_LANGUAGE_ITALIAN:     return "it";
    case PSP_SYSTEMPARAM_LANGUAGE_DUTCH:       return "nl";
    case PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE:  return "pt";
    case PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN:     return "ru";
    default:                                   return "en";
    }
}

void lang_apply(void)
{
    char requested_code[CODE_CAP] = "";
    if (g_lang_setting > 0 && g_lang_setting < s_lang_count)
        strncpy(requested_code, s_codes[g_lang_setting], sizeof(requested_code) - 1);
    scan_languages();
    /* Names are loaded from each file's S_LANGNAME_<CODE> metadata. */
    for (int i = 1; i < s_lang_count; i++)
        load_json(s_codes[i]);
    if (requested_code[0]) {
        s_lang = lang_from_code(requested_code);
        /* Directory iteration order is not guaranteed; keep the saved index in sync. */
        g_lang_setting = (s_lang == LANG_AUTO) ? LANG_AUTO : (unsigned char)s_lang;
    } else {
        s_lang = lang_from_code(system_code());
        if (s_lang <= 0 || s_lang >= s_lang_count) s_lang = lang_from_code("en");
        if (s_lang <= 0 || s_lang >= s_lang_count) s_lang = 1;
        g_lang_setting = LANG_AUTO;
    }
    load_language();
}

const char *L(StrId id)
{
    if (id < 0 || id >= S_COUNT) return "";
    return s_text[id][0] ? s_text[id] : "";
}

const char *lang_name(int id)
{
    if (id == LANG_AUTO) return L(S_LANG_AUTO);
    return id > 0 && id < s_lang_count ? s_names[id] : "";
}

const char *lang_code(int id)
{
    return id == LANG_AUTO ? "auto" : (id > 0 && id < s_lang_count ? s_codes[id] : "auto");
}

int lang_from_code(const char *code)
{
    if (s_lang_count <= 1) scan_languages();
    if (!code || !*code) return LANG_AUTO;
    if (!strcmp(code, "auto")) return LANG_AUTO;
    for (int i = 1; i < s_lang_count; i++)
        if (!strcmp(code, s_codes[i])) return i;
    return LANG_AUTO;
}

int lang_needs_cjk(void)
{
    return s_lang > 0 && s_lang < s_lang_count &&
           (!strcmp(s_codes[s_lang], "ja") || !strcmp(s_codes[s_lang], "ru"));
}
