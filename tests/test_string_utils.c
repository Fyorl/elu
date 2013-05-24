#include <string.h>

#include "minunit.h"
#include "../string_utils.c"

char tsu__str1[4];
char tsu__str2[3];

void tsu__callback (const char* string, void* data) {
	static int call_count = 0;
	if (call_count == 0) {
		strcpy(tsu__str1, string);
	} else if (call_count == 1) {
		strcpy(tsu__str2, string);
	}

	call_count++;
}

static char* test_string_split_cb () {
	string_split_cb(&tsu__callback, NULL, "abcdef", 'd');
	mu_assert("split[0] != 'abc'", strcmp("abc", tsu__str1) == 0);
	mu_assert("split[1] != 'ef'", strcmp("ef", tsu__str2) == 0);

	return 0;
}

static char* test_string_split () {
	vector_t vector;

	// Test simple split.
	vector_init(&vector, sizeof(char*));
	string_split(&vector, "abcdef", 'd');

	mu_assert("vector[0] != 'abc'", strcmp("abc", vector_get(vector, 0, char*)) == 0);
	mu_assert("vector[1] != 'ef'", strcmp("ef", vector_get(vector, 1, char*)) == 0);

	vector_free_deep(&vector);

	// Test string containing no delimiter.
	vector_init(&vector, sizeof(char*));
	string_split(&vector, "abcdef", 'g');

	mu_assert("vector[0] != 'abcdef'", strcmp("abcdef", vector_get(vector, 0, char*)) == 0);

	vector_free_deep(&vector);

	// Test string containing delimiters in various places.
	vector_init(&vector, sizeof(char*));

	string_split(&vector, "aaaa", 'a');
	mu_assert("vector len != 0", vector.length == 0);

	string_split(&vector, "abba", 'b');
	string_split(&vector, "abba", 'a');

	mu_assert("vector[0] != 'a'", strcmp("a", vector_get(vector, 0, char*)) == 0);
	mu_assert("vector[1] != 'a'", strcmp("a", vector_get(vector, 1, char*)) == 0);
	mu_assert("vector[2] != 'bb'", strcmp("bb", vector_get(vector, 2, char*)) == 0);
	mu_assert("vector len != 3", vector.length == 3);

	vector_free_deep(&vector);

	return 0;
}

static char* test_string_utils () {
	mu_run_test(test_string_split_cb);
	mu_run_test(test_string_split);
	return 0;
}