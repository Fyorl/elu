#ifndef __IRC_H
#define __IRC_H

#include <stdint.h>

void irc_handle_chunk (char* chunk, int64_t timestamp);
int irc_send_raw (const char* msg);
int irc_send_privmsg (const char* channel, const char* msg);
void alias_runner (void* data);

#endif
