#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "elu.h"
#include "string_utils.h"

extern int sock;
extern config_t* config;

int irc_send_raw (const char* msg) {
	int bytes_written = write(sock, msg, strlen(msg));

	if (bytes_written < 1) {
		fprintf(stderr, "Error writing message to socket.\n");
	}

	return bytes_written;
}

int irc_send_privmsg (const char* channel, const char* msg) {
	char raw[1024];
	snprintf(raw, 1024, "PRIVMSG %s :%s\r\n", channel, msg);
	return irc_send_raw(raw);
}

void irc_register () {
	char msg[1024];
	snprintf(msg, 1024, "NICK %s\r\nUSER %s 8 * : %s\r\n", config->nick, config->nick, config->nick);
	irc_send_raw(msg);
}

void identify () {
	char msg[1024];
	snprintf(msg, 1024, "id %s", config->pass);
	irc_send_privmsg("nickserv", msg);

	// Join channels.
	int i;
	for (i = 0; i < config->channels.length; i++) {
		snprintf(msg, 1024, "JOIN %s\r\n", vector_get(config->channels, i, char*));
		irc_send_raw(msg);
	}
}

void pong () {
	char msg[1024];
	snprintf(msg, 1024, "PONG : %s\r\n", config->host);
	irc_send_raw(msg);
}

void filter (char* string) {
	// Try to determine if the string is interesting or not and, if so, pass it
	// on to the appropriate function.
	vector_t colon_split;
	vector_init(&colon_split, sizeof(char*));
	string_split(&colon_split, string, ":");

	if (strpos(vector_get(colon_split, 0, char*), "439 *") > -1) {
		irc_register();
	} else if (strpos(vector_get(colon_split, 0, char*), "001") > -1) {
		identify();
	} else if (strpos(vector_get(colon_split, 0, char*), "PING") > -1) {
		pong();
	}

	vector_free_deep(&colon_split);
}

void irc_handle_chunk (void* chunk) {
	// Since we sometimes receive multiple lines at once, we have to split on
	// those lines before sending it to the filter function.
	vector_t lines;
	vector_init(&lines, sizeof(char*));
	string_split(&lines, chunk, "\r\n");

	int i;
	for (i = 0; i < lines.length; i++) {
		filter(vector_get(lines, i, char*));
	}

	vector_free_deep(&lines);
	free(chunk);
}
