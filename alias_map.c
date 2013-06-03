#include <stdio.h>

#include "aliases_basic.h"
#include "alias_map.h"
#include "hash_map.h"

void map_aliases (hashmap_t* map) {
	hashmap_init(map, sizeof(alias), 64);

	alias func = alias_say;
	hashmap_put(map, "say", &func);

	func = alias_reply;
	hashmap_put(map, "reply", &func);
}
