#ifndef __VECTOR_H
#define __VECTOR_H

#define VECTOR_INITIAL_SIZE 10
#define VECTOR_ERR_ALLOC "Unable to (re)allocate memory for vector."


// We define some macros here to make adding an element to a vector a little
// more convenient.
#define vector_int_add(vector, element) vector_add(vector, (void*) &(element), sizeof(int))
#define vector_charptr_add(vector, element) vector_add(vector, (void*) &(element), sizeof(char*))

typedef struct vector_t {
	int capacity;
	int length;
	void* data;
} vector_t;

void vector_init (vector_t* vector, size_t element_size);
void vector_free (vector_t* vector);
void vector_add (vector_t* vector, const void* element, size_t element_size);

#endif
