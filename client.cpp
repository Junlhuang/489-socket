#include <arpa/inet.h>	// ntohs()
#include <stdio.h>		// printf(), perror()
#include <stdlib.h>		// atoi()
#include <string.h>		// strlen()
#include <sys/socket.h>	// socket(), connect(), send(), recv()
#include <unistd.h>		// close()

#include "helpers.h"	// make_client_sockaddr()

static const int MAX_MESSAGE_SIZE = 256;

/**
 * Sends a string message to the server.
 *
 * Parameters:
 *		hostname: 	Remote hostname of the server.
 *		port: 		Remote port of the server.
 * 		message: 	The message to send, as a C-string.
 * Returns:
 *		0 on success, -1 on failure.
 */
int send_message(const char *hostname, int port, const char *message) {
	if (strlen(message) > MAX_MESSAGE_SIZE) {
		perror("Error: Message exceeds maximum length\n");
		return -1;
	}

	// (1) Create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("Error opening stream socket");
		return -1;
	}

	// (2) Create a sockaddr_in to specify remote host and port
	struct sockaddr_in addr;
	if (make_client_sockaddr(&addr, hostname, port) == -1) {
		return -1;
	}

	// (3) Connect to remote server
	if (connect(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Error connecting stream socket");
		return -1;		
	}

	// (4) Send message to remote server
	size_t message_len = strlen(message);
	size_t sent = 0;
	ssize_t sval;
	// Call send() enough times to send all the data
	do {
		sval = send(sockfd, message + sent, message_len - sent, 0);
		if (sval == -1) {
			perror("Error sending on stream socket");
			return -1;
		}
		sent += sval;
	} while (sent < message_len);

	// (5) Close connection
	close(sockfd);

	return 0;
}

int main(int argc, const char **argv) {
	// Parse command line arguments
	if (argc != 4) {
		printf("Usage: ./client hostname port_num message\n");
		return 1;
	}
	const char *hostname = argv[1];
	int port = atoi(argv[2]);
	const char *message = argv[3];

	printf("Sending message %s to %s:%d\n", message, hostname, port);
	if (send_message(hostname, port, message) == -1) {
		return 1;
	}

	return 0;
}
