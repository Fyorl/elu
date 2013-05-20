#ifndef __VECTOR_H
#define __VECTOR_H

#define VECTOR_INITIAL_SIZE 10
#define VECTOR_ERR_ALLOC "Unable to (re)allocate memory for vector."

typedef struct vector_t {
	int capacity;
	int length;
	size_t element_size;
	void* data;
} vector_t;

void vector_init (vector_t* vector, size_t element_size);
void vector_free (vector_t* vector);
void vector_add (vector_t* vector, const void* element);

#endif
