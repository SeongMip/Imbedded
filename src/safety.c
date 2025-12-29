#include "../include/safety.h"
#include "../include/policy.h"
#include <string.h>

// 단일 전역 상태(실무에서는 보호/원자성 고려)
static volatile system_state_t g_state = STATE_INIT;
static volatile uint32_t g_tick = 0;

// fault 카운터
static volatile uint32_t g_fault_total = 0;
static volatile uint32_t g_fault_critical = 0;

// SAFE 진입 당시 스냅샷
static safe_snapshot_t g_last_safe;

// heartbeat: src enum을 인덱스로 사용(단순화)
static volatile uint32_t g_heartbeat[SRC__COUNT];

typedef struct {
	uint32_t window_start_tick;
	uint16_t count;
} occ_t;

static occ_t g_occ[ERR__COUNT];

#define OCC_WINDOW_TICKS 200

static uint16_t occ_update_and_get(error_code_t code, uint32_t now_tick)
{
	if ((uint32_t)code >= (uint32_t)ERR__COUNT)
		return 0;
	
	occ_t* o = &g_occ[code];

	if (o->count == 0) {
		o->window_start_tick = now_tick;
		o->count = 1;
		return o->count;
	}

	// window가 지나면 리셋
	if ((now_tick - o->window_start_tick) > OCC_WINDOW_TICKS) {
		o->window_start_tick = now_tick;
		o->count = 1;
		return o->count;
	}

	// window 내 누적
	if (o->count < 65535)
		o->count++;

	return o->count;
}

system_state_t safety_get_state(void)
{
    return (system_state_t)g_state;
}

void safety_set_state(system_state_t st)
{
	g_state = st;
}

void safety_tick_set(uint32_t tick)
{
    g_tick = tick;
}

uint32_t safety_tick_get(void)
{
    return g_tick;
}

void safety_heartbeat_kick(error_source_t task_src)
{
    if ((uint32_t)task_src < (uint32_t)SRC__COUNT)
        g_heartbeat[task_src] = g_tick;
}

uint32_t safety_heartbeat_get(error_source_t task_src)
{
    if ((uint32_t)task_src < (uint32_t)SRC__COUNT)
        return g_heartbeat[task_src];
    return 0;
}

safe_snapshot_t safety_get_last_safe_snapshot(void)
{
    return g_last_safe;
}

void safety_raise_fault(error_code_t code, error_source_t src, uint32_t data)
{
	error_severity_t base = policy_severity_for(code);
	uint16_t cnt = occ_update_and_get(code, (uint32_t)g_tick);
	error_severity_t sev = policy_escalate(code, base, cnt);

    // 1) 카운트
    g_fault_total++;
    if (sev == SEV_CRITICAL) g_fault_critical++;

    // 2) 로그 기록(원인 보존)
    error_event_t ev;
    ev.tick = g_tick;
    ev.code = code;
    ev.sev  = sev;
    ev.src  = src;
    ev.data = data;
    error_log_push(ev);

    // 3) 상태 전이(정책)
	system_state_t cur = (system_state_t)g_state;
	system_state_t next = policy_next_state(cur, code, sev);
    g_state = next;

    // 4) SAFE 진입 시 스냅샷 저장(면접에서 강력 포인트)
    if (next == STATE_SAFE) {
        g_last_safe.tick = g_tick;
        g_last_safe.code = code;
        g_last_safe.sev  = sev;
        g_last_safe.src  = src;
        g_last_safe.data = data;
        g_last_safe.count_total_faults = g_fault_total;
        g_last_safe.count_critical_faults = g_fault_critical;
		(void)error_log_last((error_event_t*)&g_last_safe.last_event);
    }
}
