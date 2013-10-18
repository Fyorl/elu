#ifndef __ELU_H
#define __ELU_H

#include "config.h"
#include "vendor/sqlite3.h"

#define THREADS 4

// Global socket object so we can write to it from anywhere.
int sock;
config_t* config;
sqlite3* db;

#endif
