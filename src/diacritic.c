#include <math.h>
#include "diacritic.h"

/* Measures of the system font in pixels at text size 1.0 (all scale with the size). The cap height
 * follows from how the UI centres text: y + 4.5 at size 0.5 means about 17.5 px. */
#define CAP 17.5f   /* capital letters */
#define XH 12.8f    /* lower-case letters without ascender */
#define ASC 18.3f   /* d, l, b … */
#define TOP_T 16.0f /* top of the stem of t */

static const struct {
    unsigned short cp;
    char base;
    char kind;
    char upper;
} LETTERS[] = {
    { 0x010C, 'C', DIAC_CARON, 1 }, { 0x010D, 'c', DIAC_CARON, 0 },
    { 0x010E, 'D', DIAC_CARON, 1 }, { 0x010F, 'd', DIAC_TICK, 0 },
    { 0x011A, 'E', DIAC_CARON, 1 }, { 0x011B, 'e', DIAC_CARON, 0 },
    { 0x0147, 'N', DIAC_CARON, 1 }, { 0x0148, 'n', DIAC_CARON, 0 },
    { 0x0158, 'R', DIAC_CARON, 1 }, { 0x0159, 'r', DIAC_CARON, 0 },
    { 0x0160, 'S', DIAC_CARON, 1 }, { 0x0161, 's', DIAC_CARON, 0 },
    { 0x0164, 'T', DIAC_CARON, 1 }, { 0x0165, 't', DIAC_TICK, 0 },
    { 0x016E, 'U', DIAC_RING, 1 },  { 0x016F, 'u', DIAC_RING, 0 },
    { 0x017D, 'Z', DIAC_CARON, 1 }, { 0x017E, 'z', DIAC_CARON, 0 },
    /* Polish letters absent from the PSP Latin system fonts. */
    { 0x0104, 'A', DIAC_OGONEK, 1 }, { 0x0105, 'a', DIAC_OGONEK, 0 },
    { 0x0106, 'C', DIAC_ACUTE, 1 }, { 0x0107, 'c', DIAC_ACUTE, 0 },
    { 0x0118, 'E', DIAC_OGONEK, 1 }, { 0x0119, 'e', DIAC_OGONEK, 0 },
    { 0x0141, 'L', DIAC_STROKE, 1 }, { 0x0142, 'l', DIAC_STROKE, 0 },
    { 0x0143, 'N', DIAC_ACUTE, 1 }, { 0x0144, 'n', DIAC_ACUTE, 0 },
    { 0x00D3, 'O', DIAC_ACUTE, 1 }, { 0x00F3, 'o', DIAC_ACUTE, 0 },
    { 0x015A, 'S', DIAC_ACUTE, 1 }, { 0x015B, 's', DIAC_ACUTE, 0 },
    { 0x0179, 'Z', DIAC_ACUTE, 1 }, { 0x017A, 'z', DIAC_ACUTE, 0 },
    { 0x017B, 'Z', DIAC_DOT, 1 },   { 0x017C, 'z', DIAC_DOT, 0 },
};
#define N_LETTERS (int)(sizeof(LETTERS) / sizeof(LETTERS[0]))

/* These Latin Extended letters use two-byte UTF-8 sequences (C3, C4 or C5). */
int diac_maybe(const char *s)
{
    for (; *s; s++)
        if ((unsigned char)*s == 0xC3 || (unsigned char)*s == 0xC4 || (unsigned char)*s == 0xC5)
            return 1;
    return 0;
}

int diac_split(const char *s, char *out, int out_size, DiacMark *marks, int max_marks)
{
    int n = 0, o = 0;
    while (*s) {
        unsigned char c = (unsigned char)*s;
        int hit = -1;
        if ((c == 0xC3 || c == 0xC4 || c == 0xC5) && ((unsigned char)s[1] & 0xC0) == 0x80) {
            unsigned cp = ((c & 0x1Fu) << 6) | ((unsigned char)s[1] & 0x3Fu);
            for (int i = 0; i < N_LETTERS; i++)
                if (LETTERS[i].cp == cp) {
                    hit = i;
                    break;
                }
        }
        if (hit >= 0) {
            if (o + 1 >= out_size || n >= max_marks)
                return -1;
            marks[n] = (DiacMark){ (short)o, LETTERS[hit].kind, LETTERS[hit].upper, LETTERS[hit].base };
            n++;
            out[o++] = LETTERS[hit].base;
            s += 2;
        } else {
            if (o + 1 >= out_size)
                return -1;
            out[o++] = *s++;
        }
    }
    out[o] = 0;
    return n;
}

static float clampf(float v, float lo, float hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

/* --- soft-edged lines ---
 * A line of width w becomes a strip of triangles whose cross-section has three corners: alpha 1 on
 * the centre line and 0 one `fade` (= max(w, 1) px) to each side. The GPU blends the colour across
 * that "tent", which is what anti-aliasing a line about a pixel wide looks like. A line thinner
 * than a pixel gets a lower peak instead, so it fades rather than disappears. The ends fade out the
 * same way. */

typedef struct {
    float x, y;   /* centre */
    float nx, ny; /* normal, scaled so that the edges of the line stay `fade` away at corners */
    float a;      /* peak alpha of this cross-section */
} Sect;

#define MAX_SECT 14

static int add_tri(DiacVert *out, int n, int max, DiacVert a, DiacVert b, DiacVert c)
{
    if (n + 3 > max)
        return n;
    out[n++] = a;
    out[n++] = b;
    out[n++] = c;
    return n;
}

/* The polyline p[2*np]; `closed` joins the last point to the first (a ring). */
static int stroke(const float *p, int np, int closed, float w, DiacVert *out, int max)
{
    float fade = fmaxf(w, 1.0f), peak = fminf(w, 1.0f);
    Sect s[MAX_SECT];
    int ns = 0;
    /* unit directions of the segments */
    float dx[MAX_SECT], dy[MAX_SECT];
    int nseg = closed ? np : np - 1;
    if (np < 2 || np + 4 > MAX_SECT)
        return 0;
    for (int i = 0; i < nseg; i++) {
        int j = (i + 1) % np;
        float ex = p[j * 2] - p[i * 2], ey = p[j * 2 + 1] - p[i * 2 + 1];
        float len = sqrtf(ex * ex + ey * ey);
        if (len < 1e-4f)
            len = 1;
        dx[i] = ex / len;
        dy[i] = ey / len;
    }
    /* one cross-section per point; a mitre at the corners, but never more than twice the width */
    for (int i = 0; i < np; i++) {
        int has_prev = closed || i > 0, has_next = closed || i < np - 1;
        int sp = (i + nseg - 1) % nseg, sn = i % nseg;
        float nx, ny;
        if (has_prev && has_next) {
            float ax = -dy[sp], ay = dx[sp], bx = -dy[sn], by = dx[sn];
            float mx = ax + bx, my = ay + by, ml = sqrtf(mx * mx + my * my);
            if (ml < 1e-4f) {
                nx = ax, ny = ay;
            } else {
                mx /= ml, my /= ml;
                float k = 1.0f / fmaxf(mx * ax + my * ay, 0.5f);
                nx = mx * k, ny = my * k;
            }
        } else {
            int sg = has_next ? sn : sp;
            nx = -dy[sg], ny = dx[sg];
        }
        s[ns++] = (Sect){ p[i * 2], p[i * 2 + 1], nx, ny, peak };
    }
    if (closed) {
        s[ns] = s[0]; /* back to the start */
        ns++;
    } else {
        /* fade the two ends out along the line */
        Sect first = s[0], last = s[ns - 1];
        float cap = 0.6f;
        for (int i = ns; i > 0; i--)
            s[i] = s[i - 1];
        s[0] = first;
        s[0].x -= dx[0] * cap, s[0].y -= dy[0] * cap, s[0].a = 0;
        ns++;
        s[ns] = last;
        s[ns].x += dx[nseg - 1] * cap, s[ns].y += dy[nseg - 1] * cap, s[ns].a = 0;
        ns++;
    }
    int n = 0;
    for (int i = 0; i + 1 < ns; i++) {
        const Sect *a = &s[i], *b = &s[i + 1];
        /* corners: left edge (alpha 0), centre line, right edge (alpha 0) */
        DiacVert al = { a->x - a->nx * fade, a->y - a->ny * fade, 0 }, ac = { a->x, a->y, a->a },
                 ar = { a->x + a->nx * fade, a->y + a->ny * fade, 0 };
        DiacVert bl = { b->x - b->nx * fade, b->y - b->ny * fade, 0 }, bc = { b->x, b->y, b->a },
                 br = { b->x + b->nx * fade, b->y + b->ny * fade, 0 };
        n = add_tri(out, n, max, al, ac, bl);
        n = add_tri(out, n, max, ac, bc, bl);
        n = add_tri(out, n, max, ac, ar, bc);
        n = add_tri(out, n, max, ar, br, bc);
    }
    return n;
}

/* Where the ink of a letter is centred, relative to the middle of its advance, in pixels at size 1.0.
 * Letters with the stem on the left (R, D, E, r) have their ink to the right of the middle; the
 * others are centred. Measured on DejaVu Sans, Liberation Sans and FreeSans, which agree. */
static float optical_shift(char base)
{
    switch (base) {
    case 'R': return 0.60f;
    case 'D': return 0.47f;
    case 'E': return 0.53f;
    case 'r': return 0.70f;
    default: return 0;
    }
}

int diac_mesh(const DiacMark *m, float x, float adv, float base, float size, int bold, DiacVert *out, int max)
{
    float p[2 * 12];
    float cx = x + adv / 2 + optical_shift(m->base) * size; /* over the middle of the ink */
    float weight = bold ? 1.3f : 1.0f; /* the bold font has thicker strokes */
    switch (m->kind) {
    case DIAC_TICK: { /* d' and t': a short stroke beside the top of the stem */
        int is_d = m->base == 'd';
        /* just right of the stem: the stem of d ends about 1.8 px before the end of the advance, the
         * stem of t about 0.57 of the way along it */
        float sx = is_d ? x + adv - 1.4f * size : x + adv * 0.57f;
        float top = base - (is_d ? ASC + 0.4f : TOP_T + 1.4f) * size;
        p[0] = sx + 1.0f * size, p[1] = top;
        p[2] = sx + 0.2f * size, p[3] = top + 3.4f * size;
        return stroke(p, 2, 0, fmaxf(0.85f, 1.2f * size) * weight, out, max);
    }
    case DIAC_ACUTE: { /* Polish acute accent: ć ń ó ś ź */ 
        float sx = cx - 1.0f * size, sy = base - (m->upper ? CAP : XH) * size - 1.0f * size;
        p[0] = sx - 0.8f * size; p[1] = sy - 2.5f * size;
        p[2] = sx + 1.5f * size; p[3] = sy - 5.8f * size;
        return stroke(p, 2, 0, fmaxf(0.85f, 1.2f * size) * weight, out, max);
    }
    case DIAC_DOT: { /* ż / Ż */
        float r = fmaxf(0.8f, 1.15f * size);
        float cy = base - (m->upper ? CAP : XH) * size - 3.4f * size;
        for (int i = 0; i < 8; i++) {
            float a = (float)i / 8 * 6.2831853f;
            p[i * 2] = cx + cosf(a) * r;
            p[i * 2 + 1] = cy + sinf(a) * r;
        }
        return stroke(p, 8, 1, fmaxf(0.8f, size) * weight, out, max);
    }
    case DIAC_OGONEK: { /* ą / ę: a small hook below the baseline */
        float sx = x + adv * 0.62f;
        p[0] = sx; p[1] = base - 1.0f * size;
        p[2] = sx + 1.0f * size; p[3] = base + 1.0f * size;
        p[4] = sx + 2.5f * size; p[5] = base + 0.5f * size;
        return stroke(p, 3, 0, fmaxf(0.85f, 1.15f * size) * weight, out, max);
    }
    case DIAC_STROKE: { /* ł / Ł: diagonal stroke through the letter */
        p[0] = x + adv * 0.18f; p[1] = base - (m->upper ? CAP * 0.55f : XH * 0.55f) * size;
        p[2] = x + adv * 0.82f; p[3] = base - (m->upper ? CAP * 0.10f : XH * 0.10f) * size;
        return stroke(p, 2, 0, fmaxf(0.85f, 1.2f * size) * weight, out, max);
    }
    case DIAC_RING: { /* U and u */
        float r = 1.3f * size + 0.35f;
        float ring_y = base - (m->upper ? CAP : XH) * size - (0.6f * size + 0.5f) - r;
        for (int i = 0; i < 8; i++) {
            float a = (float)i / 8 * 6.2831853f;
            p[i * 2] = cx + cosf(a) * r;
            p[i * 2 + 1] = ring_y + sinf(a) * r;
        }
        return stroke(p, 8, 1, fmaxf(0.8f, 1.0f * size) * weight, out, max);
    }
    default: { /* hacek: a "v" */
        float w = clampf(adv * 0.5f, 3.0f * size, 5.4f * size);
        float ch = 1.8f * size + 0.3f;
        float apex = base - (m->upper ? CAP : XH) * size - (0.7f * size + 0.5f);
        p[0] = cx - w / 2, p[1] = apex - ch;
        p[2] = cx, p[3] = apex;
        p[4] = cx + w / 2, p[5] = apex - ch;
        return stroke(p, 3, 0, fmaxf(0.85f, 1.25f * size) * weight, out, max);
    }
    }
}
