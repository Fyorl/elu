#include <string.h>

#include "minunit.h"
#include "../vector.c"

static char* test_vector_init () {
	vector_t vector;
	vector_init(&vector, sizeof(int));

	mu_assert("vector capacity != 10", vector.capacity == 10);
	mu_assert("vector length != 0", vector.length == 0);
	mu_assert("vector el size != sizeof(int)", vector.element_size == sizeof(int));

	vector_free(&vector);

	return 0;
}

static char* test_vector_add () {
	vector_t vector;
	vector_init(&vector, sizeof(int));

	int el_1 = 1;
	int el_2 = 400;
	vector_add(&vector, (void*) &el_1);
	vector_add(&vector, (void*) &el_2);

	mu_assert("vector[0] != 1", vector_get(vector, 0, int) == 1);
	mu_assert("vector[1] != 400", vector_get(vector, 1, int) == 400);

	vector_free(&vector);
	vector_init(&vector, sizeof(char*));

	char* str1 = "str1";
	char* str2 = "str2";
	vector_add(&vector, (void*) &str1);
	vector_add(&vector, (void*) &str2);

	mu_assert("vector[0] != 'str1'", strcmp("str1", vector_get(vector, 0, char*)) == 0);
	mu_assert("vector[1] != 'str2'", strcmp("str2", vector_get(vector, 1, char*)) == 0);

	vector_free(&vector);

	return 0;
}

static char* test_vector_resize () {
	vector_t vector;
	vector_init(&vector, sizeof(int));

	int i;
	for (i = 0; i < 20; i++) {
		vector_add(&vector, &i);
	}

	mu_assert("vector capacity != 40", vector.capacity == 40);
	mu_assert("vector length != 20", vector.length == 20);
	mu_assert("vector[0] != 0", vector_get(vector, 0, int) == 0);
	mu_assert("vector[19] != 19", vector_get(vector, 19, int) == 19);

	vector_free(&vector);

	return 0;
}

static char* test_vector () {
	mu_run_test(test_vector_init);
	mu_run_test(test_vector_add);
	mu_run_test(test_vector_resize);
	return 0;
}
