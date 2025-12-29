#include "../include/control.h"

static volatile int g_brightness = 0;

static int clamp100(int v)
{
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

void control_set_brightness(int pct_0_100)
{
    g_brightness = clamp100(pct_0_100);
}

int control_get_brightness(void)
{
    return (int)g_brightness;
}

