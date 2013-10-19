#include <stdio.h>
#include "minunit.h"
#include "../ticker.c"

static int tti__num_ticks = 0;
void tti__tick () {
	tti__num_ticks++;
}

static char* test_ticker_tick () {
	vector_t handlers;
	vector_init(&handlers, sizeof(ticker_handler));

	ticker_t ticker;
	ticker_init(&ticker, &handlers);
	ticker_register_handler(&ticker, tti__tick);

	struct timespec spec;
	spec.tv_sec = 2;
	spec.tv_nsec = 0;

	nanosleep(&spec, NULL);

	ticker_destroy(&ticker);

	mu_assert("tick was not called twice", tti__num_ticks == 2);

	return 0;
}

static char* test_ticker () {
	mu_run_test(test_ticker_tick);

	return 0;
}
