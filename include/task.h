#ifndef TASK_H
#define TASK_H

typedef void* (*task_func_t)(void*);

void task_create(task_func_t func const char* name);

#endif TASK_H
