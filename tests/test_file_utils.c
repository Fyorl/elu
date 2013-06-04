#include "minunit.h"
#include "../file_utils.c"

char tfu__str1[19];
char tfu__str2[22];
vector_t tfu__vec;

void tfu__callback (const char* line, void* data) {
	static int count = 0;

	if (count == 0) {
		memmove(tfu__str1, line, 19 * sizeof(char));
	} else if (count == 4) {
		memmove(tfu__str2, line, 22 * sizeof(char));
	}

	count++;
}

void tfu__multi_callback (const char* line, void* data) {
	char* alloc = calloc(strlen(line) + 1, sizeof(char));
	strcpy(alloc, line);
	vector_add(&tfu__vec, &alloc);
}

static char* test_file_get_lines () {
	file_get_lines("../example.conf", &tfu__callback);

	mu_assert("1st line != 'host=irc.rizon.net'", strcmp("host=irc.rizon.net", tfu__str1) == 0);
	mu_assert("4th line != 'command_char=`'", strcmp("command_char=`", tfu__str2) == 0);

	return 0;
}

static char* test_multi_chunk_file () {
	vector_init(&tfu__vec, sizeof(char*));
	file_get_lines("multi_chunk_file", &tfu__multi_callback);

	mu_assert("multichunk vec len != 2048", tfu__vec.length == 2048);
	mu_assert("multichunk vec[0] != '0'", *(vector_get(tfu__vec, 0, char*)) == '0');
	mu_assert("multichunk vec[2047] != '2047'", strcmp("2047", vector_get(tfu__vec, 2047, char*)) == 0);

	vector_free_deep(&tfu__vec);
	return 0;
}

static char* test_file_get_contents () {
	char* contents = file_get_contents("multi_chunk_file");

	mu_assert("file_get_contents file beginning corrupt", contents[0] == '0');
	mu_assert("file_get_contents file end corrupt", strcmp(contents + 9125, "2047") == 0);

	return 0;
}

static char* test_file_utils () {
	mu_run_test(test_file_get_lines);
	mu_run_test(test_multi_chunk_file);
	mu_run_test(test_file_get_contents);

	return 0;
}
