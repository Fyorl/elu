#include <time.h>
#include "timestamp.h"

long timestamp () {
	long ms;
	struct timespec spec;

	clock_gettime(CLOCK_REALTIME, &spec);
	ms = (long) spec.tv_sec * 1000;
	ms += spec.tv_nsec / 1.0e06;

	return ms;
}
