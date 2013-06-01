#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vendor/sqlite3.h"

#include "connection.h"
#include "config.h"
#include "threadpool.h"

#define CONFIG_FILE "bot.conf"

#define TRUE 1
#define FALSE 0
#define RECEIVE_BUFFER_SIZE 1024

int main (int argc, char** argv) {
	config_t config;
	read_config(&config, CONFIG_FILE);

	int sock = establish_connection(config.host, config.port);

	int bytes_received;
	char bytes_in[RECEIVE_BUFFER_SIZE];

	while (TRUE) {
		// Receive up to RECEIVE_BUFFER_SIZE bytes of data from the server
		bytes_received = recv(sock, bytes_in, RECEIVE_BUFFER_SIZE - 1, 0);

		// Make the last character a null character so that printf can print
		// the whole thing as a string.
		bytes_in[bytes_received] = '\0';
		printf("%s", bytes_in);

		// Flush standard output so we see the strings printed as soon as we
		// get them from the server.
		fflush(stdout);
	}
}
