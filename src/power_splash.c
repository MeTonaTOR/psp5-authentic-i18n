/* PSP5 power-transition splash: short, theme-independent blue light transition. */
#include <pspkernel.h>
#include <math.h>
#include "gfx.h"
#include "power_splash.h"

static float clamp01(float v) { return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v; }
static float ease(float v) { v = clamp01(v); return v * v * (3.0f - 2.0f * v); }

/* manage_gfx=1 is for calls made after app_stop(); 0 reuses the active menu renderer. */
void power_splash_run(int mode, int manage_gfx)
{
    const float duration = 1.35f;
    const char *label = mode == POWER_SPLASH_SHUTDOWN ? "Shutting down..." :
                        mode == POWER_SPLASH_SLEEP ? "Entering rest mode..." :
                        mode == POWER_SPLASH_WAKE ? "Welcome back" : "Restarting...";
    u64 start;
    if (manage_gfx) {
        gfx_init();
        font_init(0);
    }
    start = sceKernelGetSystemTimeWide();
    while (1) {
        float t = (sceKernelGetSystemTimeWide() - start) / 1000000.0f;
        float in = ease(t / 0.30f);
        float out = 1.0f - ease((t - 0.88f) / 0.42f);
        float a = in * out;
        float sweep = (t / duration) * 620.0f - 70.0f;
        if (t > duration) break;

        gfx_begin(RGBA(1, 3, 10, 255));
        gfx_circle(240.0f, 122.0f, 105.0f + 8.0f * sinf(t * 2.0f),
                   RGBA(5, 27, 78, (int)(55.0f * a)));
        gfx_circle(240.0f, 122.0f, 68.0f + 6.0f * sinf(t * 2.7f),
                   RGBA(8, 58, 145, (int)(65.0f * a)));
        gfx_glow(sweep - 90.0f, 113.0f, 180.0f, 2.0f, 24.0f,
                 RGBA(65, 145, 255, (int)(105.0f * a)));
        gfx_line(sweep - 70.0f, 122.0f, sweep + 70.0f, 122.0f,
                 RGBA(175, 215, 255, (int)(150.0f * a)));
        /* Abstract console mark, avoiding use of Sony's trademark logo. */
        float pulse = 1.0f + 2.0f * sinf(t * 3.0f);
        gfx_ring(218.0f - pulse, 100.0f - pulse, 44.0f + pulse * 2.0f,
                 44.0f + pulse * 2.0f, 22.0f + pulse,
                 RGBA(205, 226, 255, (int)(185.0f * a)));
        gfx_ring(229.0f, 111.0f, 22.0f, 22.0f, 11.0f,
                 RGBA(235, 244, 255, (int)(205.0f * a)));
        gfx_circle(240.0f, 122.0f, 3.0f, RGBA(255, 255, 255, (int)(230.0f * a)));
        text_draw(240.0f, 181.0f, 0.65f, 0,
                  RGBA(215, 228, 250, (int)(225.0f * a)), ALIGN_CENTER, label);
        gfx_end();
    }
    if (manage_gfx) {
        font_term();
        gfx_term();
    }
}
