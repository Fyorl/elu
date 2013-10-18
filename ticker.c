#include "ticker.h"

void ticker_init (ticker_t* ticker, queue_t* queue) {
	ticker->queue = queue;
	ticker->terminate = false;
	pthread_mutex_init(&(ticker->queue_mutex), NULL);
	pthread_mutex_init(&(ticker->terminate_mutex), NULL);
	pthread_create(&(ticker->thread), NULL, tick, (void*) ticker);
}

void ticker_destroy (ticker_t* ticker) {
	pthread_mutex_lock(&(ticker->terminate_mutex));
	ticker->terminate = true;
	pthread_mutex_unlock(&(ticker->terminate_mutex));
}
