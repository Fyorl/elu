#ifndef __ALIAS_MAP_H
#define __ALIAS_MAP_H

#include "hash_map.h"

typedef struct alias_arg {
	const char* nick;
	const char* channel;
	const char* msg;
	long timestamp;
} alias_arg;

typedef char* (*alias)(const alias_arg*);
hashmap_t alias_map;
void map_aliases (hashmap_t* map);

#endif
