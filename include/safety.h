#ifndef SAFETY_H
#define SAFETY_H

#include <stdint.h>
#include "../include/state.h"
#include "../include/error.h"

// "fault 발생" 단일 진입점: task들은 이것만 호출 (severity는 policy가 결정)
void safety_raise_fault(error_code_t code, error_source_t src, uint32_t data);

// 시스템 상태 조회/설정
system_state_t safety_get_state(void);
void safety_set_state(system_state_t st);

// tick/heartbeat
void safety_tick_set(uint32_t tick);
uint32_t safety_tick_get(void);

void safety_heartbeat_kick(error_source_t task_src);
uint32_t safety_heartbeat_get(error_source_t task_src);

// SAFE 진입 시 어떤 정보를 남길지 (핵심)
typedef struct {
    uint32_t			tick;
    error_code_t		code;
    error_severity_t	sev;
    error_source_t		src;
    uint32_t			data;
    uint32_t			count_total_faults;
    uint32_t			count_critical_faults;
	error_event_t		last_event;
} safe_snapshot_t;

safe_snapshot_t safety_get_last_safe_snapshot(void);

#endif
