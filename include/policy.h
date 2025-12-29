#ifndef POLICY_H
#define POLICY_H

#include "state.h"
#include "error.h"
#include <stdint.h>

// 기본 severity(정책표)
error_severity_t policy_severity_for(error_code_t code);
// 상태 전이 규칙
system_state_t policy_next_state(system_state_t cur, error_code_t code, error_severity_t sev);
// escalation: 최근 window 내 count 기반으로 severity 승격
error_severity_t policy_escalate(error_code_t code, error_severity_t base, uint16_t count_in_window);

#endif
