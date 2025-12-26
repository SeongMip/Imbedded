#include "task.h"

void* watchdog_task(void* arg)
{
    while (1)
    {
        check_task_heartbeat();
        if (fault_detected)
        {
            enter_safe_state("Task timeout");
        }
        sleep(1);
    }
}

