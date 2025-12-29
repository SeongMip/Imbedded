#include "../include/state.h"

const char* state_str(system_state_t st)
{
    switch (st) {
    case STATE_INIT: return "INIT";
    case STATE_READY: return "READY";
    case STATE_ACTIVE: return "ACTIVE";
    case STATE_DEGRADED: return "DEGRADED";
    case STATE_SAFE: return "SAFE";
    default: return "UNKNOWN";
    }
}
