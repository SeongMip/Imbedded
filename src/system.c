#include "../include/system.h"
#include "../include/task.h"
#include "../include/queue.h"
#include "../include/safety.h"
#include "../include/error.h"
#include "../include/timebase.h"
#include "../include/time_delay.h"
#include "../include/state.h"
#include <stdio.h>

typedef struct {
    queue_t* q;
} task_ctx_t;

static queue_t g_q;
static int g_q_buf[8];

static pthread_t g_t_input;
static pthread_t g_t_control;
static pthread_t g_t_output;
static pthread_t g_t_watchdog;

static task_ctx_t g_ctx;

int system_init(void)
{
    error_log_init();
    safety_set_state(STATE_INIT);

    if (queue_init(&g_q, g_q_buf, 8) != 0) {
        safety_raise_fault(ERR_INIT_FAILED, SRC_SYSTEM, 1);
        return -1;
    }

    g_ctx.q = &g_q;

    if (timebase_start() != 0) {
        safety_raise_fault(ERR_INIT_FAILED, SRC_SYSTEM, 2);
        return -1;
    }

    safety_set_state(STATE_READY);
    return 0;
}

static void print_safe_report(void)
{
    safe_snapshot_t snap = safety_get_last_safe_snapshot();

    fprintf(stderr, "\n=== SYSTEM ENTERED SAFE ===\n");
    fprintf(stderr, "SAFE SNAPSHOT:\n");
    fprintf(stderr, "  tick=%u state=%s\n", snap.tick, state_str(STATE_SAFE));
    fprintf(stderr, "  cause: %s %s %s data=%u\n",
            error_sev_str(snap.sev),
            error_code_str(snap.code),
            error_src_str(snap.src),
            snap.data);
    fprintf(stderr, "  totals: total=%u critical=%u\n",
            snap.count_total_faults,
            snap.count_critical_faults);

    fprintf(stderr, "\nLAST EVENT (for post-mortem):\n");
    fprintf(stderr, "  [%u] %s %s %s data=%u\n",
            snap.last_event.tick,
            error_sev_str(snap.last_event.sev),
            error_code_str(snap.last_event.code),
            error_src_str(snap.last_event.src),
            snap.last_event.data);
}

int system_start(void)
{
    if (task_create(&g_t_input, input_task, "InputTask", &g_ctx) != 0)
        return 1;
    if (task_create(&g_t_control, control_task, "ControlTask", &g_ctx) != 0)
        return 1;
    if (task_create(&g_t_output, output_task, "OutputTask", &g_ctx) != 0)
        return 1;
    if (task_create(&g_t_watchdog, watchdog_task, "WatchdogTask", &g_ctx) != 0)
        return 1;

    // 데모: SAFE 들어가면 몇 초 후 로그 덤프하고 종료
    while (1) {
        system_state_t st = safety_get_state();
        if (st == STATE_READY) safety_set_state(STATE_ACTIVE);

        if (st == STATE_SAFE) {
			print_safe_report();
            time_delay_ms(300);
            break;
        }
		time_delay_ms(50);
    }

    timebase_stop();
    return 0;
}
