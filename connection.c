#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int establish_connection (const char* host_string, uint16_t port) {
	/*
	 * This function is magic as far as I'm concerned. Not really sure what's
	 * going on but I know it somehow opens a connection to a server
	 * and listens on that port. Reading and writing from the socket only
	 * requires having the socket handle which this function returns.
	 */

	/*
	 * Don't know what's going on here, I think it does some DNS lookup and
	 * puts a bunch of data in a structure but the only thing we're interested
	 * in is the IP address which I guess is in the h_addr field.
	 */
	struct hostent* host = gethostbyname(host_string);

	/*
	 * Cast the h_addr into an in_addr. I guess the two structures are
	 * interchangeable. Probably nothing interesting here.
	 */
	struct in_addr* host_addr = (struct in_addr*) host->h_addr;

	// Open a socket. I think AF_INET means an Internet socket.
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1) {
		perror("Error opening socket");
	}

	/*
	 * Create a new structure of type socketaddr_in. I assume that's for incoming
	 * connections?
	 */
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET; // Internet address

	/*
	 * This line is apparently needed because network byte-orders may be
	 * different to the processor's so this function converts a given number to
	 * the correct representation.
	 */
	server_addr.sin_port = htons(port);

	// Dereference the pointer so this actually creates a copy.
	server_addr.sin_addr = *host_addr;

	/*
	 * No idea what's going on here. All bzero does is zero the first n bytes
	 * (in this case, 8) but I have no idea what the sin_zero field is. Maybe
	 * this just makes sure it's initialised to a value of zero instead of some
	 * undefined value.
	 */
	bzero(&(server_addr.sin_zero), 8);

	/*
	 * Convert the sockaddr_in pointer into a sockaddr pointer. I assume these
	 * are interchangeable.
	 */
	struct sockaddr* server_addr_pntr = (struct sockaddr*) &server_addr;

	// Try to connect to the server.
	int conn_attempt = connect(sock, server_addr_pntr, sizeof(struct sockaddr));

	if (conn_attempt == -1) {
		perror("Error establishing connection");
	}

	return sock;
}
