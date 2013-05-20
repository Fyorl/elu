#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

/*
 * When adding an element to a vector, the element to be added must first be a
 * pointer but also cast to void*.
 */
void vector_add (vector_t* vector, const void* element) {
	// Cast to a char pointer because pointer arithmetic on void pointers is
	// technically illegal.
	char* mem_address = (char*) vector->data;

	// Increment the address by a number of bytes equal to the next index in
	// the array multiplied by the number of bytes of the type.
	mem_address += vector->length * vector->element_size;

	// Copy the new element to that address.
	memmove(mem_address, element, vector->element_size);
	vector->length++;

	if (vector->length >= vector->capacity) {
		// If we're running out of space in our array, resize it.
		vector->capacity *= 2;
		void* alloc = realloc(vector->data, vector->capacity * vector->element_size);

		if (alloc == NULL) {
			fprintf(stderr, VECTOR_ERR_ALLOC);
		}
	}
}

/*
 * Vector implementation uses void pointers to enable it to store any kind
 * of value. When we initialise the vector, the user passes a pointer to a
 * vector struct that they want to initialise as well as the size (in bytes)
 * of the type of element they want to store in the vector.
 */
void vector_init (vector_t* vector, size_t element_size) {
	vector->data = (void*) calloc(VECTOR_INITIAL_SIZE, element_size);
	vector->element_size = element_size;
	vector->capacity = VECTOR_INITIAL_SIZE;
	vector->length = 0;

	if (vector->data == NULL) {
		fprintf(stderr, VECTOR_ERR_ALLOC);
	}
}

void vector_free (vector_t* vector) {
	free(vector->data);
}