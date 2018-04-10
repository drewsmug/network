#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "net.h"

#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char ** argv) {
	int rv;
	int sockfd, numbytes;
	struct addrinfo hints, *res, *p;
	char buf[MAXDATASIZE];

	/*
	if (argc != 2) {
		printf("usage: client ip\n");
		exit(0);
	}
	*/

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	rv = getaddrinfo(HOST, PORT, &hints, &res);
	if (rv) {
		printf("ERROR: Could not get address info\n");
		exit(0);
	}

	for(p = res; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (!sockfd) {
			continue;
		}


		rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
		if (rv) {
			shutdown(sockfd, 2);
			continue;
		}

		break;
	}

	if (p == NULL) {
		printf("ERROR: Could not connect\n");
		exit(0);
	}

	printf("Connecting to server...\n");

	freeaddrinfo(res);

	numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
	buf[numbytes] = '\0';

	printf("buf: %s\n",buf);

	shutdown(sockfd, 2);

	return 0;
}
