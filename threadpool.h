#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include <pthread.h>
#include "queue.h"

typedef struct threadpool_t {
	int num_threads;
	int terminate;
	pthread_t* threads;
	pthread_cond_t* notifications;
	pthread_mutex_t* notify_mutexes;
	pthread_mutex_t queue_mutex;
	queue_t* queue;
} threadpool_t;

typedef void (*threadpool_cb)(void*);

void threadpool_init (threadpool_t* threadpool, int num_threads, queue_t* queue, threadpool_cb cb);
void threadpool_destroy (threadpool_t* threadpool);

#endif
