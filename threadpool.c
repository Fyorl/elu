#include <pthread.h>

#include "threadpool.h"

struct worker_arg {
	int* terminate;
	queue_t* queue;
	pthread_mutex_t* queue_mutex;
	pthread_mutex_t* notify_mutex;
	pthread_cond_t* notification;
	threadpool_cb cb;
};

void* worker (void* data) {
	struct worker_arg* arg = data;

	while (1) {
		pthread_cond_wait(arg->notification, arg->notify_mutex);
		if (*(arg->terminate)) {
			break;
		}
	}

	free(arg);
	return NULL;
}

void threadpool_add_work (threadpool_t* threadpool, void* work) {
	pthread_mutex_lock(&(threadpool->queue_mutex));
	queue_add(threadpool->queue, work);
	pthread_mutex_unlock(&(threadpool->queue_mutex));

	int i;
}

void threadpool_init (threadpool_t* threadpool, int num_threads, queue_t* queue, threadpool_cb cb) {
	threadpool->num_threads = num_threads;
	threadpool->terminate = 0;
	threadpool->queue = queue;

	// Allocate memory for our arrays.
	threadpool->threads = calloc(num_threads, sizeof(pthread_t));
	threadpool->notifications = calloc(num_threads, sizeof(pthread_cond_t));
	threadpool->notify_mutexes = calloc(num_threads, sizeof(pthread_mutex_t));
	pthread_mutex_init(&(threadpool->queue_mutex), NULL);

	// Spawn the threads.
	int i;
	struct worker_arg* arg;

	for (i = 0; i < num_threads; i++) {
		arg = malloc(sizeof(struct worker_arg));
		arg->terminate = &(threadpool->terminate);
		arg->queue = threadpool->queue;
		arg->queue_mutex = &(threadpool->queue_mutex);
		arg->cb = cb;

		pthread_cond_init(&(threadpool->notifications[i]), NULL);
		arg->notification = &(threadpool->notifications[i]);

		pthread_mutex_init(&(threadpool->notify_mutexes[i]), NULL);
		arg->notify_mutex = &(threadpool->notify_mutexes[i]);

		pthread_create(&(threadpool->threads[i]), NULL, worker, (void*) arg);
	}
}

void threadpool_destroy (threadpool_t* threadpool) {
	threadpool->terminate = 1;

	// Wake up all the threads and wait for them to terminate.
	int i;
	for (i = 0; i < threadpool->num_threads; i++) {
		pthread_cond_signal(&(threadpool->notifications[i]));
		pthread_join(threadpool->threads[i], NULL);
	}

	free(threadpool->threads);
	free(threadpool->notifications);
	free(threadpool->notify_mutexes);
	queue_free(threadpool->queue);
}
