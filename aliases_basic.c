#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alias_map.h"
#include "elu.h"
#include "irc.h"
#include "string_utils.h"
#include "vendor/sqlite3.h"

extern int sock;
extern sqlite3* db;
extern pthread_mutex_t db_mutex;

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

	return 0;
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

	return 0;
}

char* alias_in (const alias_arg* params) {
	const char* time_to_add = string_chomp(params->msg, " ");
	const char* cmd = string_chomp(time_to_add, " ");
	
	if (time_to_add[0] == '\0' || cmd[0] == '\0') {
		return 0;
	}
	
	int space = strpos(time_to_add, " ");
	int pos;
	char* tmp;
	
	int days = 0;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	
	pos = strpos(time_to_add, "d");
	if (pos > 0 && pos < space) {
		days = (int) strtol(time_to_add, &tmp, 10);
		time_to_add = tmp + 1;
	}
	
	pos = strpos(time_to_add, "h");
	if (pos > 0 && pos < space) {
		hours = (int) strtol(time_to_add, &tmp, 10);
		time_to_add = tmp + 1;
	}
	
	pos = strpos(time_to_add, "m");
	if (pos > 0 && pos < space) {
		minutes = (int) strtol(time_to_add, &tmp, 10);
		time_to_add = tmp + 1;
	}
	
	pos = strpos(time_to_add, "s");
	if (pos > 0 && pos < space) {
		seconds = (int) strtol(time_to_add, &tmp, 10);
		time_to_add = tmp + 1;
	}
	
	printf("Days: %d\nHours: %d\nMinutes: %d\nSeconds: %d\n", days, hours, minutes, seconds);
	
	return 0;
}

char* alias_at (const alias_arg* params) {
	return 0;
}
