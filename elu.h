#ifndef __ELU_H
#define __ELU_H

#include <pthread.h>

#include "alias_map.h"
#include "config.h"
#include "threadpool.h"
#include "ticker.h"
#include "vendor/sqlite3.h"

// The number of threads we want to create in a pool to handle IRC commands.
#define EXEC_THREADS 4
#define RECEIVE_BUFFER_SIZE 2048

struct alias_runner_arg {
	alias func;
	alias_arg* args;
};

// Global socket object so we can write to it from anywhere.
int sock;
config_t* config;
ticker_t ticker;
threadpool_t executor;

sqlite3* db;
pthread_mutex_t db_mutex;

#endif
