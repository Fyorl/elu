#ifndef __IRC_H
#define __IRC_H

struct irc_msg {
	char* msg;
	long timestamp;
};

void irc_handle_chunk (void* chunk);
int irc_send_raw (const char* msg);
int irc_send_privmsg (const char* channel, const char* msg);
const char* irc_get_arg (const char* msg);

#endif
