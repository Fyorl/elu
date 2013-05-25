#include <string.h>

#include "vector.h"
#include "queue.h"

/*
 * Since we implement a queue as a vector, many of the methods here simply
 * call the corresponding vector function. Many implementations implement
 * queues as linked lists but array-backed queues are faster.
 */

void queue_init (queue_t* queue, size_t element_size) {
	vector_init(&(queue->vector), element_size);
	queue->garbage = 0;
}

void queue_free (queue_t* queue) {
	vector_free(&(queue->vector));
}

void queue_free_deep (queue_t* queue) {
	vector_free_deep(&(queue->vector));
}

void queue_add (queue_t* queue, const void* element) {
	vector_add(&(queue->vector), element);
}

void flush (queue_t* queue) {
	// How many items are still in the queue
	int queued = queue->vector.length - queue->garbage;

	if (queued > 0) {
		// If everything is garbage then we don't need to bother with this step
		// of copying the data.
		char* head = ((char*) queue->vector.data) + queue->garbage * queue->vector.element_size;
		memmove(queue->vector.data, head, queued * queue->vector.element_size);
	}

	queue->vector.length = queued;
	queue->garbage = 0;
}

void* queue_dequeue (queue_t* queue) {
	/*
	 * To dequeue, we first allocate some memory for the element and then copy
	 * it from the vector's data block. Then we increment the data pointer by
	 * one element's length to simulate removing the front of the queue.
	 *
	 * Since we always just incremement the pointer, we never get rid of the
	 * old data before the pointer which would be bad if the queue was added to
	 * a lot. So we set a threshold for the size of the vector. When it hits
	 * this length, we clear the old data and copy the remaining queue back to
	 * the head.
	 */

	size_t element_size = queue->vector.element_size;
	char* head = ((char*) queue->vector.data) + queue->garbage * element_size;
	void* alloc = malloc(element_size);
	memmove(alloc, head, element_size);

	queue->garbage++;

	if (queue->garbage >= QUEUE_CLEAR_THRESHOLD) {
		flush(queue);
	}

	return alloc;
}
