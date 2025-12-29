#include "../include/policy.h"
#include <stdio.h>

static int fail(const char* msg)
{
    printf("FAIL: %s\n", msg);
    return 1;
}

static int expect_int(const char* name, int got, int exp)
{
    if (got != exp) {
        printf("FAIL: %s (got=%d exp=%d)\n", name, got, exp);
        return 1;
    }
    printf("PASS: %s\n", name);
    return 0;
}

int main(void)
{
    int rc = 0;

    // severity 기본 정책 검증
    rc |= expect_int("sev INIT_FAILED == CRITICAL",
                     policy_severity_for(ERR_INIT_FAILED), SEV_CRITICAL);

    rc |= expect_int("sev INPUT_OUT_OF_RANGE == WARN",
                     policy_severity_for(ERR_INPUT_OUT_OF_RANGE), SEV_WARN);

    // 상태 전이 검증
    rc |= expect_int("ACTIVE + WARN -> DEGRADED",
                     policy_next_state(STATE_ACTIVE, ERR_INPUT_OUT_OF_RANGE, SEV_WARN),
                     STATE_DEGRADED);

    rc |= expect_int("DEGRADED + WARN -> DEGRADED",
                     policy_next_state(STATE_DEGRADED, ERR_INPUT_OUT_OF_RANGE, SEV_WARN),
                     STATE_DEGRADED);

    rc |= expect_int("ANY + CRITICAL -> SAFE",
                     policy_next_state(STATE_ACTIVE, ERR_INTERNAL_INVARIANT, SEV_CRITICAL),
                     STATE_SAFE);

    // escalation 검증
    rc |= expect_int("OUT_OF_RANGE WARN x10 -> ERROR",
                     policy_escalate(ERR_INPUT_OUT_OF_RANGE, SEV_WARN, 10),
                     SEV_ERROR);

    rc |= expect_int("QUEUE_TIMEOUT WARN x3 -> ERROR",
                     policy_escalate(ERR_QUEUE_TIMEOUT, SEV_WARN, 3),
                     SEV_ERROR);

    rc |= expect_int("INPUT_TIMEOUT ERROR x3 -> CRITICAL",
                     policy_escalate(ERR_INPUT_TIMEOUT, SEV_ERROR, 3),
                     SEV_CRITICAL);

    if (rc == 0)
        printf("ALL TESTS PASSED\n");
    return rc;
}
