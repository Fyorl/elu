#ifndef __ALIAS_MAP_H
#define __ALIAS_MAP_H

#include "hash_map.h"

typedef struct alias_arg {
	char* nick;
	char* channel;
	char* msg;
	long timestamp;
} alias_arg;

typedef char* (*alias)(const alias_arg*);
hashmap_t alias_map;
void map_aliases (hashmap_t* map);

#endif
