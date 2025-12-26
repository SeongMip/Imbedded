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
