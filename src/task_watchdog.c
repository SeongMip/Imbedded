#include "../include/task.h"
#include "../include/safety.h"
#include <unistd.h>

static int missed(uint32_t now, uint32_t last, uint32_t max_gap)
{
	return (now > last) && ((now - last) > max_gap);
}

void* watchdog_task(void* arg)
{
	(void)arg;

    while (1) {
        safety_heartbeat_kick(SRC_WATCHDOG_TASK);

        uint32_t now = safety_tick_get();

        // input/control/output가 300 tick(=3초) 이상 heartbeat 없으면 SAFE
        uint32_t in  = safety_heartbeat_get(SRC_INPUT_TASK);
        uint32_t ctl = safety_heartbeat_get(SRC_CONTROL_TASK);
        uint32_t out = safety_heartbeat_get(SRC_OUTPUT_TASK);

        if (missed(now, in, 300)) {
            safety_raise_fault(ERR_TASK_HEARTBEAT_LOST, SRC_WATCHDOG_TASK, SRC_INPUT_TASK);
        }
        if (missed(now, ctl, 300)) {
            safety_raise_fault(ERR_TASK_HEARTBEAT_LOST, SRC_WATCHDOG_TASK, SRC_CONTROL_TASK);
        }
        if (missed(now, out, 300)) {
            safety_raise_fault(ERR_TASK_HEARTBEAT_LOST, SRC_WATCHDOG_TASK, SRC_OUTPUT_TASK);
        }

        if (safety_get_state() == STATE_SAFE)
            break;

        usleep(200 * 1000);
    }

	return NULL;
}
