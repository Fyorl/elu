#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include <pthread.h>
#include "queue.h"

typedef void (*threadpool_cb)(void*);

typedef struct threadpool_t {
	int num_threads;
	int terminate;
	int* thread_status;
	pthread_t* threads;
	pthread_cond_t* notifications;
	pthread_mutex_t* notify_mutexes;
	pthread_mutex_t* status_mutexes;
	pthread_mutex_t queue_mutex;
	pthread_mutex_t terminate_mutex;
	queue_t* queue;
	threadpool_cb cb;
} threadpool_t;

void threadpool_init (threadpool_t* threadpool, int num_threads, queue_t* queue, threadpool_cb cb);
void threadpool_destroy (threadpool_t* threadpool);
void threadpool_add_work (threadpool_t* threadpool, void* work);

#endif
