#include "minunit.h"
#include "../file_utils.c"

char tfu__str1[19];
char tfu__str2[22];

void tfu__callback (const char* line, void* data) {
	static int count = 0;

	if (count == 0) {
		memmove(tfu__str1, line, 19 * sizeof(char));
	} else if (count == 4) {
		memmove(tfu__str2, line, 22 * sizeof(char));
	}

	count++;
}

static char* test_file_get_lines () {
	file_get_lines("../example.conf", &tfu__callback);

	mu_assert("1st line != 'host=irc.rizon.net'", strcmp("host=irc.rizon.net", tfu__str1) == 0);
	mu_assert("4th line != 'channels=#impsvillage'", strcmp("channels=#impsvillage", tfu__str2) == 0);

	return 0;
}

static char* test_file_utils () {
	mu_run_test(test_file_get_lines);
	return 0;
}
