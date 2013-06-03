#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>
#include "hash_map.h"
#include "vector.h"

typedef struct config_t {
	char* host;
	char* nick;
	char* pass;
	char* command_char;
	vector_t channels;
	hashmap_t admins;
	uint16_t port;
} config_t;

void read_config (config_t* config, const char* filename);
void config_destroy (config_t* config);

#endif
