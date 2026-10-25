#ifndef POWER_SPLASH_H
#define POWER_SPLASH_H
/* Show a short power transition. Pass 1 after app_stop(), 0 while the menu renderer is active. */
void power_splash_run(int manage_gfx);
#endif
