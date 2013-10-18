#include <pthread.t>
#include <stdbool.h>
#include "queue.h"

// The ticker is a thread that runs in the background and processes whatever
// tasks are registered in the queue every second.
typedef struct ticker_t {
	pthread_t thread;
	pthread_mutex_t queue_mutex;
	queue_t* queue;
	bool terminate;
	pthread_mutex_t terminate_mutex;
} ticker_t;

void ticker_init (ticker_t* ticker, queue_t* queue);
void ticker_destroy (ticker_t* ticker);
