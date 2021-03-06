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
	string_split_cb(&tsu__callback, NULL, "abcdef", "d");
	mu_assert("split[0] != 'abc'", strcmp("abc", tsu__str1) == 0);
	mu_assert("split[1] != 'ef'", strcmp("ef", tsu__str2) == 0);

	return 0;
}

static char* test_string_split () {
	vector_t vector;

	// Test simple split.
	vector_init(&vector, sizeof(char*));
	string_split(&vector, "abcdef", "d");

	mu_assert(
		"vector[0] != 'abc'"
		, strcmp("abc", vector_get(vector, 0, char*)) == 0);
	mu_assert(
		"vector[1] != 'ef'"
		, strcmp("ef", vector_get(vector, 1, char*)) == 0);

	vector_free_deep(&vector);

	// Test string containing no delimiter.
	vector_init(&vector, sizeof(char*));
	string_split(&vector, "abcdef", "g");

	mu_assert(
		"vector[0] != 'abcdef'"
		, strcmp("abcdef", vector_get(vector, 0, char*)) == 0);

	vector_free_deep(&vector);

	// Test string containing delimiters in various places.
	vector_init(&vector, sizeof(char*));

	string_split(&vector, "aaaa", "a");
	mu_assert("vector len != 0", vector.length == 0);

	string_split(&vector, "abba", "b");
	string_split(&vector, "abba", "a");

	mu_assert(
		"vector[0] != 'a'"
		, strcmp("a", vector_get(vector, 0, char*)) == 0);
	mu_assert(
		"vector[1] != 'a'"
		, strcmp("a", vector_get(vector, 1, char*)) == 0);
	mu_assert(
		"vector[2] != 'bb'"
		, strcmp("bb", vector_get(vector, 2, char*)) == 0);
	mu_assert("vector len != 3", vector.length == 3);

	vector_free_deep(&vector);

	return 0;
}

static char* test_strpos () {
	char* string = "abcdef";

	mu_assert("cannot find substring at start", strpos(string, "ab") == 0);
	mu_assert("cannot find substring in middle", strpos(string, "cde") == 2);
	mu_assert("cannot find substring at end", strpos(string, "f") == 5);
	mu_assert("no fail on substring > string", strpos(string, "abcdefg") == -1);
	mu_assert(
		"cannot find substring if length == string"
		, strpos(string, string) == 0);

	return 0;
}

static char* test_string_chomp () {
	char* string = "0 1 2";
	
	const char* chomp1 = string_chomp(string, " ");
	mu_assert("chomp1 != 1 2", strcmp(chomp1, "1 2") == 0);
	
	const char* chomp2 = string_chomp(chomp1, " ");
	mu_assert("chomp2 != 2", strcmp(chomp2, "2") == 0);
	
	const char* chomp3 = string_chomp(chomp2, " ");
	mu_assert("chomp3 != \\0", chomp3[0] == '\0');
	
	const char* chomp4 = string_chomp(chomp3, " ");
	mu_assert("chomp4 != chomp3", chomp3 == chomp4);
	
	return 0;
}

static char* test_string_join () {
	char** pieces = calloc(4, sizeof(char*));
	char* piece1 = "1";
	char* piece2 = "2";
	char* piece3 = "3";
	
	pieces[0] = piece1;
	pieces[2] = piece2;
	pieces[3] = piece3;
	
	char result[640];
	string_join(pieces, 4, ", ", result, 640);
	mu_assert("String join != 1, 2, 3", strcmp(result, "1, 2, 3") == 0);
	
	memset(result, 0, 640 * sizeof(char));
	string_join(pieces, 4, ", ", result, 2);
	mu_assert("String join didn't truncate", strcmp(result, "1") == 0);
	
	memset(result, 0, 640 * sizeof(char));
	pieces[2] = NULL;
	pieces[3] = NULL;
	string_join(pieces, 4, ", ", result, 640);
	mu_assert("String join of one element failed", strcmp(result, "1") == 0);
	
	free(pieces);

	return 0;
}

static char* test_string_join_english () {
	char** pieces = calloc(4, sizeof(char*));
	char* piece1 = "1";
	char* piece2 = "2";
	char* piece3 = "3";
	
	pieces[0] = piece1;
	pieces[1] = piece2;
	pieces[3] = piece3;
	
	char result[640];
	string_join_english(pieces, 4, result, 640);
	
	mu_assert(
		"String join english != 1, 2 and 3"
		, strcmp(result, "1, 2 and 3") == 0);
	free(pieces);

	return 0;
}

static char* test_string_utils () {
	mu_run_test(test_string_split_cb);
	mu_run_test(test_string_split);
	mu_run_test(test_strpos);
	mu_run_test(test_string_chomp);
	mu_run_test(test_string_join);
	mu_run_test(test_string_join_english);
	return 0;
}
