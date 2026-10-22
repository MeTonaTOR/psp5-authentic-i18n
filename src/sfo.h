/* psp5 – made by Mille Weber */
#ifndef SFO_H
#define SFO_H

#include <psptypes.h>

/* Kopierar strängvärdet för nyckeln `key` till out (nollterminerat).
 * Returnerar 1 om nyckeln hittades, annars 0. */
int sfo_get_string(const u8 *sfo, u32 sfo_size, const char *key, char *out, int out_size);

/* Går igenom alla poster i en PARAM.SFO (för informationsskärmen). sfo_count är 0 om datat inte är en
 * användbar SFO. sfo_entry kopierar nyckeln för post i och värdet som text (tal i decimal) och
 * returnerar 1, eller 0 om i ligger utanför eller posten är trasig. */
int sfo_count(const u8 *sfo, u32 sfo_size);
int sfo_entry(const u8 *sfo, u32 sfo_size, int i, char *key, int key_size, char *val, int val_size);

#endif
