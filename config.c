#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define CONF_BUF_LEN 4096
#define PARSE_BUF_LEN 1024
#define CONF_DEFAULT_PORT 6667
#define CONF_MAX_CHANNELS 20

void parse_config_string (const char* string, int len, char** dest) {
	*dest = (char*) calloc(len + 1, sizeof(char));
	memmove(*dest, string, len * sizeof(char));
	(*dest)[len] = '\0';
}

void parse_config_uint16 (const char* string, uint16_t* dest) {
	errno = 0;
	*dest = (uint16_t) strtol(string, NULL, 10);

	if (errno != 0) {
		fprintf(stderr, "Error converting port number, using default value.\n");
		*dest = CONF_DEFAULT_PORT;
	}
}

void parse_config_list (char* string, int len, char*** dest) {
	int i;
	int channel_len;
	int last_comma = 0;
	int channel_num = 0;
	*dest = (char**) calloc(CONF_MAX_CHANNELS, sizeof(char*));

	// Decrement string here to simulate having a comma just before the string.
	string--;

	// We must start at 1 because the pointer was decremented.
	for (i = 1; i <= len; i++) {
		if (i == len || string[i] == ',') {
			channel_len = i - last_comma - 1;
			(*dest)[channel_num] = (char*) calloc(channel_len + 1, sizeof(char));
			memmove((*dest)[channel_num], string + last_comma + 1, channel_len * sizeof(char));
			(*dest)[channel_num][channel_len] = '\0';

			channel_num++;
			last_comma = i;

			if (channel_num >= CONF_MAX_CHANNELS) {
				fprintf(stderr, "Maximum number of channels reached.\n");
				break;
			}
		}
	}
}

/*
 * We use three static variables here which retain their value even through
 * multiple calls of the function. We then loop over each character in the data
 * and use it to parse the config file.
 */
void parse_config_data (const char* data, int len, config_t* config) {
	static int state = 0;
	static int buffer_pos = 0;
	static char buffer[PARSE_BUF_LEN];

	char** pntr;
	char c;
	int i;

	for (i = 0; i < len; i++) {
		c = data[i];

		if (c == '=') {
			buffer[buffer_pos] = '\0';
			state = 1;

			if (strcmp("host", buffer) == 0) {
				pntr = &(config->host);
			} else if (strcmp("nick", buffer) == 0) {
				pntr = &(config->nick);
			} else if (strcmp("pass", buffer) == 0) {
				pntr = &(config->pass);
			} else if (strcmp("port", buffer) == 0) {
				state = 2;
			} else if (strcmp("channels", buffer) == 0) {
				state = 3;
			}

			memset(buffer, 0, PARSE_BUF_LEN * sizeof(char));
			buffer_pos = 0;
		} else if (c == '\n') {
			switch (state) {
				case 1:
					parse_config_string(buffer, buffer_pos, pntr);
					break;

				case 2:
					parse_config_uint16(buffer, &(config->port));
					break;

				case 3:
					parse_config_list(buffer, buffer_pos, &(config->channels));
					break;
			}

			memset(buffer, 0, PARSE_BUF_LEN * sizeof(char));
			buffer_pos = 0;
		} else {
			buffer[buffer_pos] = c;
			buffer_pos++;

			if (buffer_pos >= PARSE_BUF_LEN) {
				fprintf(
					stderr
					, "Config lines are too long to fit in buffer. Exiting.\n"
				);

				exit(1);
			}
		}
	}
}

/*
 * Reads the config file 4096 bytes at a time and passes the data to another
 * function for parsing. Directly modifies the config structure passed to it
 * as a pointer
 */
void read_config (config_t* config, const char* filename) {
	FILE* file = fopen(filename, "r");

	if (file == NULL) {
		fprintf(stderr, "Error opening config file for reading.\n");
	}

	// Create a buffer to read chunks of a file at a time.
	char buffer[CONF_BUF_LEN];
	int bytes_read;

	while (!feof(file)) {
		bytes_read = fread(buffer, sizeof(char), CONF_BUF_LEN, file);
		parse_config_data(buffer, bytes_read, config);
	}

	fclose(file);
}
