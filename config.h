#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>
#include "vector.h"

typedef struct config_t {
	char* host;
	char* nick;
	char* pass;
	vector_t channels;
	uint16_t port;
} config_t;

void read_config (config_t* config, const char* filename);

#endif
