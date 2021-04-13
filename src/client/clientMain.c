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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "header.h"

#define PORT 9090
#define SA struct sockaddr

/**
 *
 */
void* cientThread(void *arg) {
	printf("In thread\n");
	char message[1000];
	char buffer[1024];

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
	} else {
		printf("\tSocket successfully connected to the server.\n");
	}

	strcpy(message, "Hello");

	if (send(socketId, message, strlen(message), 0) < 0) {
		printf("Send failed\n");
	}

	//Read the message from the server into the buffer
	if (recv(socketId, buffer, 1024, 0) < 0) {
		printf("Receive failed\n");
	}
	//Print the received message
	printf("Data received: %s\n", buffer);
	close(socketId);
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
	int i = 0;
	pthread_t tid[51];
	while (i < 50) {
		if (pthread_create(&tid[i], NULL, cientThread, NULL) != 0)
			printf("Failed to create thread\n");
		i++;
	}
	sleep(20);
	i = 0;
	while (i < 50) {
		pthread_join(tid[i++], NULL);
		printf("%d:\n", i);
	}
	return 0;
}
