#ifndef __ALIAS_MAP_H
#define __ALIAS_MAP_H

#include "hash_map.h"

typedef char* (*alias)(const char*, const char*, const char*);
hashmap_t alias_map;
void map_aliases (hashmap_t* map);

#endif
