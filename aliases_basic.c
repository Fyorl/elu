#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alias_map.h"
#include "elu.h"
#include "irc.h"
#include "string_utils.h"
#include "timers.h"
#include "timestamp.h"
#include "vendor/sqlite3.h"

extern int sock;
extern sqlite3* db;

char* write_time_string (int time_item, const char* time_item_name) {
	char* str = NULL;
	int len;

	if (time_item > 0) {
		str = malloc(64);
		snprintf(str, 62, "%d %s", time_item, time_item_name);
		
		if (time_item > 1) {
			len = strlen(str);
			str[len] = 's';
			str[len + 1] = '\0';
		}
	}

	return str;
}

char* timer_response (int days, int hours, int minutes, int seconds) {
	char join[640];
	char** pieces = calloc(4, sizeof(char*));
	int len;
	
	pieces[0] = write_time_string(days, "day");
	pieces[1] = write_time_string(hours, "hour");
	pieces[2] = write_time_string(minutes, "minute");
	pieces[3] = write_time_string(seconds, "second");
	
	string_join_english(pieces, 4, join, 638);
	
	int i;
	for (i = 0; i < 4; i++) {
		if (pieces[i] != NULL) {
			free(pieces[i]);
		}
	}
	free(pieces);
	
	len = strlen(join);
	join[len] = '.';
	join[len + 1] = '\0';

	char* response_return = calloc(strlen(join) + 15, sizeof(char));
	strcpy(response_return, "Timer set for ");
	strcat(response_return, join);
	
	return response_return;
}

char* alias_say (const alias_arg* params) {
	const char* arg = string_chomp(params->msg, " ");
	const char* output;

	if (arg[0] != '\0') {
		if (params->channel[0] == '#') {
			output = params->channel;
		} else {
			output = params->nick;
		}

		irc_send_privmsg(output, arg);
	}

	return NULL;
}

char* alias_reply (const alias_arg* params) {
	const char* arg = string_chomp(params->msg, " ");
	char* reply;
	const char* output;

	if (arg[0] != '\0') {
		if (params->channel[0] == '#') {
			output = params->channel;
		} else {
			output = params->nick;
		}

		reply = calloc(strlen(params->nick) + strlen(arg) + 3, sizeof(char));
		sprintf(reply, "%s: %s", params->nick, arg);
		irc_send_privmsg(output, reply);

		free(reply);
	}

	return NULL;
}

const char* parse_timer_string (
		const char* timer_string
		, const char* time_char
		, int space_pos
		, int* result) {

	char* unparsed;
	int pos = strpos(timer_string, time_char);
	if (pos > 0 && pos < space_pos) {
		*result = (int) strtol(timer_string, &unparsed, 10);
		timer_string = unparsed + 1;
	}

	return timer_string;
}

char* alias_in (const alias_arg* params) {
	const char* time_to_add = string_chomp(params->msg, " ");
	const char* cmd = string_chomp(time_to_add, " ");
	
	if (time_to_add[0] == '\0' || cmd[0] == '\0') {
		return 0;
	}
	
	int space = strpos(time_to_add, " ");
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;

	time_to_add = parse_timer_string(time_to_add, "d", space, &days);
	time_to_add = parse_timer_string(time_to_add, "h", space, &hours);
	time_to_add = parse_timer_string(time_to_add, "m", space, &minutes);
	time_to_add = parse_timer_string(time_to_add, "s", space, &seconds);
	
	// Time maths.
	int quotient;
	
	quotient = seconds / 60;
	minutes += quotient;
	seconds -= quotient * 60;
	
	quotient = minutes / 60;
	hours += quotient;
	minutes -= quotient * 60;
	
	quotient = hours / 24;
	days += quotient;
	hours -= quotient * 24;
	
	if (days > 0 || hours > 0 || minutes > 0 || seconds > 0) {
		int64_t ms = timestamp()
			+ (days * 24 * 60 * 60
			+ hours * 60 * 60
			+ minutes * 60
			+ seconds)
			* 1000;

		timer_add(params->nick, params->channel, ms, cmd);
		return timer_response(days, hours, minutes, seconds);
	}
	
	return NULL;
}

char* alias_at (const alias_arg* params) {
	return NULL;
}
