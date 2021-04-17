/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson, Mohammad Musaqlab
 Email		 : jjohn84@ostatemail.okstate.edu,
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

#include "serverHeader.h"

#define PORT 9090
#define SERVER_CNT 5
#define SA struct sockaddr

/**
 * Arguments:
 * 		argc - count of input arguments to your program.
 * 		argv - pointer to all the input arguments.
 *
 * Return:
 *		0 for Success.
 *
 */
int main(int argc, char **argv) {
	int socketId, connectionId = 0;
	struct sockaddr_in saddress;

	// create socket
	socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bzero(&saddress, sizeof(saddress));

	// assign IP, PORT
	saddress.sin_family = AF_INET;
	saddress.sin_addr.s_addr = htonl(INADDR_ANY);
	saddress.sin_port = htons(PORT);

	// Binding newly created socket
	if (bind(socketId, (SA*) &saddress, sizeof(saddress)) < 0) {
		printf("\tSocket Failed to bind.\n");
		return 1;
	} else {
		printf("\tSocket successful bind.\n");
	}

	// Now server is ready to listen and verification
	if (listen(socketId, 5) < 0) {
		printf("\tServer listening failed.\n");
		return 1;
	} else {
		printf("\tServer listening.\n");
	}

	//TODO: Make this an input?
	pthread_t tid[SERVER_CNT];
	int i = 0;
	while (1) {
		// Accept call creates a new socket for the incoming connection
		// Accept the data packet from client and verification
		connectionId = accept(socketId, (struct sockaddr*) NULL, NULL);
		if (connectionId < 0) {
			printf("\tServer accept failed.\n");
		} else {
			printf("\tServer accept the client.\n");

			//for each client request creates a thread and assign the client request to it to process
			//so the main thread can entertain next request
			if (pthread_create(&tid[i++], NULL, serverThread, &connectionId) != 0)
				printf("Failed to create thread\n");

			//We are all full of connections, lets wait to free.
			if (i >= SERVER_CNT) {
				i = 0;
				while (i < SERVER_CNT) {
					pthread_join(tid[i++], NULL);
					//TODO: free up the connection
					i--;
				}
				i = 0;
			}
		}
	}
	return 0;
}
