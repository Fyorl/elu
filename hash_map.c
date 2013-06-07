#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "vendor/city.h"
#include "hash_map.h"
#include "vector.h"

struct node_t {
	char* key;
	void* value;
} node_t;

void hashmap_init (hashmap_t* hashmap, size_t element_size, int capacity) {
	hashmap->element_size = element_size;
	hashmap->capacity = capacity;
	hashmap->data = calloc(capacity, sizeof(vector_t**));
	vector_init(&(hashmap->keys), sizeof(char*));

	if (hashmap->data == NULL) {
		fprintf(stderr, "Unable to allocate memory for hashmap.\n");
	}
}

void* hashmap_get (hashmap_t* hashmap, const char* key) {
	int index = CityHash64(key, strlen(key)) % hashmap->capacity;
	vector_t* vector = hashmap->data[index];

	if (vector == NULL) {
		return NULL;
	}

	struct node_t node;
	int i;
	for (i = 0; i < vector->length; i++) {
		node = vector_get(*vector, i, struct node_t);
		if (strcmp(node.key, key) == 0) {
			return node.value;
		}
	}

	return NULL;
}

void hashmap_put (hashmap_t* hashmap, const char* key, void* value) {
	int keylen = strlen(key);
	int index = CityHash64(key, keylen) % hashmap->capacity;
	vector_t* vector = hashmap->data[index];

	struct node_t node;
	char* node_key;
	void* node_value;

	if (vector == NULL) {
		vector = malloc(sizeof(vector_t));
		vector_init(vector, sizeof(struct node_t));
		hashmap->data[index] = vector;
	}

	node_key = calloc(keylen + 1, sizeof(char));
	node_value = malloc(hashmap->element_size);

	memmove(node_key, key, (keylen + 1) * sizeof(char));
	memmove(node_value, value, hashmap->element_size);

	node.key = node_key;
	node.value = node_value;

	int i;
	struct node_t tmp_node;
	if (hashmap_get(hashmap, key) != NULL) {
		for (i = 0; i < vector->length; i++) {
			tmp_node = vector_get(*vector, i, struct node_t);
			if (strcmp(tmp_node.key, key) == 0) {
				free(tmp_node.key);
				free(tmp_node.value);
				memmove(((struct node_t*) vector->data) + i, &node, sizeof(struct node_t));
				break;
			}
		}
	} else {
		vector_add(vector, &node);
	}

	vector_add(&(hashmap->keys), &node_key);
}

void hashmap_destroy (hashmap_t* hashmap) {
	int i;
	int j;
	vector_t* vector;
	struct node_t node;

	for (i = 0; i < hashmap->capacity; i++) {
		if (hashmap->data[i] != NULL) {
			vector = hashmap->data[i];
			for (j = 0; j < vector->length; j++) {
				node = vector_get(*vector, j, struct node_t);
				free(node.key);
				free(node.value);
			}

			vector_free(vector);
			free(vector);
		}
	}

	free(hashmap->data);
	vector_free(&(hashmap->keys));
}

void hashmap_destroy_deep (hashmap_t* hashmap) {
	int i;
	int j;
	vector_t* vector;
	struct node_t node;

	for (i = 0; i < hashmap->capacity; i++) {
		if (hashmap->data[i] != NULL) {
			vector = hashmap->data[i];
			for (j = 0; j < vector->length; j++) {
				node = vector_get(*vector, j, struct node_t);
				free(node.key);
				free(*((char**) node.value));
				free(node.value);
			}

			vector_free(vector);
			free(vector);
		}
	}

	free(hashmap->data);
	vector_free(&(hashmap->keys));
}
