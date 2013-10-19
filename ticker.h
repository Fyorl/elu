#ifndef __TICKER_H
#define __TICKER_H

#include <pthread.h>
#include <stdbool.h>
#include "vector.h"

// The ticker is a thread that runs in the background and processes whatever
// tasks are registered in the queue every second.
typedef struct ticker_t {
	pthread_t thread;
	pthread_mutex_t handlers_mutex;
	vector_t* handlers;
	bool terminate;
	pthread_mutex_t terminate_mutex;
} ticker_t;

typedef void (*ticker_handler)();

void ticker_init (ticker_t* ticker, vector_t* handlers);
void ticker_destroy (ticker_t* ticker);
void ticker_register_handler(ticker_t* ticker, ticker_handler* handler);

#endif
