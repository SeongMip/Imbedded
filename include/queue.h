#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stddef.h>

typedef struct {
    int*	buf;
	size_t	cap;
	size_t	head;
	size_t	tail;
	size_t	count;
    pthread_mutex_t mtx;
	pthread_cond_t cv;
} queue_t;

int queue_init(queue_t* q, int* backing_buf, size_t cap);
int queue_push(queue_t* q, int v);                 // non-blocking; overflow면 실패
int queue_pop_timeout(queue_t* q, int* out, int timeout_ms); // timeout_ms 동안 기다림

#endif
