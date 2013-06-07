#ifndef __HASH_MAP_H
#define __HASH_MAP_H

#include <stdlib.h>

#include "vector.h"

#define hashmap_value(hashmap, key, type) *((type*)hashmap_get(hashmap, key))

typedef struct hashmap_t {
	int capacity;
	size_t element_size;
	vector_t** data;
	vector_t keys;
} hashmap_t;

void hashmap_init (hashmap_t* hashmap, size_t element_size, int capacity);
void hashmap_put (hashmap_t* hashmap, const char* key, void* value);
void* hashmap_get (hashmap_t* hashmap, const char* key);
void hashmap_destroy (hashmap_t* hashmap);
void hashmap_destroy_deep (hashmap_t* hashmap);

#endif
