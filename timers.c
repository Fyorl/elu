#include <inttypes.h>
#include <stdint.h>

#include "elu.h"
#include "irc.h"
#include "timers.h"
#include "vendor/sqlite3.h"

extern sqlite3* db;

void check_timers () {
	
}

void timer_add (
		const char* nick
		, const char* channel
		, int64_t timestamp
		, const char* cmd) {

	char* querystr = \
		"INSERT INTO `timers` " \
		"(`nick`, `channel`, `timestamp`, `cmd`) " \
		"VALUES ('%q', '%q', '%" PRId64 "', '%q');";

	char query_buf[512];
	char* query = sqlite3_snprintf(
		512
		, query_buf
		, querystr
		, nick
		, channel
		, timestamp
		, cmd);

	sqlite3_exec(db, query, NULL, NULL, NULL);
}
