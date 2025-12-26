#ifndef STATE_H
#define STATE_H

typedef enum {
    STATE_INIT,
    STATE_READY,
    STATE_ACTIVE,
    STATE_DEGRADED,
    STATE_SAFE
} system_state_t;

#endif
