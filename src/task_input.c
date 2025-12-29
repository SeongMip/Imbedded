#include "../include/task.h"
#include "../include/queue.h"
#include "../include/safety.h"
#include "../include/time_delay.h"
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    queue_t* q;
} task_ctx_t;

static int clamp(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void* input_task(void* arg)
{
    task_ctx_t* ctx = (task_ctx_t*)arg;
    int last = 0;
    int stuck_count = 0;

    while (1) {
        safety_heartbeat_kick(SRC_INPUT_TASK);

        // “가변저항” 대신 pseudo input 생성: 0..4095 근처, 가끔 이상값 주입
        int v = (rand() % 4200) - 50;   // -50..4149
        if ((rand() % 200) == 0) v = 9000; // 가끔 out-of-range 강제

        // stuck 감지(데모): 같은 값이 오래면 fault
        if (v == last) stuck_count++;
        else stuck_count = 0;
        last = v;

        // 범위 체크: 0..4095만 정상
        if (v < 0 || v > 4095) {
            safety_raise_fault(ERR_INPUT_OUT_OF_RANGE, SRC_INPUT_TASK, (uint32_t)v);
            // 안전 관점에서 값은 버리거나 clamp
            v = clamp(v, 0, 4095);
        }

        if (stuck_count > 200) {
            safety_raise_fault(ERR_INPUT_STUCK, SRC_INPUT_TASK, (uint32_t)v);
            stuck_count = 0;
        }

        // 큐 push (overflow면 fault)
        if (queue_push(ctx->q, v) != 0) {
            safety_raise_fault(ERR_QUEUE_OVERFLOW, SRC_INPUT_TASK, (uint32_t)v);
        }

        time_delay_ms(20); // 20ms 주기
        if (safety_get_state() == STATE_SAFE)
            break;
    }
    return NULL;
}
