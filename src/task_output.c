#include "../include/task.h"
#include "../include/safety.h"
#include "../include/error.h"
#include "../include/control.h"
#include <stdio.h>
#include <unistd.h>

static void print_bar(int pct)
{
    int bars = (pct + 1) / 5; // 0..20
    printf("LED [%3d%%] |", pct);
    for (int i = 0; i < 20; i++)
        putchar(i < bars ? '#' : '.');
    printf("|\n");
}

void* output_task(void* arg)
{
    (void)arg;

    system_state_t last = safety_get_state();

    while (1) {
        safety_heartbeat_kick(SRC_OUTPUT_TASK);

        system_state_t st = safety_get_state();
        if (st != last) {
            printf("\n[STATE] %s -> %s (tick=%u)\n",
                   policy_state_str(last), policy_state_str(st), safety_tick_get());
            last = st;
        }

        if (st == STATE_SAFE) {
            printf("[OUTPUT] SAFE state entered. Output disabled.\n");
            break;
        }

        int b = control_get_brightness();

        // DEGRADED에서는 “출력 제한” 같은 걸 보여주기 좋음
        if (st == STATE_DEGRADED && b > 60)
            b = 60;

        print_bar(b);
        usleep(150 * 1000);
    }

    return NULL;
}
