#include "../include/task.h"
#include "../include/queue.h"
#include "../include/safety.h"
#include "../include/time_delay.h"
#include "../include/control.h"
#include <unistd.h>

typedef struct {
    queue_t* q;
} task_ctx_t;

void* control_task(void* arg)
{
    task_ctx_t* ctx = (task_ctx_t*)arg;

    while (1) {
        safety_heartbeat_kick(SRC_CONTROL_TASK);

        int v = 0;
        // 100ms 안에 입력 못 받으면 timeout fault
        if (queue_pop_timeout(ctx->q, &v, 100) != 0) {
            safety_raise_fault(ERR_INPUT_TIMEOUT, SRC_CONTROL_TASK, 100);
            // degraded에선 유지/완만한 변화 등 정책 가능
            time_delay_ms(10);
        } else {
            // 0..4095 → 0..100
            int b = (v * 100) / 4095;
            g_brightness = b;
        }

        time_delay_ms(10 * 1000);
        if (safety_get_state() == STATE_SAFE)
            break;
    }
    return NULL;
}
