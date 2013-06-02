#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "string_utils.h"

void irc_register () {

}

void filter (char* string) {
	// Try to determine if the string is interesting or not and, if so, pass it
	// on to the appropriate function.
	vector_t colon_split;
	vector_init(&colon_split, sizeof(char*));
	string_split(&colon_split, string, ':');

	if (strpos(vector_get(colon_split, 0, char*), "439 *") > -1) {
		irc_register();
	}

	vector_free_deep(&colon_split);
}

void irc_handle_chunk (void* chunk) {
	// Since we sometimes receive multiple lines at once, we have to split on
	// those lines before sending it to the filter function.
	vector_t lines;
	vector_init(&lines, sizeof(char*));
	string_split(&lines, chunk, '\n');

	int i;
	for (i = 0; i < lines.length; i++) {
		filter(vector_get(lines, i, char*));
	}

	vector_free_deep(&lines);
	free(chunk);
}
