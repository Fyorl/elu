#include <stdint.h>
#include <time.h>
#include "timestamp.h"

int64_t timestamp () {
	int64_t ms;
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);
	ms = (int64_t) spec.tv_sec * 1000;
	ms += spec.tv_nsec / 1.0e06;

	return ms;
}
