#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 8

typedef struct {
    int buffer[QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
} queue_t;

#endif
