#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "elu.h"
#include "irc.h"

extern int sock;

char* alias_say (const char* nick, const char* channel, const char* msg) {
	const char* arg = irc_get_arg(msg);

	if (arg[0] != '\0') {
		irc_send_privmsg(channel, arg);
	}

	return 0;
}

char* alias_reply (const char* nick, const char* channel, const char* msg) {
	const char* arg = irc_get_arg(msg);
	char* reply;

	if (arg[0] != '\0') {
		reply = calloc(strlen(nick) + strlen(arg) + 3, sizeof(char));
		sprintf(reply, "%s: %s", nick, arg);
		irc_send_privmsg(channel, reply);

		free(reply);
	}

	return 0;
}
