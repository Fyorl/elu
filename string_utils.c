#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"
#include "vector.h"

void strsplit_construct_vector (const char* string, void* vector) {
	size_t size = strlen(string) + 1;
	char* alloc = calloc(size, sizeof(char));
	strncpy(alloc, string, size);

	vector_add((vector_t*) vector, &alloc);
}

/*
 * Split the given string into multiple parts based on a given delimiter. Each
 * part of the string is passed to a callback function.
 */
void string_split_cb (strsplit_cb callback, void* cb_data, const char* string, char delimiter) {
	char buffer[STRSPLIT_MAX_ELEM_LEN];
	char c;
	int i = 0;
	int n = 0;

	// Loop over every character in the string.
	do {
		c = string[i];
		if (c == delimiter || c == '\0') {
			// If we found the delimiter character or it is the end of the
			// string then we take whatever's in the buffer and make it a
			// string before sending it to the callback.

			if (n > 0) {
				buffer[n] = '\0';
				callback(buffer, cb_data);

				// Reset the buffer ready for the next chunk of string.
				n = 0;
				memset(buffer, 0, STRSPLIT_MAX_ELEM_LEN * sizeof(char));
			}
		} else {
			// If we don't find the delimiter character, just add the current
			// character to the buffer.
			buffer[n] = c;
			n++;

			if (n >= STRSPLIT_MAX_ELEM_LEN - 1) {
				fprintf(stderr, "Substring element too long.\n");
				break;
			}
		}

		i++;
	} while (c != '\0');
}

/*
 * Split a string and place the results in a vector. Just calls
 * string_split_cb with a specific callback.
 */
void string_split (vector_t* vector, const char* string, char delimiter) {
	string_split_cb(&strsplit_construct_vector, vector, string, delimiter);
}
