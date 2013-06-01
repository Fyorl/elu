#include "minunit.h"
#include "../threadpool.c"

pthread_mutex_t tt__count_mutex = PTHREAD_MUTEX_INITIALIZER;
int tt__worker_count = 0;
int tt__work_in_order = 1;

void tt__dummy (void* data) {}

void tt__worker (void* data) {
	char* string = data;
	if (strcmp("thread", string) == 0) {
		pthread_mutex_lock(&tt__count_mutex);
		tt__worker_count++;
		pthread_mutex_unlock(&tt__count_mutex);
	}

	free(data);
}

void tt__ordered_worker (void* data) {
	static int previous = -1;
	int work = *((int*) data);

	tt__work_in_order = tt__work_in_order && (work == previous + 1);

	free(data);
}

static char* test_threadpool_destroy () {
	queue_t queue;
	queue_init(&queue, sizeof(char*), 1);

	threadpool_t threadpool;
	threadpool_init(&threadpool, 8, &queue, &tt__dummy);
	threadpool_destroy(&threadpool);

	mu_assert("threads did not terminate", threadpool.terminate == 1);

	return 0;
}

static char* test_threadpool_add_work () {
	queue_t queue;
	queue_init(&queue, sizeof(char*), 1);

	threadpool_t threadpool;
	threadpool_init(&threadpool, 8, &queue, &tt__worker);

	char* work;
	int i;
	for (i = 0; i < 10; i++) {
		work = calloc(7, sizeof(char));
		strcpy(work, "thread");
		threadpool_add_work(&threadpool, &work);
	}

	threadpool_destroy(&threadpool);

	mu_assert("all work was not done", tt__worker_count == 10);

	return 0;
}

static char* test_threadpool_single () {
	queue_t queue;
	queue_init(&queue, sizeof(int), 0);

	threadpool_t threadpool;
	threadpool_init(&threadpool, 1, &queue, &tt__ordered_worker);

	int i;
	for (i = 0; i < 10; i++) {
		threadpool_add_work(&threadpool, &i);
	}

	mu_assert("the work was not received in order", tt__work_in_order);

	return 0;
}

static char* test_threadpool () {
	mu_run_test(test_threadpool_destroy);
	mu_run_test(test_threadpool_add_work);
	mu_run_test(test_threadpool_single);
	return 0;
}
