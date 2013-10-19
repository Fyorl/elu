#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "ticker.h"
#include "timestamp.h"
#include "vector.h"

void* tick (void* data) {
	ticker_t* ticker = data;
	struct timespec spec;
	int64_t start;
	int64_t end;
	int64_t wait;

	while (true) {
		start = timestamp() * 1000000;

		pthread_mutex_lock(&(ticker->terminate_mutex));
		if (ticker->terminate) {
			pthread_mutex_unlock(&(ticker->terminate_mutex));
			break;
		}
		pthread_mutex_unlock(&(ticker->terminate_mutex));

		pthread_mutex_lock(&(ticker->handlers_mutex));
		int length = ticker->handlers->length;
		pthread_mutex_unlock(&(ticker->handlers_mutex));

		int i;
		for (i = 0; i < length; i++) {
			pthread_mutex_lock(&(ticker->handlers_mutex));
			ticker_handler handler = vector_get(*(ticker->handlers), i, ticker_handler);
			pthread_mutex_unlock(&(ticker->handlers_mutex));
			(*handler)();
		}

		end = timestamp() * 1000000;
		wait = 1000000000LL - (end - start);

		if (wait > 999999999LL) {
			// This should never happen as it means our loop somehow took less
			// than 1ns to execute.
			wait = 999999999LL;
		} else if (wait < 0) {
			wait = 0;
		}

		spec.tv_sec = 0;
		spec.tv_nsec = wait;
		nanosleep(&spec, NULL);
	}

	return NULL;
}

void ticker_register_handler (ticker_t* ticker, ticker_handler handler) {
	pthread_mutex_lock(&(ticker->handlers_mutex));
	vector_add(ticker->handlers, &handler);
	pthread_mutex_unlock(&(ticker->handlers_mutex));
}

void ticker_init (ticker_t* ticker, vector_t* handlers) {
	ticker->handlers = handlers;
	ticker->terminate = false;
	pthread_mutex_init(&(ticker->handlers_mutex), NULL);
	pthread_mutex_init(&(ticker->terminate_mutex), NULL);
	pthread_create(&(ticker->thread), NULL, tick, (void*) ticker);
}

void ticker_destroy (ticker_t* ticker) {
	pthread_mutex_lock(&(ticker->terminate_mutex));
	ticker->terminate = true;
	pthread_mutex_unlock(&(ticker->terminate_mutex));
	pthread_join(ticker->thread, NULL);
	vector_free(ticker->handlers);
}
