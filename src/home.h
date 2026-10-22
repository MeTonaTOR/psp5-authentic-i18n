/* psp5 – made by Mille Weber */
#ifndef HOME_H
#define HOME_H

#include "scan.h"

/* Home is the Games tab: at most HOME_MAX cards, made of the pinned ones (at most HOME_PIN_MAX, so there is always room
 * for the one played last) followed by the games and apps opened most recently, newest first. The UMD disc is not
 * counted: while one is inserted it is the first card.
 *
 * What Home remembers lives in <data dir>/home.txt: the pinned cards in the order they were pinned, and a longer list
 * of the cards opened last (so that removing one card brings the next one up). A card is named by entry_cover_key().
 * Every change is written to the file at once. */
#define HOME_MAX     10
#define HOME_PIN_MAX 9

/* bits of home_state() */
enum { HOME_SHOWN = 1, HOME_PINNED = 2 };

/* Reads home.txt. Without the file (first start) Home is filled once with the HOME_MAX games and apps added last. Keys
 * of games that are gone are dropped. Call again after the game list has been rebuilt. */
void home_load(const EntryList *list);

/* Fills `out` (room for max entries, HOME_MAX + 1 is enough) with the indices in `list` of the cards Home shows, left to
 * right. Returns how many. */
int home_build(const EntryList *list, int *out, int max);

/* HOME_SHOWN: the card (list index `index`) is in the Games tab now. HOME_PINNED: it is pinned. The disc counts as shown. */
int home_state(const EntryList *list, int index);

/* The card was started: it becomes the newest. */
void home_note_launch(const Entry *e);
/* Add to Home: the card becomes the newest. */
void home_add(const Entry *e);
/* Remove from Home: unpinned and forgotten, until it is started or added again. */
void home_remove(const Entry *e);
/* Pin to Home (a card that is not on Home is added first). 0 = HOME_PIN_MAX cards are pinned already, nothing changed. */
int home_pin(const Entry *e);
void home_unpin(const Entry *e);

#endif
