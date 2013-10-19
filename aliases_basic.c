#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "alias_map.h"
#include "elu.h"
#include "irc.h"

extern int sock;

char* alias_say (const alias_arg* params) {
	const char* arg = irc_get_arg(params->msg);
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
	const char* arg = irc_get_arg(params->msg);
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
