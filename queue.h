#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdlib.h>
#include "vector.h"

#define QUEUE_CLEAR_THRESHOLD 40

#define queue_peek(queue, type) *((type*)((queue).vector.data + (queue).garbage * (queue).vector.element_size))

typedef struct queue_t {
	vector_t vector;
	int garbage;
} queue_t;

void queue_init (queue_t* queue, size_t element_size);
void queue_free (queue_t* queue);
void queue_free_deep (queue_t* queue);
void queue_add (queue_t* queue, const void* element);
void* queue_dequeue (queue_t*);

#endif
