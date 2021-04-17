/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson
 Email		 : jjohn84@ostatemail.okstate.edu,
 Date		 : 4/14/2021
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

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 *  This is the thread that interacts with the client and will control
 */
void* serverThread(void *arg) {
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
			//TODO: Make a Reservation
			createReservation(newSocket);

			//find the critical section
			pthread_mutex_lock(&lock);
			pthread_mutex_unlock(&lock);
			//End critical section.

		} else if(strcmp(client_message, "2") == 0) {
			//TODO: Inquire about a ticket.
			inquireReservation(newSocket);

		} else if(strcmp(client_message, "3") == 0) {
			//TODO: Modify the Reservation.
			modifyReservation(newSocket);

		} else if(strcmp(client_message, "4") == 0) {
			//TODO: Cancel the Reservation.
			cancelReservation(newSocket);
		}
	}

	printf("Exit socketThread \n");
	close(newSocket);
	pthread_exit(NULL);
}
