#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"
#include "vector.h"

void string_join (
		char** pieces
		, int len
		, const char* delimiter
		, char* result
		, int maxlen) {
	bool first = true;
	int totallen = 0;
	int addlen;
	int i;

	for (i = 0; i < len; i++) {
		if (pieces[i] != NULL) {
			addlen = strlen(pieces[i]) + ((first) ? strlen(delimiter) : 0);
			if (totallen + addlen >= maxlen - 1) {
				return;
			}
		
			if (first) {
				strcpy(result, pieces[i]);
				first = false;
			} else {
				strcat(result, delimiter);
				strcat(result, pieces[i]);
			}
			
			totallen += addlen;
		}
	}
}

void string_join_english (char** pieces, int len, char* result, int maxlen) {
	// First we find the last two elements.
	int ultimate = -1;
	int penultimate = -1;
	int i;

	for (i = 0; i < len; i++) {
		if (pieces[i] != NULL) {
			penultimate = ultimate;
			ultimate = i;
		}
	}
	
	if (penultimate < 0) {
		// Everything was NULL or only one element.
		return;
	}
	
	char* join_last = calloc(
		strlen(pieces[penultimate]) + strlen(pieces[ultimate]) + 6
		, sizeof(char));
	strcpy(join_last, pieces[penultimate]);
	strcat(join_last, " and ");
	strcat(join_last, pieces[ultimate]);
	
	pieces[penultimate] = join_last;
	pieces[ultimate] = NULL;
	
	string_join(pieces, len, ", ", result, maxlen);
	free(join_last);
}

int strpos (const char* string, const char* substring) {
	// Find the earliest occurrence of substring in string.
	if (strlen(string) < strlen(substring)) {
		return -1;
	}

	int start = -1;
	int n = 0;
	int i;

	for (i = 0; i < strlen(string); i++) {
		if (substring[n] == '\0') {
			return start;
		}

		if (substring[n] == string[i]) {
			if (n == 0) {
				start = i;
			}

			n++;
		} else {
			n = 0;
			start = -1;
		}
	}

	return start;
}

const char* string_chomp (const char* string, const char* substring) {
	int space_pos = strpos(string, substring);
	int total_len = strlen(string);

	if (space_pos > -1) {
		return string + space_pos + 1;
	} else {
		return string + total_len;
	}
}

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
void string_split_cb (
		strsplit_cb callback
		, void* cb_data
		, const char* string
		, const char* delimiter) {
	char buffer[STRSPLIT_MAX_ELEM_LEN];

	int delimiter_len = strlen(delimiter);
	int string_len = strlen(string);

	int i = 0;
	int n;

	do {
		// Find the first position of the delimiter in the string.
		n = strpos(string + i, delimiter);

		if (n < 0) {
			n = string_len - i + 1;
		}

		if (n >= STRSPLIT_MAX_ELEM_LEN - 1) {
			fprintf(stderr, "Substring element too long.\n");
		} else if (n > 0) {
			// Copy up to that delimiter into a new string and send it to
			// the callback.
			strncpy(buffer, string + i, n);
			buffer[n] = '\0';
			callback(buffer, cb_data);
		}

		// Advance the current position in the string past the delimiter.
		i += n + delimiter_len;
	} while (i < string_len);
}

/*
 * Split a string and place the results in a vector. Just calls
 * string_split_cb with a specific callback.
 */
void string_split (
		vector_t* vector
		, const char* string
		, const char* delimiter) {
	string_split_cb(&strsplit_construct_vector, vector, string, delimiter);
}
