#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "net.h"

//#include <sys/types.h>
//#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <errno.h>



int main(void) {
	int rv;
	int sockfd, newfd;
	struct addrinfo hints, *res, *p;
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//rv = getaddrinfo(NULL, PORT, &hints, &res);
	rv = getaddrinfo(HOST, PORT, &hints, &res);
	if (rv == -1) {
		printf("ERROR: Could not get address info\n");
		exit(0);
	}

	for(p = res; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd == -1) {
			continue;
		}


		rv = bind(sockfd, p->ai_addr, p->ai_addrlen);
		if (rv == -1) {
			shutdown(sockfd, 2);
			continue;
		}

		break;
	}

	if (p == NULL) {
		printf("ERROR: Could not get socket file descriptor\n");
		exit(EXIT_FAILURE);
	}

	rv = listen(sockfd, BACKLOG);
	if  (rv == -1) {
		printf("ERROR: Failed to listen\n");
		exit(EXIT_FAILURE);
	}

	printf("Waiting for connections...\n");

	while(1) {
		client_addr_size = sizeof client_addr;
		newfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_size);
		if (newfd == -1) {
			printf("WARNING: Failed to accept client %d\n", newfd); 
			printf("errno: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
			continue;
		}

		if (!fork()) {
			shutdown(sockfd, 2);
			rv = send(newfd, "Andrew", 6, 0);
			printf("send rv:%d\n",rv);
			if (rv != 6) {
				printf("ERROR: Failed to send message\n");
				printf("errno: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			shutdown(newfd, 2);
			exit(0);
		}
		shutdown(newfd, 2);
	}

	return 0;
}
