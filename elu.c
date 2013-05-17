#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define RECEIVE_BUFFER_SIZE 1024

int main (int argc, char** argv) {
	// Hard-code the server info for now
	const char* server_host = "irc.rizon.net";
	//const char* server_nick = "elu";
	//const char* server_chan = "#impsvillage";
	const uint16_t server_port = 6667;

	struct hostent* host = gethostbyname(server_host);
	struct in_addr* host_address = (struct in_addr*) host->h_addr;

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr = *host_address;
	bzero(&(server_address.sin_zero), 8);

	struct sockaddr* server_address_pointer = (struct sockaddr*) &server_address;

	connect(sock, server_address_pointer, sizeof(struct sockaddr));

	int bytes_received;
	char bytes_in[RECEIVE_BUFFER_SIZE];

	while (1) {
		bytes_received = recv(sock, bytes_in, RECEIVE_BUFFER_SIZE - 1, 0);
		bytes_in[bytes_received] = '\0';
		printf("%s", bytes_in);
		fflush(stdout);
	}
}
