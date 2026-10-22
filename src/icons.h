#ifndef ICONS_H
#define ICONS_H

/* Smooth icon shapes: the button symbols, the cog and rounded rectangles of the small key caps.
 *
 * They are rendered on the CPU into a small alpha image of exactly the size they are drawn at,
 * with the edges anti-aliased from the real geometry, so they are as smooth as the glyphs of the
 * font. gfx.c keeps the images in a cache and draws them pixel for pixel, tinted with the colour.
 * (Shrinking a big round texture, as gfx_circle does, only looks at a few of its pixels and gives
 * jagged edges at this size.) */

enum {
    ICON_DISC,     /* filled circle, radius r */
    ICON_RRECT,    /* rounded rectangle w * h with corner radius r */
    /* the symbols below are drawn inside a button of radius r, in the same size of image as ICON_DISC */
    ICON_CIRCLE,   /* ring */
    ICON_CROSS,
    ICON_TRIANGLE,
    ICON_SQUARE,   /* outline of a square */
    ICON_GEAR,     /* cog with a hole, outer radius r */
    ICON_RRING,    /* rounded rectangle w * h, only a thin ring just inside the edge (0.19 r thick) */
    ICON_BOLT,     /* the charging bolt, 2 r high (about r wide) */
    ICON_TRI_R,    /* triangle w * h pointing right: the back edge on the left, the tip in the middle of the right */
    ICON_TRI_L,    /* the same pointing left */
    ICON_NOTE,     /* the music note (two notes with a beam) in a square of side 2 r, all in one piece */
    ICON_SPEAKER,  /* the loudspeaker: box and cone in one piece, 2 r wide and 4 r high (r = 4.5 for the usual size) */
    ICON_RFRAME,   /* rounded rectangle w * h, a ring of the fixed thickness ICON_FRAME_T just inside the edge */
    ICON_RFRAME_S  /* the same with the thinner ICON_FRAME_T_S - the ring around the selected card of the library grid */
};

#define ICON_FRAME_T   2.5f /* thickness of ICON_RFRAME in pixels (about what the big card's ring always had) */
#define ICON_FRAME_T_S 2.0f /* thickness of ICON_RFRAME_S in pixels (about what the small grid cards' ring always had) */

/* Where an icon goes: the size of its image, the whole-pixel position of the image's top left
 * corner on the screen, and where the centre of the shape is inside the image. The centre is
 * snapped to half a pixel, so only a few different images are ever needed (`phase_x`/`phase_y`,
 * 0 or 1, tell which, for the cache). */
typedef struct {
    int tw, th;
    int x, y;
    float cx, cy;
    int phase_x, phase_y;
} IconBox;

/* The box for a shape centred on (cx, cy) on the screen. `w`, `h` are only used by ICON_RRECT and
 * the triangles (where they are the size of the triangle). */
void icon_box(int kind, float cx, float cy, float w, float h, float r, IconBox *b);

/* Writes the coverage (0 = outside, 255 = inside) of the shape for every pixel of a tw * th image
 * to out[tw * th]. The centre of the shape is at (cx, cy) measured in pixels from the top left
 * corner of the image. `w` and `h` are only used by ICON_RRECT and the triangles. */
void icon_render(int kind, float w, float h, float r, float cx, float cy, int tw, int th, unsigned char *out);

/* --- rounded rectangles of any size from one small image ---
 * The image has the shape of a square rounded rectangle with a transparent pixel around it and one
 * pixel column and row in the middle that can be stretched. It is cut into up to 9 cells, the four
 * corners drawn pixel for pixel and the rest stretched, so a rectangle w * h on whole pixels comes
 * out exactly as if it had been rendered as a whole. */

#define ICON_NINE_MAX_R 28 /* bigger radii are not worth an image (and use the old drawing) */

/* k: half the size of the image; it is 2k+1 pixels wide and high. */
int icon_nine_k(float r);

/* The image of ICON_RRECT or ICON_RRING for radius r: call icon_render with
 * w = h = 2k-1, the centre at (k+0.5, k+0.5) and tw = th = 2k+1. */

/* One piece: the rectangle x0,y0 - x1,y1 on the screen (relative to the top left corner of the
 * rounded rectangle, one pixel outside it on every side) shows the part u0,v0 - u1,v1 of the image
 * (in pixels). */
typedef struct {
    float x0, y0, x1, y1;
    float u0, v0, u1, v1;
} IconCell;

/* The pieces for a rounded rectangle of w * h whole pixels; returns how many (at most 9). The
 * rectangle itself is x = 1 .. w+1, y = 1 .. h+1 in these coordinates. */
int icon_nine(int w, int h, int k, IconCell *cells);

#endif
