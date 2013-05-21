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
	return 0;
}

static char* test_string_utils () {
	mu_run_test(test_string_split_cb);
	mu_run_test(test_string_split);
	return 0;
}
