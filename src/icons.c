#include <math.h>
#include <string.h>
#include "icons.h"

static float clampf(float v, float lo, float hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

static float len2(float x, float y)
{
    return sqrtf(x * x + y * y);
}

/* --- signed distances in pixels (negative inside the shape) --- */

/* box around the origin with half sizes hx, hy and rounded corners of radius r */
static float sd_rbox(float px, float py, float hx, float hy, float r)
{
    float qx = fabsf(px) - (hx - r), qy = fabsf(py) - (hy - r);
    return len2(fmaxf(qx, 0), fmaxf(qy, 0)) + fminf(fmaxf(qx, qy), 0) - r;
}

/* a line from a to b with flat ends and the half width hw */
static float sd_line(float px, float py, float ax, float ay, float bx, float by, float hw)
{
    float dx = bx - ax, dy = by - ay, len = len2(dx, dy);
    dx /= len, dy /= len;
    float qx = px - (ax + bx) / 2, qy = py - (ay + by) / 2;
    return sd_rbox(qx * dx + qy * dy, -qx * dy + qy * dx, len / 2, hw, 0);
}

/* polygon v[2 * n] */
static float sd_poly(const float *v, int n, float px, float py)
{
    float d = (px - v[0]) * (px - v[0]) + (py - v[1]) * (py - v[1]);
    float s = 1;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        float ex = v[j * 2] - v[i * 2], ey = v[j * 2 + 1] - v[i * 2 + 1];
        float wx = px - v[i * 2], wy = py - v[i * 2 + 1];
        float h = clampf((wx * ex + wy * ey) / (ex * ex + ey * ey), 0, 1);
        float bx = wx - ex * h, by = wy - ey * h;
        d = fminf(d, bx * bx + by * by);
        int c1 = py >= v[i * 2 + 1], c2 = py < v[j * 2 + 1], c3 = ex * wy > ey * wx;
        if ((c1 && c2 && c3) || (!c1 && !c2 && !c3))
            s = -s;
    }
    return s * sqrtf(d);
}

/* --- the cog: 8 teeth, the first one 22.5 degrees from the vertical, as it has always been --- */

#define TEETH 8
#define GEAR_PTS (TEETH * 4)

static void gear_polygon(float r, float *v)
{
    const float root = r * 0.72f;            /* between the teeth */
    const float top = 8.0f * 3.14159265f / 180, base = 16.0f * 3.14159265f / 180; /* half angles */
    for (int k = 0; k < TEETH; k++) {
        float c = (k + 0.5f) * 2.0f * 3.14159265f / TEETH;
        const float a[4] = { c - base, c - top, c + top, c + base };
        const float rr[4] = { root, r, r, root };
        for (int i = 0; i < 4; i++) {
            v[(k * 4 + i) * 2] = cosf(a[i]) * rr[i];
            v[(k * 4 + i) * 2 + 1] = sinf(a[i]) * rr[i];
        }
    }
}

void icon_box(int kind, float cx, float cy, float w, float h, float r, IconBox *b)
{
    int sized = kind == ICON_RRECT || kind == ICON_TRI_R || kind == ICON_TRI_L; /* these have their own w * h */
    float ex = sized ? w : r * 2, ey = sized ? h : kind == ICON_SPEAKER ? r * 4 : r * 2;
    b->tw = (int)ceilf(ex) + 4; /* two pixels of margin on every side */
    b->th = (int)ceilf(ey) + 4;
    float fx = cx - b->tw / 2.0f, fy = cy - b->th / 2.0f;
    float x0 = floorf(fx), y0 = floorf(fy);
    int qx = (int)floorf((fx - x0) * 2 + 0.5f), qy = (int)floorf((fy - y0) * 2 + 0.5f); /* 0, 1 or 2 halves */
    if (qx == 2)
        qx = 0, x0 += 1;
    if (qy == 2)
        qy = 0, y0 += 1;
    b->x = (int)x0;
    b->y = (int)y0;
    b->phase_x = qx;
    b->phase_y = qy;
    b->cx = b->tw / 2.0f + qx * 0.5f;
    b->cy = b->th / 2.0f + qy * 0.5f;
}

typedef struct {
    int kind;
    float w, h, r;
    float sw, s, sq, tri[6];
    float bolt[12];
    float gear[GEAR_PTS * 2];
    float note[8], beam[8]; /* ICON_NOTE: lx, ly, rx, ry, top_l, top_r, head radius, stem width; the beam polygon */
    float cone[8];          /* ICON_SPEAKER: the cone polygon */
    float u;                /* ICON_SPEAKER: pixels per design unit */
} Shape;

/* The music note, as it was always drawn from separate pieces, but as one shape: two heads, two stems
 * and the beam between the tops of the stems. The size of the note is `s` (a square of that side). */
static void note_shape(float s, Shape *p)
{
    float rad = 0.2f * s, sw = fmaxf(0.09f * s, 1.2f);
    float lx = -0.28f * s, rx = 0.32f * s, ly = 0.32f * s, ry = 0.24f * s;
    float top_l = -0.42f * s, top_r = -0.5f * s, beam = 0.17f * s;
    const float n[8] = { lx, ly, rx, ry, top_l, top_r, rad, sw };
    const float b[8] = { lx + rad - sw, top_l, rx + rad, top_r, rx + rad, top_r + beam, lx + rad - sw, top_l + beam };
    memcpy(p->note, n, sizeof(n));
    memcpy(p->beam, b, sizeof(b));
}

static float sd_note(const Shape *p, float px, float py)
{
    const float *n = p->note;
    float d = fminf(len2(px - n[0], py - n[1]) - n[6], len2(px - n[2], py - n[3]) - n[6]); /* the two heads */
    float sw = n[7];
    d = fminf(d, sd_rbox(px - (n[0] + n[6] - sw / 2), py - (n[4] + n[1]) / 2, sw / 2, (n[1] - n[4]) / 2, 0));
    d = fminf(d, sd_rbox(px - (n[2] + n[6] - sw / 2), py - (n[5] + n[3]) / 2, sw / 2, (n[3] - n[5]) / 2, 0));
    return fminf(d, sd_poly(p->beam, 4, px, py));
}

/* The loudspeaker: a box (4 x 6 in design units) and a cone that widens from 6 to 15 units high. The
 * centre is the middle of the 9 unit wide shape. */
static void speaker_shape(float u, Shape *p)
{
    const float c[8] = { -1.0f * u, -3.0f * u, 4.5f * u, -7.5f * u, 4.5f * u, 7.5f * u, -1.0f * u, 3.0f * u };
    memcpy(p->cone, c, sizeof(c));
    p->u = u;
}

static float sd_speaker(const Shape *p, float px, float py)
{
    float u = p->u;
    return fminf(sd_rbox(px + 2.5f * u, py, 2.0f * u, 3.0f * u, 0), sd_poly(p->cone, 4, px, py));
}

/* The bolt: a zigzag 2 r high, drawn with the corners in this order (the units are 1/5.2 r). */
static void bolt_polygon(float r, float *v)
{
    static const float b[12] = { 1.6f, -5.2f, -2.6f, 0.9f, -0.2f, 0.9f, -1.6f, 5.2f, 2.6f, -0.9f, 0.2f, -0.9f };
    for (int i = 0; i < 12; i++)
        v[i] = b[i] * r / 5.2f;
}

/* signed distance in pixels from the centre of the shape to the point (px, py), negative inside */
static float shape_distance(const Shape *p, float px, float py)
{
    switch (p->kind) {
    case ICON_DISC:
        return len2(px, py) - p->r;
    case ICON_RRECT:
        return sd_rbox(px, py, p->w / 2, p->h / 2, fminf(p->r, fminf(p->w, p->h) / 2));
    case ICON_CIRCLE:
        return fabsf(len2(px, py) - 0.45f * p->r) - p->sw / 2;
    case ICON_CROSS:
        return fminf(sd_line(px, py, -p->s, -p->s, p->s, p->s, p->sw / 2),
                     sd_line(px, py, p->s, -p->s, -p->s, p->s, p->sw / 2));
    case ICON_TRIANGLE:
        return fabsf(sd_poly(p->tri, 3, px, py)) - p->sw / 2;
    case ICON_SQUARE:
        return fabsf(sd_rbox(px, py, p->sq - p->sw / 2, p->sq - p->sw / 2, 0.4f)) - p->sw / 2;
    case ICON_RRING: {
        float sd = sd_rbox(px, py, p->w / 2, p->h / 2, fminf(p->r, fminf(p->w, p->h) / 2));
        return fmaxf(sd, -sd - 0.19f * p->r); /* inside the edge, 0.19 r thick */
    }
    case ICON_RFRAME:
    case ICON_RFRAME_S: {
        float sd = sd_rbox(px, py, p->w / 2, p->h / 2, fminf(p->r, fminf(p->w, p->h) / 2));
        return fmaxf(sd, -sd - (p->kind == ICON_RFRAME ? ICON_FRAME_T : ICON_FRAME_T_S));
    }
    case ICON_BOLT:
        return sd_poly(p->bolt, 6, px, py);
    case ICON_TRI_R:
    case ICON_TRI_L:
        return sd_poly(p->tri, 3, px, py);
    case ICON_NOTE:
        return sd_note(p, px, py);
    case ICON_SPEAKER:
        return sd_speaker(p, px, py);
    default: /* ICON_GEAR */
        return fmaxf(sd_poly(p->gear, GEAR_PTS, px, py), -(len2(px, py) - 0.34f * p->r));
    }
}

void icon_render(int kind, float w, float h, float r, float cx, float cy, int tw, int th, unsigned char *out)
{
    Shape sh = { .kind = kind, .w = w, .h = h, .r = r };
    sh.sw = fmaxf(1.5f, 0.23f * r); /* line width of the symbols */
    sh.s = 0.45f * r;               /* half size of the symbols */
    /* Horizontal and vertical edges are moved onto whole pixels (by at most half a pixel), so that
     * the square and the base of the triangle are crisp; only the slanted lines and curves need
     * smoothing. The distances are measured from the centre, which is at (cx, cy) in the image. */
    float fx = cx - floorf(cx), fy = cy - floorf(cy);
    sh.sq = sh.s; /* half size of the square, outside edge */
    if (fabsf(fx - fy) < 0.01f) /* the same on both axes, otherwise it could not stay a square */
        sh.sq = floorf(sh.s - fx + 0.5f) + fx;
    float base = floorf(cy + 0.8f * sh.s + sh.sw / 2 + 0.5f) - cy - sh.sw / 2; /* the base of the triangle: outer edge on a pixel line */
    const float tri[6] = { 0, -(sh.s + 0.5f), -(sh.s + 0.5f), base, sh.s + 0.5f, base };
    memcpy(sh.tri, tri, sizeof(tri));
    if (kind == ICON_GEAR)
        gear_polygon(r, sh.gear);
    if (kind == ICON_BOLT)
        bolt_polygon(r, sh.bolt);
    if (kind == ICON_NOTE)
        note_shape(2 * r, &sh);
    if (kind == ICON_SPEAKER)
        speaker_shape(r / 4.5f, &sh);
    if (kind == ICON_TRI_R || kind == ICON_TRI_L) { /* the tip is in the middle of the far side */
        float d = kind == ICON_TRI_R ? 1.0f : -1.0f;
        const float t[6] = { -d * w / 2, -h / 2, d * w / 2, 0, -d * w / 2, h / 2 };
        memcpy(sh.tri, t, sizeof(t));
    }

    /* The coverage of a pixel follows from the distance of its centre to the edge (a one pixel wide ramp),
     * which is exact for straight edges and good for curves that are not too tight. Small round shapes
     * (a few pixels across) are tighter than that, so they are sampled 4 x 4 times per pixel instead.
     * So are the bolt, the triangles, the note and the speaker, whose sharp tips and thin stems are tighter still. */
    float feature = kind == ICON_DISC ? r
                  : kind == ICON_RRECT || kind == ICON_RRING || kind == ICON_RFRAME || kind == ICON_RFRAME_S
                      ? fminf(r, fminf(w, h) / 2) : 99;
    int fine = kind == ICON_BOLT || kind == ICON_TRI_R || kind == ICON_TRI_L || kind == ICON_NOTE || kind == ICON_SPEAKER;
    int ss = feature < 2.5f || fine ? 4 : 1;
    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            float cov;
            if (ss == 1) {
                cov = clampf(0.5f - shape_distance(&sh, x + 0.5f - cx, y + 0.5f - cy), 0, 1);
            } else {
                cov = 0;
                for (int j = 0; j < ss; j++)
                    for (int i = 0; i < ss; i++) /* each sample covers 1/4 pixel: a ramp 1/4 pixel wide */
                        cov += clampf(0.5f - 4 * shape_distance(&sh, x + (i + 0.5f) / ss - cx, y + (j + 0.5f) / ss - cy), 0, 1);
                cov /= ss * ss;
            }
            out[y * tw + x] = (unsigned char)(cov * 255.0f + 0.5f);
        }
    }
}

int icon_nine_k(float r)
{
    return (int)ceilf(r) + 2; /* the corner arc ends at 1 + r, and the pixel after it must be straight too */
}

/* Cuts one direction (n pixels) into at most three parts: the start, the stretched middle, the end.
 * The parts of the start and the end are as wide as the image allows (k), but never more than half
 * of what there is, so that they do not overlap in a small rectangle. */
static void nine_axis(int n, int k, float *a0, float *a1, float *b0, float *b1)
{
    int d = 2 * k + 1;
    int total = n + 2; /* with the transparent pixel on each side */
    int first = total / 2 < k ? total / 2 : k;
    int last = total - first < k ? total - first : k;
    a0[0] = 0, a1[0] = (float)first, b0[0] = 0, b1[0] = (float)first;
    a0[1] = (float)first, a1[1] = (float)(total - last), b0[1] = (float)k, b1[1] = (float)(k + 1);
    a0[2] = (float)(total - last), a1[2] = (float)total, b0[2] = (float)(d - last), b1[2] = (float)d;
}

int icon_nine(int w, int h, int k, IconCell *cells)
{
    float xa0[3], xa1[3], xb0[3], xb1[3], ya0[3], ya1[3], yb0[3], yb1[3];
    nine_axis(w, k, xa0, xa1, xb0, xb1);
    nine_axis(h, k, ya0, ya1, yb0, yb1);
    int n = 0;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (xa1[col] <= xa0[col] || ya1[row] <= ya0[row])
                continue; /* the middle part of a small rectangle has no size */
            cells[n++] = (IconCell){ xa0[col], ya0[row], xa1[col], ya1[row], xb0[col], yb0[row], xb1[col], yb1[row] };
        }
    }
    return n;
}
