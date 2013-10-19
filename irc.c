#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "alias_map.h"
#include "elu.h"
#include "irc.h"
#include "string_utils.h"
#include "threadpool.h"
#include "timers.h"
#include "queue.h"

extern int sock;
extern config_t* config;
extern hashmap_t alias_map;
extern threadpool_t executor;
extern ticker_t ticker;

struct ident {
	char* nick;
	char* channel;
};

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

struct ident get_nick_channel (const char* string, vector_t* space_split, vector_t* bang_split) {
	struct ident ident;

	vector_init(space_split, sizeof(char*));
	vector_init(bang_split, sizeof(char*));
	string_split(space_split, string + 1, " ");
	string_split(bang_split, vector_get(*space_split, 0, char*), "!");

	ident.nick = vector_get(*bang_split, 0, char*);
	ident.channel = vector_get(*space_split, 2, char*);

	return ident;
}

void alias_runner (void* data) {
	struct alias_runner_arg* params = data;
	(*((alias*) params->func))(params->args);

	free(params->args->nick);
	free(params->args->channel);
	free(params->args->msg);
	free(params->args);
	free(params);
}

void privmsg (const char* string, int64_t timestamp) {
	vector_t space_split;
	vector_t bang_split;
	struct ident ident = get_nick_channel(string, &space_split, &bang_split);

	int colon_pos = strpos(string + 1, ":");
	char* msg = calloc(strlen(string) - colon_pos - 1, sizeof(char));
	strcpy(msg, string + colon_pos + 2);

	int space;
	char* cmd;
	alias_arg* args;
	alias func;

	if (msg[0] == config->command_char[0]) {
		space = strpos(msg, " ");
		if (space < 0) {
			space = strlen(msg);
		}

		cmd = calloc(space, sizeof(char));
		strncpy(cmd, msg + 1, space - 1);
		cmd[space - 1] = '\0';

		args = malloc(sizeof(alias_arg));
		args->nick = calloc(strlen(ident.nick) + 1, sizeof(char));
		strcpy(args->nick, ident.nick);
		args->channel = calloc(strlen(ident.channel) + 1, sizeof(char));
		strcpy(args->channel, ident.channel);
		args->msg = calloc(strlen(msg) + 1, sizeof(char));
		strcpy(args->msg, msg);
		args->timestamp = timestamp;

		struct alias_runner_arg* runner_arg = malloc(sizeof(struct alias_runner_arg));
		runner_arg->args = args;

		func = hashmap_get(&alias_map, cmd);
		if (func == NULL) {
			// Look it up in custom alias table
		} else {
			runner_arg->func = func;
			threadpool_add_work(&executor, &runner_arg);
		}

		free(cmd);
	}

	free(msg);
	vector_free_deep(&bang_split);
	vector_free_deep(&space_split);
}

void join (const char* string) {
	static bool first_run = true;

	if (!first_run) {
		return;
	}

	vector_t space_split;
	vector_t bang_split;
	struct ident ident = get_nick_channel(string, &space_split, &bang_split);

	if (strcmp(ident.nick, config->nick) == 0) {
		first_run = false;
		ticker_register_handler(&ticker, check_timers);
	}

	vector_free_deep(&bang_split);
	vector_free_deep(&space_split);
}

void filter (const char* string, int64_t timestamp) {
	// Try to determine if the string is interesting or not and, if so, pass it
	// on to the appropriate function.
	vector_t colon_split;
	vector_init(&colon_split, sizeof(char*));
	string_split(&colon_split, string, ":");

	char* irc_command = vector_get(colon_split, 0, char*);
	if (irc_command == NULL) {
		vector_free_deep(&colon_split);
		return;
	}

	if (strpos(irc_command, "439 *") > -1) {
		irc_register();
	} else if (strpos(irc_command, "001") > -1) {
		identify();
	} else if (strpos(irc_command, "PING") > -1) {
		pong();
	} else if (strpos(irc_command, "JOIN") > -1) {
		join(string);
	} else if (strpos(irc_command, "PRIVMSG") > -1) {
		privmsg(string, timestamp);
	}

	vector_free_deep(&colon_split);
}

void irc_handle_chunk (char* chunk, int64_t timestamp) {
	// Since we sometimes receive multiple lines at once, we have to split on
	// those lines before sending it to the filter function.
	vector_t lines;
	vector_init(&lines, sizeof(char*));
	string_split(&lines, chunk, "\r\n");

	int i;
	for (i = 0; i < lines.length; i++) {
		filter(vector_get(lines, i, char*), timestamp);
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
