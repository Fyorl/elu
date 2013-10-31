#include <pthread.h>
#include <stdbool.h>
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


char* timer_response (int days, int hours, int minutes, int seconds) {
	char join[640];
	char** pieces = calloc(4, sizeof(char*));
	int len;
	
	char* days_str = NULL;
	char* hours_str = NULL;
	char* minutes_str = NULL;
	char* seconds_str = NULL;
	
	if (days > 0) {
		days_str = malloc(64);
		snprintf(days_str, 62, "%d day", days);
		
		if (days > 1) {
			len = strlen(days_str);
			days_str[len] = 's';
			days_str[len + 1] = '\0';
		}
	}
	
	if (hours > 0) {
		hours_str = malloc(64);
		snprintf(hours_str, 62, "%d hour", hours);
		
		if (hours > 1) {
			len = strlen(hours_str);
			hours_str[len] = 's';
			hours_str[len + 1] = '\0';
		}
	}
	
	if (minutes > 0) {
		minutes_str = malloc(64);
		snprintf(minutes_str, 62, "%d minute", minutes);
		
		if (minutes > 1) {
			len = strlen(minutes_str);
			minutes_str[len] = 's';
			minutes_str[len + 1] = '\0';
			printf("%s\n", minutes_str);
		}
	}
	
	if (seconds > 0) {
		seconds_str = malloc(64);
		snprintf(seconds_str, 62, "%d second", seconds);
		
		if (seconds > 1) {
			len = strlen(seconds_str);
			seconds_str[len] = 's';
			seconds_str[len + 1] = '\0';
		}
	}
	
	pieces[0] = days_str;
	pieces[1] = hours_str;
	pieces[2] = minutes_str;
	printf("pieces[2] = %s\n", pieces[2]);
	pieces[3] = seconds_str;
	
	string_join_english(pieces, 4, join, 638);
	
	int i;
	for (i = 0; i < 4; i++) {
		if (pieces[i] != NULL) {
			printf("Freeing index %d\n", i);
			printf("%s\n", pieces[i]);
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
	printf("\n\n");
	
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
	hours -= hours * 24;
	
	if (days > 0 || hours > 0 || minutes > 0 || seconds > 0) {
		return timer_response(days, hours, minutes, seconds);
	}
	
	return NULL;
}

char* alias_at (const alias_arg* params) {
	return NULL;
}
