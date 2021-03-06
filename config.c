#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "file_utils.h"
#include "hash_map.h"
#include "vector.h"

// We declare a global config here so that the callback can access it.
config_t* global_config;

void parse_lines (const char* line, void* data) {
	vector_t admin_split;
	char* store_admin;
	int i;

	vector_t vector;
	vector_init(&vector, sizeof(char*));
	string_split(&vector, line, "=");

	// We split each line of the config file by the '='. The first element is
	// the key and the second is the value.
	char* key = vector_get(vector, 0, char*);
	char* value = vector_get(vector, 1, char*);
	char** field = NULL;

	// Check for those keys which we only need to store directly as strings.
	if (strcmp("host", key) == 0) {
		field = &(global_config->host);
	} else if (strcmp("nick", key) == 0) {
		field = &(global_config->nick);
	} else if (strcmp("pass", key) == 0) {
		field = &(global_config->pass);
	} else if (strcmp("command_char", key) == 0) {
		field = &(global_config->command_char);
	}

	// If field is null then our key is not one we can just store as a straight
	// string and must instead do some conversion on it.
	if (field == NULL) {
		if (strcmp("port", key) == 0) {
			// The port must be converted into a 16-bit int.
			global_config->port = (uint16_t) strtol(value, NULL, 10);
		} else if (strcmp("channels", key) == 0) {
			// The channels we can just store as a vector by splitting on the
			// comma.
			vector_init(&(global_config->channels), sizeof(char*));
			string_split(&(global_config->channels), value, ",");
		} else if (strcmp("admins", key) == 0) {
			hashmap_init(&(global_config->admins), sizeof(int), 64);
			vector_init(&admin_split, sizeof(char*));
			string_split(&admin_split, value, ",");

			for (i = 0; i < admin_split.length; i++) {
				store_admin = calloc(strlen(vector_get(admin_split, i, char*)) + 1, sizeof(char));
				strcpy(store_admin, vector_get(admin_split, i, char*));
				hashmap_put(&(global_config->admins), store_admin, &i);
				free(store_admin);
			}

			vector_free_deep(&admin_split);
		}
	} else {
		// If we can just store it as a string then allocate some permanent
		// memory and do just that.
		*field = calloc(strlen(value) + 1, sizeof(char));
		strcpy(*field, value);
	}

	vector_free_deep(&vector);
}

/*
 * Reads the config file in and attempts to parse useful fields.
 */
void read_config (config_t* config, const char* filename) {
	global_config = config;
	file_get_lines(filename, &parse_lines);
}

void config_destroy (config_t* config) {
	free(config->host);
	free(config->nick);
	free(config->pass);
	free(config->command_char);
	hashmap_destroy(&(config->admins));
	vector_free_deep(&(config->channels));
}
