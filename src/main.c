#include "/include/system.h"
#include "/include/state.h"
#include "/include/queue.h"
#include "/include/task.h"
#include "/include/safety.h"

int main(void)
{
    system_init();

    task_create(input_task, "InputTask");
    task_create(control_task, "ControlTask");
    task_create(output_task, "OutputTask");
    task_create(watchdog_task, "WatchdogTask");

    system_start(); // join or loop
}
