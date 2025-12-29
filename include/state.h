#ifndef STATE_H
#define STATE_H

typedef enum {
    STATE_INIT = 0,
    STATE_READY,
    STATE_ACTIVE,
    STATE_DEGRADED,
    STATE_SAFE
} system_state_t;

const char* state_str(system_state_t st);

#endif
