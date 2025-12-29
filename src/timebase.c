#include "../include/timebase.h"
#include "../include/time_delay.h"
#include "../include/safety.h"
#include <pthread.h>
#include <unistd.h>

static pthread_t g_thr;
static volatile int g_run = 0;

static void* timebase_thread(void* arg)
{
    (void)arg;
    uint32_t tick = 0;
    while (g_run) {
        time_delay_ms(10); // 10ms = 100Hz tick
        tick++;
        safety_tick_set(tick);
        safety_heartbeat_kick(SRC_TIMER_ISR_SIM);
    }
    return NULL;
}

int timebase_start(void)
{
    g_run = 1;
    if (pthread_create(&g_thr, NULL, timebase_thread, NULL) != 0)
        return -1;
    return 0;
}

void timebase_stop(void)
{
    g_run = 0;
    (void)pthread_join(g_thr, NULL);
}
