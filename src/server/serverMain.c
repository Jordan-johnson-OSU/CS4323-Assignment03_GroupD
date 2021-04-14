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
#define SERVER_CNT 5
#define SA struct sockaddr


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 *
 */
void* socketThread(void *arg) {
	int newSocket = *((int*) arg);
	printf("Client(%d) Connected to Server Thread: %d\n",newSocket, getpid());

	char client_message[2000];
	char *server_response;
	int read_size;

	while( 1 ) {
		server_response = "Please choose an Option (1-5): \n\t1. Make a reservation.\n\t2. Inquire about a ticket.\n\t3. Modify the reservation.\n\t4. Cancel the reservation.\n\t5. Exit the program.\n";
		//printf("Sending message size: %d, |%s|", strlen(server_response), server_response);
		send(newSocket, server_response, strlen(server_response), 0);

		recv(newSocket, client_message, 2000, 0);

		printf("Option %s selected\n", client_message);

		if(strcmp(client_message, "5") == 0) {
			server_response = "exit";
			send(newSocket, server_response, strlen(server_response), 0);
			break;
		}

		server_response = "continue";
		send(newSocket, server_response, strlen(server_response), 0);

		if(strcmp(client_message, "1") == 0) {

			//find the critical section
			pthread_mutex_lock(&lock);
			pthread_mutex_unlock(&lock);
			//End critical section.

		} else if(strcmp(client_message, "2") == 0) {

		} else if(strcmp(client_message, "3") == 0) {

		} else if(strcmp(client_message, "4") == 0) {

		}
	}

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

	//TODO: Make this an input?
	pthread_t tid[SERVER_CNT];
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

		//We are all full of connections, lets wait to free one up.
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
	return 0;
}
