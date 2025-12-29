PC 환경에서 RTOS Task 구조와 Fail-safe 상태 머신을 시뮬레이션하여
실시간 제약과 안전 설계를 검증한다

[Timer Thread]  → ISR 역할
       ↓
InputTask      (High priority)
       ↓ (Queue)
ControlTask    (Mid priority)
       ↓
OutputTask     (Low priority)

WatchdogTask   (Monitor all)

상태 전이 규칙
INIT → READY       : system init OK
READY → ACTIVE     : input valid
ACTIVE → DEGRADED  : transient fault
ANY → SAFE         : critical fault

ERR_INPUT_TIMEOUT → CRITICAL → SAFE
ERR_QUEUE_TIMEOUT → ERROR → DEGRADED
ERR_INPUT_OUT_OF_RANGE → WARN → DEGRADED (또는 SAFE)

모든 오류는 단일 진입점(safety_raise_fault)을 통해 보고되어 일관된 정책 적용이 가능합니다.

오류 로깅은 안전 정책과 분리됩니다: 모듈은 직접 로깅하지 않으며 오직 오류를 발생시킬 뿐입니다.

SAFE 상태 진입 시 사후 분석을 위한 스냅샷이 캡처됩니다.

1. Architecture (Task/Queue/Policy/Safety/Error)
2. Fault Handling Philosophy (단일 진입점)
3. Policy Table (표)
4. Escalation Rules (횟수/시간 창)
5. Logging Strategy (ring buffer + snapshot, dump 제한)
6. Verification (policy tests)

##################################################################
본 프로젝트는 POSIX usleep을 직접 사용하지 않고
time_delay_ms() API로 시간 지연을 추상화하였다.
이를 통해 Linux/RTOS 환경 간 이식성을 확보했으며,
busy wait을 배제하고 watchdog·safety 로직 이후에만 delay를 배치하여
시스템 안정성을 강화했다.

# RTOS Safety Policy Simulation (Pthreads)

## 1. Overview
This project simulates an RTOS-like architecture on Linux using `pthread`.
Key focus is **Safety-Critical design**:
- Single entry point for faults: `safety_raise_fault(code, src, data)`
- Centralized policy: `policy.c` (severity, state transitions, escalation)
- Deterministic logging: fixed-size ring buffer
- Post-mortem snapshot on SAFE state

## 2. Architecture
### Modules
- **Tasks**
  - `task_input.c`: generates ADC-like input values
  - `task_control.c`: converts input to brightness (0..100)
  - `task_output.c`: prints LED bar (degraded output limit)
  - `task_watchdog.c`: monitors heartbeats, triggers SAFE on loss
- **Safety**
  - `safety.c`: counts faults, logs events, applies policy, captures SAFE snapshot
- **Policy**
  - `policy.c`: maps `error_code -> severity`, determines `next_state`, applies escalation rules
- **Error**
  - `error.c`: fixed-size ring buffer, string formatting, `error_log_last()`
- **State**
  - `state.h/state.c`: `system_state_t` and `state_str()`

## 3. Safety Principles
### 3.1 Single Fault Entry Point
All modules report faults only through:
- `safety_raise_fault(error_code_t code, error_source_t src, uint32_t data)`

Tasks **never**:
- decide global safety state directly
- write error logs directly
- bypass policy by forcing CRITICAL severity

### 3.2 Fail-Fast on Invariant Violations
`ERR_INTERNAL_INVARIANT` represents a broken design assumption.
It escalates to SAFE immediately.

## 4. Policy Table (Human-Readable)
| Error Code | Base Severity | Escalation Rule (Window) | Next State |
|---|---|---|---|
| ERR_INIT_FAILED | CRITICAL | - | SAFE |
| ERR_INTERNAL_INVARIANT | CRITICAL | - | SAFE |
| ERR_TASK_HEARTBEAT_LOST | CRITICAL | - | SAFE |
| ERR_INPUT_TIMEOUT | ERROR | if count >= 3 -> CRITICAL (within 2s window) | DEGRADED / SAFE |
| ERR_QUEUE_TIMEOUT | WARN | if count >= 3 -> ERROR (within 2s window) | DEGRADED |
| ERR_INPUT_OUT_OF_RANGE | WARN | if count >= 10 -> ERROR (within 2s window) | DEGRADED |

> Source of truth: `policy.c`

## 5. Logging Strategy
### 5.1 Deterministic Memory Usage
- Fixed-size ring buffer (`ERROR_LOG_CAP`)
- No dynamic allocation for logs
- O(1) log push

### 5.2 Post-mortem Snapshot
On entering SAFE state, a snapshot is captured:
- fault cause (code/severity/source/data)
- counters (total/critical)
- last logged event

This provides minimal but sufficient evidence for analysis.

## 6. Build & Run
```bash
make
./rtos_sim
