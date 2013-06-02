#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "connection.h"
#include "config.h"
#include "irc.h"
#include "threadpool.h"

#define CONFIG_FILE "bot.conf"

#define TRUE 1
#define FALSE 0
#define RECEIVE_BUFFER_SIZE 1024

int main (int argc, char** argv) {
	queue_t queue;
	queue_init(&queue, sizeof(char*), 1);

	// We only want a 'threadpool' of size one as we still want to process
	// messages in order but we don't want our main data receiving loop to have
	// to wait for a message to be processed before reading from the socket.
	threadpool_t threadpool;
	threadpool_init(&threadpool, 1, &queue, &irc_handle_chunk);

	config_t config;
	read_config(&config, CONFIG_FILE);

	int sock = establish_connection(config.host, config.port);

	int bytes_received;
	char bytes_in[RECEIVE_BUFFER_SIZE];
	char* work;

	while (TRUE) {
		// Receive up to RECEIVE_BUFFER_SIZE bytes of data from the server
		bytes_received = recv(sock, bytes_in, RECEIVE_BUFFER_SIZE - 1, 0);

		if (bytes_received < 0) {
			perror("Error on socket");
		} else if (bytes_received == 0) {
			printf("Connection terminated. Shutting down.\n");
			break;
		}

		// Make the last character a null character so that printf can print
		// the whole thing as a string.
		bytes_in[bytes_received] = '\0';
		printf("%s", bytes_in);

		// Flush standard output so we see the strings printed as soon as we
		// get them from the server.
		fflush(stdout);

		// Copy the data into some heap memory to pass to the threadpool to
		// deal with.
		work = calloc(bytes_received + 1, sizeof(char));
		strncpy(work, bytes_in, bytes_received + 1);
		threadpool_add_work(&threadpool, &work);
	}

	config_destroy(&config);
	threadpool_destroy(&threadpool);
	return EXIT_SUCCESS;
}
