#include "../include/task.h"
#include <stdio.h>

int task_create(pthread_t* out, task_func_t func, const char* name, void* arg)
{
    if (!out || !func || !name)
        return -1;

    int rc = pthread_create(out, NULL, func, arg);
    if (rc != 0) {
        fprintf(stderr, "Failed to create %s (rc=%d)\n", name, rc);
        return -1;
    }
    return 0;
}
