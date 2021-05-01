/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson
 Email		 : Jordan.Johnson@okstate.edu
 Date		 : 4/26/2021
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
#include <dirent.h>
#include <stdbool.h>

#include "serverHeader.h"

static pthread_mutex_t resLock = PTHREAD_MUTEX_INITIALIZER;

/**
 *  Jordan's Method with to write the summary data to a text file.
 *
 *  parameters:
 *  	Reservation res: the reservation to write to the file.
 *  	Train t: the updated train record with that reservation for reference.
 *
 */
void writeSummaryJordan(struct Reservation *res, struct Train *t) {
	printf("Write Summary Jordan\n");

	FILE *summaryFile = fopen("Summary.txt", "a");
	fprintf(summaryFile, "Reservation id: %d, Reservation date: %s, Train ID: %d, Number of Travelers: %d ", res->serverId, res->updateDate, t->id, res->numTickets);
	fclose(summaryFile);
}



/**
 * The method was 100% created by Jordan to at first show how to interact with the client and then used as a testing component since other pieces were not working in reservations.c
 *
 * parameters:
 * 	connectionFd: client connection pointer
 * 	jordan: void to designated a different signature for the program.
 *
 * return: void
 */
void createReservationJordan(int connectionFd) {
	printf("Create Reservation Jordan\n");
	char client_message[2000];
	char *server_response;

	//Ask the client for input on when they want to travel.
	server_response =
			"Please choose which day you would like to make a reservation (1-2): \n\t1. Today.\n\t2. Tomorrow.\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);
	printf("Client(%d) Option %s selected\n", connectionFd, client_message);
	char dateRequested[SMALL_BUFFER];
	strcpy(dateRequested, client_message);

	server_response = "continue";
	send(connectionFd, server_response, strlen(server_response), 0);

	struct Train *train = malloc(sizeof(struct Train) + (sizeof(struct Seat) * TRAIN_ROWS * TRAIN_COLS));

	//Assumption: when a train is closed and runs, it swaps 2 to 1 and empties 2.  Thus always train_1 and train_2.
	char trainFile[12];
	snprintf(trainFile, 12, "train_%d.txt", atoi(dateRequested));
	initTrain(train, trainFile);
	train->id = atoi(dateRequested);

	//Available tickets need to be synchronized across servers and threads.

	//check if we have this many tickets available?
	int ticketsRequested = 0;

	while (1) {
		//Build the Customer / Ticket Records
		server_response = "How many tickets do you want for this reservation (1-10):\n";
//		strcat(server_response,train->availableSeats);
//		strcat(server_response,"):\n");
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		send(connectionFd, "continue", strlen("continue"), 0);

		ticketsRequested = atoi(client_message);
		bzero(&client_message, sizeof(client_message));
		printf("Client(%d) - %d tickets\n", connectionFd, ticketsRequested);
		//Error checking to make sure that we have entered a number within the available seats.
		if (ticketsRequested <= train->availableSeats) {
			break;
		}
	}

	//Build out the customer based on client input.
	struct Customer *customers = malloc(sizeof(struct Customer) * ticketsRequested);

	int t = 0;
	for (t = 0; t < ticketsRequested; t++) {
		server_response = "Please input the information for the Customer.\n\tFull Name:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		printf("Client(%d) - %s\n", connectionFd, client_message);
		strcpy(&customers[t].name, client_message);
		bzero(&client_message, sizeof(client_message));
		send(connectionFd, "continue", strlen("continue"), 0);

		server_response = "\n\tDate of Birth:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		printf("Client(%d) - %s\n", connectionFd, client_message);
		strcpy(&customers[t].dateOfBirth, client_message);
		bzero(&client_message, sizeof(client_message));
		send(connectionFd, "continue", strlen("continue"), 0);

		server_response = "\n\tGender:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		printf("Client(%d) - %s\n", connectionFd, client_message);
		strcpy(&customers[t].gender, client_message);
		bzero(&client_message, sizeof(client_message));
		send(connectionFd, "continue", strlen("continue"), 0);

		server_response = "\n\tGovernment ID Number:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		printf("Client(%d) - %s\n", connectionFd, client_message);
		strcpy(&customers[t].governmentId, client_message);
		bzero(&client_message, sizeof(client_message));
		send(connectionFd, "continue", strlen("continue"), 0);
	}

	server_response = "Do you want to make the reservation (yes/no):\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);
	printf("Client(%d) Option %s selected\n", connectionFd, client_message);
	//Don't send a continue message because we are going to send them a receipt.
	//send(connectionFd, "continue", strlen("continue"), 0);

	if (strcmp(client_message, "no") == 0) {
		send(connectionFd, "continue", strlen("continue"), 0);
		return;
	}

	bzero(&client_message, sizeof(client_message));

	printf("\tReservation.c - entering critical section\n");

	//update the Train
	train->availableSeats -= ticketsRequested;

	// Create the Reservation File
	struct Reservation *res = malloc(sizeof(struct Reservation) + sizeof(struct Ticket) * ticketsRequested);
	res->numTickets = ticketsRequested;
	res->updateDate = dateRequested;
	res->serverId = 100 + rand() % (1000 - 100 + 1); //TODO: get this from the Server Main
	res->number = 10000 + rand() % (100000 - 10000 + 1);

	while (1) {
		//LOCK THE OTHER THREADS
//		pthread_mutex_lock(&resLock);
		struct Ticket *t = malloc(sizeof(struct Ticket) * ticketsRequested + sizeof(struct Customer));
		//Create some Tickets!!!!
		for (int i = 0; i < ticketsRequested; i++) {
			printf("Ticket[%d]\n",i);
			//Assumption: Pick the first available seats.
			for (int r = 0; r < TRAIN_ROWS; r++) {
				int brekcondition = 0;
				for (int j = 0; j < TRAIN_COLS; j++) {
					char *status = train->seats[r][j].status;
					printf("Seat[%d][%d] = %s\n",r,j,status);
					//This is the first empty seat.
					if (strcmp(status, "O") == 0) {

						//Update the Train
						train->seats[r][j].status = "X";

						//Update the ticket
//						t[i].id = 10000 + rand() % (100000 - 10000 + 1);
						t[i].customer = customers[i];
						t[i].seatNumber = (r * TRAIN_COLS) + (j + 1);
						t[i].travelDate = dateRequested;
						t[i].updateDate = "create";

						printf("Ticket Created.\n");
						//Break the loops.
//						r = TRAIN_ROWS;
						brekcondition = 1;
						break;
					}
				}
				if(brekcondition == 1){
					break;
				}
			}
		}

		//Link the reservation and tickets.
		res->tickets = t;

		int updated = updateTrain(train, trainFile);

		if (updated == 0) {
			//Release the lock so others can start updating the train file.
//			pthread_mutex_unlock(&resLock);

			//Write the summary file and send a message back to the client.
			writeSummaryJordan(res, train);
			send(connectionFd, "Worked!", strlen("Worked!"), 0);
			break;
		} else {
			//send(connectionFd, "Error please try again.", strlen("Error please try again."), 0);
			//TODO: need a condition here.
			printf("Out of synch, trying again.");
		}
	}
}
