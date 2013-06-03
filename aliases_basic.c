#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "elu.h"
#include "irc.h"

extern int sock;

char* alias_say (const char* nick, const char* channel, const char* msg) {
	const char* arg = irc_get_arg(msg);
	const char* output;

	if (arg[0] != '\0') {
		if (channel[0] == '#') {
			output = channel;
		} else {
			output = nick;
		}

		irc_send_privmsg(output, arg);
	}

	return 0;
}

char* alias_reply (const char* nick, const char* channel, const char* msg) {
	const char* arg = irc_get_arg(msg);
	char* reply;
	const char* output;

	if (arg[0] != '\0') {
		if (channel[0] == '#') {
			output = channel;
		} else {
			output = nick;
		}

		reply = calloc(strlen(nick) + strlen(arg) + 3, sizeof(char));
		sprintf(reply, "%s: %s", nick, arg);
		irc_send_privmsg(output, reply);

		free(reply);
	}

	return 0;
}
