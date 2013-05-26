#include "minunit.h"
#include "../threadpool.c"

void tt__dummy (void* data) {}

static char* test_threadpool_destroy () {
	queue_t queue;
	queue_init(&queue, sizeof(char*), 1);

	threadpool_t threadpool;
	threadpool_init(&threadpool, 8, &queue, &tt__dummy);
	threadpool_destroy(&threadpool);

	mu_assert("threads did not terminate", threadpool.terminate == 1);

	return 0;
}

static char* test_threadpool () {
	mu_run_test(test_threadpool_destroy);
	return 0;
}
