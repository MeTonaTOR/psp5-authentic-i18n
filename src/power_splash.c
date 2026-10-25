/* PSP5 power-transition splash: lightweight, theme-independent console animation. */
#include <pspkernel.h>
#include <math.h>
#include "gfx.h"
#include "power_splash.h"

static float clamp01(float v) { return v < 0.0f ? 0.0f : v > 1.0f ? 1.0f : v; }
static float ease(float v) { v = clamp01(v); return v * v * (3.0f - 2.0f * v); }

/* manage_gfx=1 is for calls made after app_stop(); 0 reuses the active menu renderer. */
void power_splash_run(int manage_gfx)
{
    const float duration = 1.15f;
    u64 start;
    if (manage_gfx)
        gfx_init();
    start = sceKernelGetSystemTimeWide();
    while (1) {
        float t = (sceKernelGetSystemTimeWide() - start) / 1000000.0f;
        float in = ease(t / 0.28f);
        float out = 1.0f - ease((t - 0.78f) / 0.37f);
        float a = in * out;
        if (t > duration) break;

        gfx_begin(RGBA(0, 0, 0, 255));
        /* Soft blue light blooms, inspired by the PS5 transition without copying its logo. */
        gfx_circle(240.0f, 136.0f, 90.0f + 10.0f * sinf(t * 2.2f),
                   RGBA(8, 35, 92, (int)(38.0f * a)));
        gfx_circle(240.0f, 136.0f, 58.0f + 5.0f * sinf(t * 3.1f),
                   RGBA(12, 62, 145, (int)(46.0f * a)));
        gfx_circle(240.0f, 136.0f, 25.0f,
                   RGBA(30, 100, 210, (int)(30.0f * a)));
        /* Minimal custom three-ring mark. */
        gfx_ring(220.0f, 116.0f, 40.0f, 40.0f, 20.0f,
                 RGBA(205, 220, 245, (int)(150.0f * a)));
        gfx_ring(229.0f, 125.0f, 22.0f, 22.0f, 11.0f,
                 RGBA(235, 242, 255, (int)(190.0f * a)));
        gfx_circle(240.0f, 136.0f, 3.0f,
                   RGBA(255, 255, 255, (int)(220.0f * a)));
        gfx_end();
    }
    if (manage_gfx)
        gfx_term();
}
