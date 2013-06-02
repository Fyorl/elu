#include <stdio.h>
#include "minunit.h"
#include "../hash_map.c"

static char* test_hashmap_init () {
	hashmap_t hashmap;
	hashmap_init(&hashmap, sizeof(int), 64);

	mu_assert("hashmap element size incorrect", hashmap.element_size == sizeof(int));
	mu_assert("hashmap capacity incorrect", hashmap.capacity == 64);
	mu_assert("hashmap data not allocated", hashmap.data != NULL);

	hashmap_destroy(&hashmap);

	return 0;
}

static char* test_hashmap_get () {
	// Test replacement.
	hashmap_t hashmap;
	hashmap_init(&hashmap, sizeof(int), 64);

	int value = 42;
	hashmap_put(&hashmap, "a", &value);
	mu_assert("hashmap['a'] != 42", hashmap_value(&hashmap, "a", int));

	value = 13001;
	hashmap_put(&hashmap, "a", &value);
	mu_assert("hashmap value not replaced", hashmap_value(&hashmap, "a", int));

	hashmap_destroy(&hashmap);

	// Test collisions.
	hashmap_init(&hashmap, sizeof(int), 1);

	value = 52;
	hashmap_put(&hashmap, "a", &value);
	mu_assert("hashmap['a'] != 52", hashmap_value(&hashmap, "a", int));

	value = 64;
	hashmap_put(&hashmap, "b", &value);
	mu_assert("hashmap['b'] != 64", hashmap_value(&hashmap, "b", int));

	value = 92;
	hashmap_put(&hashmap, "b", &value);
	mu_assert("hashmap[b] != 92", hashmap_value(&hashmap, "b", int));

	hashmap_destroy(&hashmap);

	// Test string storage.
	hashmap_init(&hashmap, sizeof(char*), 64);

	char* str = calloc(9, sizeof(char));
	strcpy(str, "test str");
	hashmap_put(&hashmap, "a", &str);
	mu_assert("hashmap['a'] != 'test str'", strcmp(str, hashmap_value(&hashmap, "a", char*)) == 0);

	char* str2 = calloc(10, sizeof(char));
	strcpy(str2, "test str2");
	hashmap_put(&hashmap, "b", &str2);
	mu_assert("hashmap['b'] != 'test str2'", strcmp(str2, hashmap_value(&hashmap, "b", char*)) == 0);

	hashmap_destroy_deep(&hashmap);

	return 0;
}

static char* test_hash_map () {
	mu_run_test(test_hashmap_init);
	mu_run_test(test_hashmap_get);

	return 0;
}
