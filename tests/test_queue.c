#include "minunit.h"
#include "../queue.c"

static char* test_queue_init () {
	queue_t queue;
	queue_init(&queue, sizeof(int));

	mu_assert("queue vector not initialised", queue.vector.capacity == 10);
	mu_assert("garbage != 0", queue.garbage == 0);

	queue_free(&queue);

	return 0;
}

static char* test_queue_add () {
	queue_t queue;
	queue_init(&queue, sizeof(int));

	int element = 13;
	queue_add(&queue, &element);

	mu_assert("13 not added to queue correctly", vector_get(queue.vector, 0, int) == 13);

	queue_free(&queue);
	queue_init(&queue, sizeof(char*));

	char* str = "queue string";
	queue_add(&queue, &str);

	mu_assert("queue string != 'queue string'", strcmp(str, vector_get(queue.vector, 0, char*)) == 0);

	queue_free(&queue);

	return 0;
}

static char* test_queue_dequeue () {
	queue_t queue;
	queue_init(&queue, sizeof(char*));

	char* str1 = "str1";
	char* str2 = "str2";
	queue_add(&queue, &str1);
	queue_add(&queue, &str2);

	char** str_dq = queue_dequeue(&queue);
	mu_assert("str_dq != 'str1'", strcmp("str1", *str_dq) == 0);
	mu_assert("queue head != 'str2'", strcmp("str2", queue_peek(queue, char*)) == 0);

	free(str_dq);
	queue_free(&queue);

	return 0;
}

static char* test_queue_flush () {
	queue_t queue;
	queue_init(&queue, sizeof(int));

	int i;
	for (i = 0; i < 80; i++) {
		queue_add(&queue, &i);
	}

	int* dq;
	for (i = 0; i < 50; i++) {
		dq = queue_dequeue(&queue);
		free(dq);
	}

	mu_assert("queue vector length != 40", queue.vector.length == 40);
	mu_assert("queue vector capacity != 160", queue.vector.capacity == 160);
	mu_assert("queue garbage count != 10", queue.garbage == 10);
	mu_assert("queue head != 50", queue_peek(queue, int));

	queue_free(&queue);

	return 0;
}

static char* test_queue () {
	mu_run_test(test_queue_init);
	mu_run_test(test_queue_add);
	mu_run_test(test_queue_dequeue);
	mu_run_test(test_queue_flush);
	return 0;
}
