#include <stdio.h>
#include <string.h>

#include "string_utils.h"
#include "vector.h"

vector_t* strsplit_vector_ptr;

void strsplit_construct_vector (const char* string) {
	vector_charptr_add(strsplit_vector_ptr, string);
}

/*
 * Split the given string into multiple parts based on a given delimiter. Each
 * part of the string is passed to a callback function.
 */
void string_split_cb (void (*callback)(const char*), const char* string, char delimiter) {
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
			buffer[n] = '\0';
			callback(buffer);

			// Reset the buffer ready for the next chunk of string.
			n = 0;
			memset(buffer, 0, STRSPLIT_MAX_ELEM_LEN * sizeof(char));
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
 * In order to avoid code duplication, we want to reuse the logic in
 * string_split_cb to store each element of the split in a vector. In order to
 * do that, all we need to do is call string_split_cb with a callback that will
 * place each item that string_split_cb sends it into a vector. The problem is
 * that the callback function provided to string_split_cb can only accept one
 * string (const char*) as its argument which means we cannot send the pointer
 * to the vector that we want populated by the vector constructing callback.
 * In order to solve this problem, we use the global variable
 * strsplit_vector_ptr to store the vector pointer.
 */
void string_split (vector_t* vector, const char* string, char delimiter) {
	strsplit_vector_ptr = vector;
	string_split_cb(strsplit_construct_vector, string, delimiter);
}
