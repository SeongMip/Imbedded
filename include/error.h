#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>
#include <stddef.h>
#include "task.h"

typedef enum {
    ERR_NONE = 0,

    // Input/ADC 계열
    ERR_INPUT_OUT_OF_RANGE,
    ERR_INPUT_TIMEOUT,
    ERR_INPUT_STUCK,

    // RTOS/IPC 계열
    ERR_QUEUE_OVERFLOW,
    ERR_QUEUE_TIMEOUT,

    // Timing/Health 계열
    ERR_TASK_MISSED_DEADLINE,
    ERR_TASK_HEARTBEAT_LOST,

    // System 계열
    ERR_INIT_FAILED,
    ERR_INTERNAL_INVARIANT,

    ERR__COUNT
} error_code_t;

// 심각도(정책 결정의 입력)
typedef enum {
    SEV_INFO = 0,
    SEV_WARN,
    SEV_ERROR,
    SEV_CRITICAL
} error_severity_t;

// 기록 한 건 (확장 가능한 최소 필드)
typedef struct {
    uint32_t tick;          // "언제" (시뮬레이션 tick)
    error_code_t code;      // "무슨 문제"
    error_severity_t sev;   // "심각도"
    error_source_t src;     // "어디서"
    uint32_t data;          // 부가정보(값/카운터/큐사용량 등)
} error_event_t;

// 기록 API
void error_log_init(void);
void error_log_push(error_event_t ev);
size_t error_log_dump(error_event_t* out, size_t max_out);

// 가장 최근 이벤트 로그
int error_log_last(error_event_t* out);

// 표현(문자열 변환)
const char* error_code_str(error_code_t code);
const char* error_sev_str(error_severity_t sev);
const char* error_src_str(error_source_t src);

#endif
