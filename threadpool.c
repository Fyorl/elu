#include <pthread.h>

#include "threadpool.h"

struct worker_arg {
	int id;
	threadpool_t* threadpool;
};

void* worker (void* data) {
	struct worker_arg* arg = data;
	int id = arg->id;
	threadpool_t* threadpool = arg->threadpool;
	void* element;

	while (1) {
		pthread_mutex_lock(&(threadpool->terminate_mutex));
		if (threadpool->terminate) {
			pthread_mutex_unlock(&(threadpool->terminate_mutex));
			break;
		}
		pthread_mutex_unlock(&(threadpool->terminate_mutex));

		pthread_cond_wait(&(threadpool->notifications[id]), &(threadpool->notify_mutexes[id]));

		while (1) {
			pthread_mutex_lock(&(threadpool->queue_mutex));
			element = queue_dequeue(threadpool->queue);
			pthread_mutex_unlock(&(threadpool->queue_mutex));

			if (element == NULL) {
				// Queue was empty so we can set our status to idle then sleep.
				pthread_mutex_lock(&(threadpool->status_mutexes[id]));
				threadpool->thread_status[id] = 0;
				pthread_mutex_unlock(&(threadpool->status_mutexes[id]));
				break;
			}

			(threadpool->cb)(element);
		}
	}

	free(arg);
	return NULL;
}

void threadpool_add_work (threadpool_t* threadpool, void* work) {
	pthread_mutex_lock(&(threadpool->queue_mutex));
	queue_add(threadpool->queue, work);
	pthread_mutex_unlock(&(threadpool->queue_mutex));

	/*
	 * We add work to the queue and then loop through all the threads and check
	 * their statuses. If we find an idle thread, wake it up and set its status
	 * to busy.
	 */
	int i;
	for (i = 0; i < threadpool->num_threads; i++) {
		if (!threadpool->thread_status[i]) {
			pthread_mutex_lock(&(threadpool->status_mutexes[i]));
			threadpool->thread_status[i] = 1;
			pthread_mutex_unlock(&(threadpool->status_mutexes[i]));

			// Wake the thread.
			pthread_cond_signal(&(threadpool->notifications[i]));
			break;
		}
	}
}

void threadpool_init (threadpool_t* threadpool, int num_threads, queue_t* queue, threadpool_cb cb) {
	/*
	 * The thread pool works by spawning N threads at once and keeping track of
	 * them in an array. We also create N mutexes and N conditional variables
	 * so that each thread has its own set. The threads 'sleep' until woken
	 * by the main thread whenever new data is added to the queue. They then
	 * pop the first item off the queue and pass it to the callback function
	 * 'cb'. They then check for any additional work still in the queue before
	 * going back to sleep.
	 */

	threadpool->num_threads = num_threads;
	threadpool->terminate = 0;
	threadpool->queue = queue;
	threadpool->cb = cb;

	// Allocate memory for our arrays.
	threadpool->thread_status = calloc(num_threads, sizeof(int));
	threadpool->threads = calloc(num_threads, sizeof(pthread_t));
	threadpool->notifications = calloc(num_threads, sizeof(pthread_cond_t));
	threadpool->notify_mutexes = calloc(num_threads, sizeof(pthread_mutex_t));
	threadpool->status_mutexes = calloc(num_threads, sizeof(pthread_mutex_t));
	pthread_mutex_init(&(threadpool->queue_mutex), NULL);
	pthread_mutex_init(&(threadpool->terminate_mutex), NULL);

	// Spawn the threads.
	int i;
	struct worker_arg* arg;

	for (i = 0; i < num_threads; i++) {
		arg = malloc(sizeof(struct worker_arg));
		arg->id = i;
		arg->threadpool = threadpool;

		pthread_cond_init(&(threadpool->notifications[i]), NULL);
		pthread_mutex_init(&(threadpool->notify_mutexes[i]), NULL);
		pthread_mutex_init(&(threadpool->status_mutexes[i]), NULL);
		pthread_create(&(threadpool->threads[i]), NULL, worker, (void*) arg);
	}
}

void threadpool_destroy (threadpool_t* threadpool) {
	pthread_mutex_lock(&(threadpool->terminate_mutex));
	threadpool->terminate = 1;
	pthread_mutex_unlock(&(threadpool->terminate_mutex));

	// Wake up all the threads and wait for them to terminate.
	int i;
	for (i = 0; i < threadpool->num_threads; i++) {
		pthread_cond_signal(&(threadpool->notifications[i]));
		pthread_join(threadpool->threads[i], NULL);
	}

	free(threadpool->threads);
	free(threadpool->notifications);
	free(threadpool->notify_mutexes);
	free(threadpool->thread_status);
	free(threadpool->status_mutexes);
	queue_free(threadpool->queue);
}
