#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <stdint.h>

typedef void* (*task_func_t)(void*);

int task_create(pthread_t* out, task_func_t func, const char* name, void* arg);

typedef enum {
	SRC_UNKNOWN = 0,
	SRC_INPUT_TASK,
	SRC_CONTROL_TASK,
	SRC_OUTPUT_TASK,
	SRC_WATCHDOG_TASK,
	SRC_TIMER_ISR_SIM,
	SRC_SYSTEM,
	SRC__COUNT
} error_source_t;

void* input_task(void* arg);
void* control_task(void* arg);
void* output_task(void* arg);
void* watchdog_task(void* arg);

#endif
