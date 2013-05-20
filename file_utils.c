#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_utils.h"
#include "string_utils.h"

void (*file_split_callback)(const char*);

/*
 * Simply reads a file in chunks of bytes and sends each chunk to a given
 * function to deal with.
 */
void read_file (const char* filename, void (*chunk_handler)(const char*, int, int)) {
	FILE* file = fopen(filename, "r");

	if (file == NULL) {
		fprintf(stderr, "Error opening config file for reading.\n");
	}

	// Create a buffer to read chunks of a file at a time.
	char buffer[FILE_READ_CHUNK_SIZE];
	int bytes_read;

	while (!feof(file)) {
		bytes_read = fread(buffer, sizeof(char), FILE_READ_CHUNK_SIZE, file);
		chunk_handler(buffer, bytes_read, feof(file));
	}

	fclose(file);
}

/*
 * We take a chunk of data and then prepare it so that it's in the right form
 * to send to string_split. Since it's very likely that the chunk of data we
 * get sent does not end on a newline character we have to chop off the end
 * and store it for later to be placed onto the front of the next chunk. We
 * use string_split_cb so that each line can be passed to a callback.
 */
void file_split_lines (const char* chunk, int len, int eof) {
	static char buffer[FILE_MAX_LINE_LEN];
	static int n = 0;

	char* data = calloc(n + len + 1, sizeof(char));

	if (n > 0) {
		// If there is some data in the buffer from the last chunk, prepend it
		// to the data we send to string_split.
		memmove(data, buffer, n * sizeof(char));
	}

	memmove(data + n, chunk, len * sizeof(char));
	data[n + len] = '\0';

	if (!eof && chunk[len - 1] != '\n') {
		// If it's not the end of the file and the last character is not a new
		// line then the chunk of the file that has been read is in the middle
		// of a line so we must save it in the buffer and send only the string
		// from the beginning and up to the last newline character.

		int count = 0;
		while (chunk[len - count - 1] != '\n' && len - count - 1 >= 0) {
			count++;
		}

		int offset = len - count;

		if (count > FILE_MAX_LINE_LEN) {
			fprintf(stderr, "Trying to buffer too much data.");
		} else {
			n = count;
			memmove(buffer, chunk + offset, count * sizeof(char));
		}

		// Lastly, place a null character to effectively truncate the data
		// before sending it off to string_split.
		data[offset] = '\0';
	} else {
		n = 0;
	}

	string_split_cb(file_split_callback, data, '\n');
	free(data);
}

/*
 * We use read_file to read in a file one chunk at a time and send each chunk
 * to file_split_lines. Because file_split_lines is only allowed to accept a
 * chunk of data and its length, it has no way of knowing which callback to
 * call on
 */
void file_get_lines (const char* filename, void (*line_handler)(const char*)) {
	file_split_callback = line_handler;
	read_file(filename, &file_split_lines);
}
