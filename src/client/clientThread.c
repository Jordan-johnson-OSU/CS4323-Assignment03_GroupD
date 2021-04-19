/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson
 Email		 : jjohn84@ostatemail.okstate.edu,
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "clientHeader.h"

#define PORT 9090
#define SA struct sockaddr

/**
 *
 */
void* clientThread(void *arg) {
	printf("In thread\n");
	char response[2000];
	char server_message[2000];

	/**
	 * Open the Socket to the Server
	 */
	int socketId;
	struct sockaddr_in saddress;

	// create socket
	socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bzero(&saddress, sizeof(saddress));

	// assign IP, PORT
	saddress.sin_family = AF_INET;
	saddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddress.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(socketId, (SA*) &saddress, sizeof(saddress)) < 0) {
		printf("\tSocket failed to connect.\n");
		close(socketId);
		pthread_exit(NULL);
	} else {
		printf("\tSocket successfully connected to the server.\n");
	}

	while (1) {

		bzero(&server_message, sizeof(server_message));

		//Read the response from the server into the server_message
		if (recv(socketId, server_message, 2000, 0) < 0) {
			printf("Receive failed\n");
			//TODO: is this right?
			break;
		}
		//Print the received message
		printf("%s\n", server_message);

		scanf("%s", response);

		if (send(socketId, response, strlen(response), 0) < 0) {
			printf("Send failed\n");
		}

		bzero(&response, sizeof(response));
		bzero(&server_message, sizeof(server_message));

		//Read the response from the server to continue.
		if (recv(socketId, server_message, 2000, 0) < 0) {
			printf("Receive failed\n");
		}

		if (strcmp(server_message, "exit") == 0) {
			printf("Session exiting, Good Bye!");
			break;
		}

		bzero(&server_message, sizeof(server_message));
	}

	close(socketId);
	pthread_exit(NULL);
}
