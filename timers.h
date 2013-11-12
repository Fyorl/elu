#ifndef __TIMERS_H
#define __TIMER_H

#include <stdint.h>

void check_timers ();
void timer_add (
	const char* nick
	, const char* channel
	, int64_t timestamp
	, const char* cmd);

#endif
