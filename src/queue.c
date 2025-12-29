#include "../include/queue.h"
#include <time.h>
#include <errno.h>
#include <pthread.h>

static void timespec_add_ms(struct timespec* ts, int ms)
{
    ts->tv_sec += ms / 1000;
    ts->tv_nsec += (long)(ms % 1000) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000L;
    }
}

int queue_init(queue_t* q, int* backing_buf, size_t cap)
{
    if (!q || !backing_buf || cap == 0) return -1;

    q->buf = backing_buf;
    q->cap = cap;
    q->head = q->tail = q->count = 0;

	if (pthread_mutex_init(&q->mtx, NULL) != 0) return -1;

	pthread_condattr_t attr;

	if (pthread_condattr_init(&attr) != 0) {
		pthread_mutex_destroy(&q->mtx);
		return -1;
	}

	if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC) != 0) {
		pthread_condattr_destroy(&attr);
		pthread_mutex_destroy(&q->mtx);
		return -1;
	}

	if (pthread_cond_init(&q->cv, &attr) != 0) {
		pthread_condattr_destroy(&attr);
		pthread_mutex_destroy(&q->mtx);
		return -1;
	}
	pthread_condattr_destroy(&attr);

    return 0;
}

int queue_push(queue_t* q, int v)
{
    if (!q) return -1;

    pthread_mutex_lock(&q->mtx);

    if (q->count == q->cap) {
        pthread_mutex_unlock(&q->mtx);
        return -1; // overflow
    }

    q->buf[q->tail] = v;
    q->tail = (q->tail + 1) % q->cap;
    q->count++;

    pthread_cond_signal(&q->cv);
    pthread_mutex_unlock(&q->mtx);
    return 0;
}

// return 0 = 성공(값 pop) / 1 = timeout / -1 = error(파라미터/동기화/기타)
int queue_pop_timeout(queue_t* q, int* out, int timeout_ms)
{
    if (!q || !out || timeout_ms < 0) return -1;

    pthread_mutex_lock(&q->mtx);

    if (q->count == 0) {
        struct timespec ts;
	
		if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
			pthread_mutex_unlock(&q->mtx);
			return -1;
		}
        timespec_add_ms(&ts, timeout_ms);

        int rc = 0;
		// spurious wakeup 대비 조건 재검사
        while (q->count == 0 && rc == 0) {
            rc = pthread_cond_timedwait(&q->cv, &q->mtx, &ts);
        }

        if (q->count == 0) {
            pthread_mutex_unlock(&q->mtx);
            return (rc == ETIMEDOUT) ? 1 : -1;
        }
    }

    *out = q->buf[q->head];
    q->head = (q->head + 1) % q->cap;
    q->count--;

    pthread_mutex_unlock(&q->mtx);
    return 0;
}

int queue_destroy(queue_t* q)
{
	if (!q) return -1;
	pthread_cond_destroy(&q->cv);
	pthread_mutex_destroy(&q->mtx);
	return 0;
}
