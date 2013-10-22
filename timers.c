#include <pthread.h>

#include "elu.h"
#include "irc.h"
#include "timers.h"
#include "vendor/sqlite3.h"

extern sqlite3* db;
extern pthread_mutex_t db_mutex;

void check_timers () {
	
}
