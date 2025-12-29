#include "../include/error.h"
#include <string.h>

#ifndef ERROR_LOG_CAP
#define ERROR_LOG_CAP 64
#endif

static error_event_t g_log[ERROR_LOG_CAP];
static size_t g_head = 0;
static size_t g_count = 0;

void error_log_init(void)
{
    memset(g_log, 0, sizeof(g_log));
    g_head = 0;
    g_count = 0;
}

void error_log_push(error_event_t ev)
{
    g_log[g_head] = ev;
    g_head = (g_head + 1) % ERROR_LOG_CAP;
    if (g_count < ERROR_LOG_CAP)
        g_count++;
}

size_t error_log_dump(error_event_t* out, size_t max_out)
{
    if (!out || max_out == 0) return 0;

    // 오래된 것부터 복사
    size_t n = (g_count < max_out) ? g_count : max_out;
    size_t start = (g_head + ERROR_LOG_CAP - g_count) % ERROR_LOG_CAP;

    for (size_t i = 0; i < n; i++) {
        out[i] = g_log[(start + i) % ERROR_LOG_CAP];
    }
    return n;
}

int error_log_last(error_event_t* out)
{
	if (!out || g_count == 0) return -1;
	size_t idx = (g_head + ERROR_LOG_CAP - 1) % ERROR_LOG_CAP;
	*out = g_log[idx];
	return 0;
}

const char* error_code_str(error_code_t code)
{
    switch (code) {
    case ERR_NONE: return "NONE";
    case ERR_INPUT_OUT_OF_RANGE: return "INPUT_OUT_OF_RANGE";
    case ERR_INPUT_TIMEOUT: return "INPUT_TIMEOUT";
    case ERR_INPUT_STUCK: return "INPUT_STUCK";
    case ERR_QUEUE_OVERFLOW: return "QUEUE_OVERFLOW";
    case ERR_QUEUE_TIMEOUT: return "QUEUE_TIMEOUT";
    case ERR_TASK_MISSED_DEADLINE: return "TASK_MISSED_DEADLINE";
    case ERR_TASK_HEARTBEAT_LOST: return "TASK_HEARTBEAT_LOST";
    case ERR_INIT_FAILED: return "INIT_FAILED";
    case ERR_INTERNAL_INVARIANT: return "INTERNAL_INVARIANT";
    default: return "UNKNOWN";
    }
}

const char* error_sev_str(error_severity_t sev)
{
    switch (sev) {
    case SEV_INFO: return "INFO";
    case SEV_WARN: return "WARN";
    case SEV_ERROR: return "ERROR";
    case SEV_CRITICAL: return "CRITICAL";
    default: return "UNKNOWN";
    }
}

const char* error_src_str(error_source_t src)
{
    switch (src) {
    case SRC_UNKNOWN: return "UNKNOWN";
    case SRC_INPUT_TASK: return "INPUT_TASK";
    case SRC_CONTROL_TASK: return "CONTROL_TASK";
    case SRC_OUTPUT_TASK: return "OUTPUT_TASK";
    case SRC_WATCHDOG_TASK: return "WATCHDOG_TASK";
    case SRC_TIMER_ISR_SIM: return "TIMER_ISR_SIM";
    case SRC_SYSTEM: return "SYSTEM";
    default: return "UNKNOWN";
    }
}
