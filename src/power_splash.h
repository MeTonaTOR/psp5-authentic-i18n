#ifndef POWER_SPLASH_H
#define POWER_SPLASH_H
enum {
    POWER_SPLASH_RESTART = 0,
    POWER_SPLASH_SHUTDOWN = 1,
    POWER_SPLASH_SLEEP = 2,
    POWER_SPLASH_WAKE = 3
};
/* Show a short transition. manage_gfx=1 after app_stop(), 0 while menu renderer is active. */
void power_splash_run(int mode, int manage_gfx);
#endif
