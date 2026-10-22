#ifndef DIACRITIC_H
#define DIACRITIC_H

/* The PSP's system fonts (ltn0, ltn4 and the jpn0 fallback) have no glyphs for most Czech letters
 * with a hacek or a ring (c, r, t, ...). Such a letter is printed as its plain base letter and the
 * mark is drawn on top by psp5 itself, so it works whatever the fonts contain.
 *
 *   hacek  C c D E e N n R r S s T U Z z   -> small "v" above the letter
 *   tick   d' t'                           -> small slanted stroke beside the stem
 *   ring   U u                             -> small ring above the letter
 *
 * The marks are drawn as soft-edged lines (the GPU has no line smoothing of its own), so that they
 * look as smooth as the glyphs of the font.
 */

enum { DIAC_CARON = 1, DIAC_TICK, DIAC_RING };

#define DIAC_MAX_MARKS 48
#define DIAC_MAX_VERTS 128

typedef struct {
    short pos;  /* byte offset of the base letter in the plain string */
    char kind;  /* DIAC_CARON, DIAC_TICK or DIAC_RING */
    char upper; /* 1 for a capital letter */
    char base;  /* the plain letter, 'c' for the letter c with a hacek */
} DiacMark;

/* One corner of a triangle. `a` (0..1) multiplies the text colour's alpha: 1 on the line, fading
 * to 0 about a pixel away from it, which gives smooth edges. */
typedef struct {
    float x, y, a;
} DiacVert;

/* Quick test: might `s` (UTF-8) contain a letter that needs a drawn mark? */
int diac_maybe(const char *s);

/* Copies `s` to `out`, replacing every such letter with its base letter, and lists the replaced
 * letters in `marks`. Returns how many there are (0 = none, `out` is then a plain copy), or -1 if
 * `out` or `marks` is too small – the caller should then use `s` as it is. */
int diac_split(const char *s, char *out, int out_size, DiacMark *marks, int max_marks);

/* The mark as a list of triangles (3 corners each) in screen coordinates: `x` is where the letter
 * starts, `adv` its advance, `base` the baseline, `size` the text size (as given to text_draw) and
 * `bold` 1 for the bold font. Returns the number of corners written to `out` (at most `max`). */
int diac_mesh(const DiacMark *m, float x, float adv, float base, float size, int bold, DiacVert *out, int max);

#endif
