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
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>

#include "serverHeader.h"

#define PORT 9090
#define THREAD_CNT 3
#define SA struct sockaddr

pthread_mutex_t tPoolLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tPoolCond = PTHREAD_COND_INITIALIZER;

/**
 * This is just a thread that is running the backend code to handle the client, but waiting for a new connection on the queue, and waiting until one is available to not take up all the CPU.
 *
 * Arguments:
 * 		arg - not needed (NULL)
 * 		argv - pointer to all the input arguments.
 *
 * Return:
 *		0 for Success.
 */
void* monitorThread(void *arg) {
	while (1) {
		pthread_mutex_lock(&tPoolLock);
		int *client;
		if ((client = readQueue()) == NULL) {
			pthread_cond_wait(&tPoolCond, &tPoolLock);
			client = readQueue();
		}
		pthread_mutex_unlock(&tPoolLock);
		if (client != NULL) {
			//Client connection available.
			serverThread(client);
		}
	}
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

	if (pthread_mutex_init(&tPoolLock, NULL) != 0) {
		printf("Mutex init has failed\n");
		return 1;
	}

	pthread_t tid[THREAD_CNT];
	int i = 0;
	for (i = 0; i < THREAD_CNT; i++) {
		pthread_create(&tid[i], NULL, monitorThread, NULL);
	}

	while (1) {
		// Accept call creates a new socket for the incoming connection
		// Accept the data packet from client and verification
		connectionId = accept(socketId, (struct sockaddr*) NULL, NULL);
		if (connectionId < 0) {
			printf("\tServer accept failed.\n");
		} else {
			printf("\tServer accept the client.\n");

			//Add the connection to the Queue to be processed by the server Threads, but lock adding it and popping it from the queue.
			pthread_mutex_lock(&tPoolLock);
			putQueue(connectionId);
			pthread_cond_signal(&tPoolCond);
			pthread_mutex_unlock(&tPoolLock);

		}
	}

	pthread_mutex_destroy(&tPoolLock);

	return 0;
}
