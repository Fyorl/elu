#include <stdio.h>
#include "minunit.h"

#include "test_vector.c"
#include "test_string_utils.c"
#include "test_file_utils.c"
#include "test_config.c"
#include "test_queue.c"
#include "test_threadpool.c"
#include "test_hash_map.c"
#include "test_ticker.c"
#include "test_aliases.c"

int tests_run = 0;

static char* all_tests () {
	mu_run_test(test_vector);
	mu_run_test(test_string_utils);
	mu_run_test(test_file_utils);
	mu_run_test(test_config);
	mu_run_test(test_queue);
	mu_run_test(test_threadpool);
	mu_run_test(test_hash_map);
	mu_run_test(test_ticker);
	mu_run_test(test_aliases);
	return 0;
}

int main (int argc, char** argv) {
	char* result = all_tests();
	if (result != 0) {
		printf("FAIL\n%s\n", result);
	} else {
		printf("PASS\n");
	}

	printf("Ran %d tests.\n", tests_run);
	return (result != 0);
}
