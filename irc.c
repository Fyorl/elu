#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "alias_map.h"
#include "elu.h"
#include "string_utils.h"

extern int sock;
extern config_t* config;
extern hashmap_t alias_map;

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

void privmsg (const char* string) {
	vector_t space_split;
	vector_init(&space_split, sizeof(char*));

	vector_t bang_split;
	vector_init(&bang_split, sizeof(char*));

	string_split(&space_split, string + 1, " ");
	string_split(&bang_split, vector_get(space_split, 0, char*), "!");

	char* nick = vector_get(bang_split, 0, char*);
	char* channel = vector_get(space_split, 2, char*);

	int colon_pos = strpos(string + 1, ":");
	char* msg = calloc(strlen(string) - colon_pos - 1, sizeof(char));
	strcpy(msg, string + colon_pos + 2);

	int space;
	char* cmd;
	alias func;

	if (msg[0] == config->command_char[0]) {
		space = strpos(msg, " ");
		if (space < 0) {
			space = strlen(msg);
		}

		cmd = calloc(space, sizeof(char));
		strncpy(cmd, msg + 1, space - 1);
		cmd[space - 1] = '\0';

		func = hashmap_get(&alias_map, cmd);
		if (func == NULL) {
			// Look it up in custom alias table
		} else {
			(*((alias*) func))(nick, channel, msg);
		}

		free(cmd);
	}

	free(msg);
	vector_free_deep(&bang_split);
	vector_free_deep(&space_split);
}

void filter (char* string) {
	// Try to determine if the string is interesting or not and, if so, pass it
	// on to the appropriate function.
	vector_t colon_split;
	vector_init(&colon_split, sizeof(char*));
	string_split(&colon_split, string, ":");

	char* irc_command = vector_get(colon_split, 0, char*);

	if (strpos(irc_command, "439 *") > -1) {
		irc_register();
	} else if (strpos(irc_command, "001") > -1) {
		identify();
	} else if (strpos(irc_command, "PING") > -1) {
		pong();
	} else if (strpos(irc_command, "PRIVMSG") > -1) {
		privmsg(string);
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

const char* irc_get_arg (const char* msg) {
	int space_pos = strpos(msg, " ");
	int total_len = strlen(msg);

	if (space_pos > -1) {
		return msg + space_pos + 1;
	} else {
		return msg + total_len;
	}
}
