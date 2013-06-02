#ifndef __IRC_H
#define __IRC_H

void irc_handle_chunk (void* chunk);
int irc_send_raw (const char* msg);

#endif
