#include "../include/policy.h"

error_severity_t policy_severity_for(error_code_t code)
{
    switch (code) {
    case ERR_NONE: return SEV_INFO;

    case ERR_INPUT_OUT_OF_RANGE: return SEV_WARN;
    case ERR_INPUT_TIMEOUT: return SEV_ERROR;
    case ERR_INPUT_STUCK: return SEV_ERROR;

    case ERR_QUEUE_OVERFLOW: return SEV_ERROR;
    case ERR_QUEUE_TIMEOUT:  return SEV_WARN;

    case ERR_TASK_MISSED_DEADLINE: return SEV_ERROR;
    case ERR_TASK_HEARTBEAT_LOST:  return SEV_CRITICAL;

    case ERR_INIT_FAILED: return SEV_CRITICAL;
    case ERR_INTERNAL_INVARIANT: return SEV_CRITICAL;

    default:
        return SEV_CRITICAL;
    }
}

error_severity_t policy_escalate(error_code_t code, error_severity_t base, uint16_t count_in_window)
{
	if (base == SEV_CRITICAL) return base;

	switch (code) {
	case ERR_INPUT_OUT_OF_RANGE:
		if (count_in_window >= 10) return SEV_ERROR;
		return base;

	case ERR_QUEUE_TIMEOUT:
		if (count_in_window >= 3) return SEV_ERROR;
		return base;

	case ERR_INPUT_TIMEOUT:
		if (count_in_window >= 3) return SEV_CRITICAL;
		return base;
	default:
		return base;
	}
}

system_state_t policy_next_state(system_state_t cur, error_code_t code, error_severity_t sev)
{
    (void)code;

    if (sev == SEV_CRITICAL)
        return STATE_SAFE;

    if (sev == SEV_ERROR) {
        if (cur == STATE_ACTIVE || cur == STATE_READY)
            return STATE_DEGRADED;
        return cur;
    }

    if (sev == SEV_WARN) {
        if (cur == STATE_ACTIVE)
            return STATE_DEGRADED;
        return cur;
    }

    return cur;
}
