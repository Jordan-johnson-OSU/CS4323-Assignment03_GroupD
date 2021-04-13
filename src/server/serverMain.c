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

#include "header.h"

#define PORT 9090
#define SA struct sockaddr

char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 *
 */
void* socketThread(void *arg) {
	printf("Socket Connected Thread");
	int newSocket = *((int*) arg);
	recv(newSocket, client_message, 2000, 0);

	// Send message to the client socket
	pthread_mutex_lock(&lock);
	char *message = malloc(sizeof(client_message) + 20);
	strcpy(message, "Hello Client : ");
	strcat(message, client_message);
	strcat(message, "\n");
	strcpy(buffer, message);
	free(message);
	pthread_mutex_unlock(&lock);
	sleep(1);
	send(newSocket, buffer, 13, 0);
	printf("Exit socketThread \n");
	close(newSocket);
	pthread_exit(NULL);
}

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


	pthread_t tid[60];
	int i = 0;
	while (1) {
		// Accept call creates a new socket for the incoming connection
		// Accept the data packet from client and verification
		connectionId = accept(socketId, (struct sockaddr*) NULL, NULL);
		if (connectionId < 0) {
			printf("\tServer accept failed.\n");
			return 1;
		} else {
			printf("\tServer accept the client.\n");
		}


		//for each client request creates a thread and assign the client request to it to process
		//so the main thread can entertain next request
		if (pthread_create(&tid[i++], NULL, socketThread, &connectionId) != 0)
			printf("Failed to create thread\n");

		if (i >= 50) {
			i = 0;
			while (i < 50) {
				pthread_join(tid[i++], NULL);
				//TODO: free up the connection
			}
			i = 0;
		}
	}
	return 0;
}
