/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Mohammad Musaqlab
 Email		 : Mohammed.Musaqlab@okstate.edu
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
#include <dirent.h>
#include <stdbool.h>

#include "serverHeader.h"

static pthread_mutex_t resLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t resCond = PTHREAD_COND_INITIALIZER;

/**
 *
 */
void createReservation(int connectionFd) {
	printf("Create Reservation");
	char client_message[2000];
	char *server_response;
	int i;
	int selectedSeats[TRAIN_ROWS * TRAIN_COLS];

	server_response =
			"Please choose which day you would like to make a reservation (1-2): \n\t1. Today.\n\t2. Tomorrow.\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);
	printf("Client(%d) Option %s selected\n", connectionFd, client_message);
	char dateRequested[SMALL_BUFFER];
	strcpy(dateRequested, client_message);

	server_response = "continue";
	send(connectionFd, server_response, strlen(server_response), 0);

	//TODO: go find if we have a file or make it?
	struct Train *train = malloc(sizeof(struct Train) + (sizeof(struct Seat) * TRAIN_ROWS * TRAIN_COLS));

	//TODO: need to figure the dates out.
	initTrain(train, "train_{date}.txt");
	//Available tickets need to be synchronized across servers and threads.

	//TODO: check if we have this many tickets available?

	//Build the Customer / Ticket Records
	server_response = "How many tickets do you want for this reservation (1-10):\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);
	send(connectionFd, "continue", strlen("continue"), 0);

	int ticketsRequested = atoi(client_message);

	printf("Client(%d) - %d tickets\n", connectionFd, ticketsRequested);

	//TODO: check if we have this many tickets available?
	struct Customer *customers = malloc(sizeof(struct Customer) * ticketsRequested);

	int t = 0;
	for (t = 0; t < ticketsRequested; t++) {
		//TODO: set Id

		server_response = "Please input the information for the Customer.\n\tFull Name:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(customers[t].name, client_message);
		send(connectionFd, "continue", strlen("continue"), 0);

		server_response = "\n\tDate of Birth:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(customers[t].dateOfBirth, client_message);
		send(connectionFd, "continue", strlen("continue"), 0);

		server_response = "\n\tGender:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(customers[t].gender, client_message);
		send(connectionFd, "continue", strlen("continue"), 0);

		server_response = "\n\tGovernment ID Number:";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(customers[t].governmentId, client_message);
		send(connectionFd, "continue", strlen("continue"), 0);
	}

	//TODO: need to probably ask about what seat they want?  Can't pass struct, only strings.
	for (i = 0; i < ticketsRequested; i++) {
		train->seats[selectedSeats[i] / TRAIN_COLS][selectedSeats[i] % TRAIN_COLS].status = "X";
//			train->seats[selectedSeats[i] / TRAIN_COLS][selectedSeats[i] % TRAIN_COLS].serverId = res.serverId;
//			train->seats[selectedSeats[i] / TRAIN_COLS][selectedSeats[i] % TRAIN_COLS].ticketId = res.ticketId;
	}

	for (i = 0; i < TRAIN_ROWS; i++) {
		for (int j = 0; j < TRAIN_COLS; j++) {
			printf("%s\t", train->seats[i][j].status);
		}
		printf("\n");
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

	//LOCK THE OTHER THREADS
	pthread_mutex_lock(&resLock);

	//TODO: update the Train
	train->availableSeats -= ticketsRequested;

	//TODO: Create the Reservation File
	struct Reservation *res = malloc(sizeof(struct Reservation) + sizeof(struct Ticket) * ticketsRequested);
	res->numTickets = ticketsRequested;
	res->updateDate = dateRequested;
	res->serverId = 100 + rand() % (1000 - 100 + 1);
//	res->number = ???;

//TODO: Create Ticket Records.

	for (i = 0; i < ticketsRequested; i++) {
		struct Ticket *t = malloc(sizeof(struct Ticket) + sizeof(struct Customer));
		//TODO: we should probably get this from the ticket file?
		t[i].id = 10000 + rand() % (100000 - 10000 + 1);
		t[i].customer = customers[i];
		t[i].seatNumber = 2; //TODO: How to calc this?
		t[i].travelDate = dateRequested;
		t[i].updateDate = "create";
	}

	//TODO: Return the Reservation Data
	writeSummary(res, train, selectedSeats);
//	updateseats(t,"train",selectedSeats ,res->numTickets ,false );
	//send(connectionFd, {message about the reservation / reciept}, strlen("continue"), 0);

	pthread_mutex_unlock(&resLock);
}

/**
 *
 */
void inquireTicket(int connectionFd) {
	printf("Inquire about Ticket");
	char client_message[2000];
	char *server_response;
	char ticketNum[SMALL_BUFFER];
	FILE *fp;
	DIR *d;
	struct dirent *dir;
	ssize_t r;
	char *line = NULL;
	size_t len = 0;
	int flag = 0;

	server_response = "What is the ticket number you would like to inquire about:\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);
	strcpy(ticketNum, client_message);

	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strstr(dir->d_name, "Summary")) {
				fp = fopen(dir->d_name, "r");
				while ((r = getline(&line, &len, fp)) != -1) {
					if (strstr(line, ticketNum)) {
						send(connectionFd, line, strlen(line) + 1, 0);
						flag = 1;
						break;
					}
				}
				fclose(fp);

				if (flag == 1)
					break;
			}
		}
		if (flag == 0)
			send(connectionFd, "not found", 10, 0);
		closedir(d);
	}

	//TODO: Return the Ticket Data
	server_response = "Ticket Number 123";
	send(connectionFd, server_response, strlen(server_response), 0);
}

/**
 * Find the reservation,
 */
void modifyReservation(int connectionFd) {
	printf("Modify Reservation");
	char *reservation_ID="" ;
	char *NewSeat="";
	char *NewDate="";
	char *NewnumberOfTravelers ="";
	char resNumber[SMALL_BUFFER];
	char fileName[SMALL_BUFFER];
	char tmpBuffer[BUFFER];
	char s[2] = " ";
	char *token;
	char *tmp;
	FILE *fp1;
	FILE *fp2;
	DIR *d;
	struct dirent *dir;
	ssize_t r;
	char *line = NULL;
	size_t len = 0;
	int flag = 0;
	int tmpFlag = 0;
	int count;
	int cancelledSeats[TRAIN_ROWS * TRAIN_COLS];
	int i;
	int j;

	char client_message[2000];

	/* Check which res the costomer want to modify it */
	char *server_response;
	server_response = "What is the reservation number you would like to modify:\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);

	send(connectionFd, "continue", strlen("continue"), 0);

	strcpy(reservation_ID, client_message);

	/* check if the customer need to modify the seat */
	server_response = "do you want to modify the seat:\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);

	if (strcmp(client_message, "yes") == 0) {
		/* if yes requiest from the customer the new seat*/
		server_response = "Please chose the new seat:\n";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(NewSeat, client_message);
	}

	/* check if the customer need to modify the date */
	server_response = "do you want to modify the date:\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);

	if (strcmp(client_message, "yes") == 0) {
		/* if yes rrequest the new date*/
		server_response = "Please chose the new date:\n";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(NewDate, client_message);
	}

	/* check if the customer need to change the number of travelers */
	server_response = "do you want to modify the number of travelers:\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);

	if (strcmp(client_message, "yes") == 0) {
		/* if yes request the new number of travelers */
		server_response = "Please chose the new number of travelers:\n";
		send(connectionFd, server_response, strlen(server_response), 0);
		recv(connectionFd, client_message, 2000, 0);
		strcpy(NewnumberOfTravelers, client_message);
	}

	/* cancel the Old reservation */

	strcpy(resNumber,reservation_ID);

	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) {

			/* looking for the summary file in the folder */
			if (strstr(dir->d_name, "Summary")) {
				fp1 = fopen(dir->d_name, "r");
				fp2 = fopen("temp.txt", "w");
				while ((r = getline(&line, &len, fp1)) != -1) {

					/* look for the reservation number at the selected line in summary file */
					if (strstr(line, resNumber)) {
						// update file
						strcpy(tmpBuffer, line);

						send(connectionFd, "cancelled", 10, 0);
						flag = 1;
					} else {
						fprintf(fp2, "%s", line);
					}
				}
				remove(dir->d_name);
				rename("temp.txt", dir->d_name);
				fclose(fp1);
				fclose(fp2);
				if (flag == 1) {
					strcpy(fileName, dir->d_name);
					tmp = strchr(fileName, 'S');
					if (tmp) {
						fileName[tmp - fileName] = '\0';
						fp1 = fopen(fileName, "r");
						fp2 = fopen("tmp.txt", "w");

						token = strtok(tmpBuffer, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						if (strchr(token, '/') == NULL) {
							token = strtok(NULL, s);
						}
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						count = atoi(token);
						printf("count is %d\n", count);
						r = getline(&line, &len, fp1);
						printf("atoi is is %d\n", atoi(line));
						fprintf(fp2, "%d\n", atoi(line) + count);
						token = strtok(NULL, s);
						i = 0;
						while (token != NULL) {
							cancelledSeats[i++] = atoi(token);
							token = strtok(NULL, s);
						}
						i = 0;
						while ((r = getline(&line, &len, fp1)) != -1) {

							for (j = 0, tmpFlag = 0; j < count; j++) {
								if (cancelledSeats[j] == i) {
									printf("22\n");
									fprintf(fp2, "=\n");
									tmpFlag = 1;
									break;
								}
							}
							i++;
							if (tmpFlag == 0)
								fprintf(fp2, "%s", line);
						}
						fclose(fp1);
						fclose(fp2);
						remove(fileName);
						rename("tmp.txt", fileName);
					}
					break;
				}
			}
		}
		if (flag == 0)
			send(connectionFd, "not found", 10, 0);
		closedir(d);
	}


	/* make new reservation*/
	int numtickets = atoi(NewnumberOfTravelers);

	struct Reservation *res = malloc(sizeof(struct Reservation) + (sizeof(struct Ticket) * numtickets));
	res->numTickets = numtickets;
	res->updateDate = NewDate;
	res->serverId = 100 + rand() % (1000 - 100 + 1);
//	updateseats(t,"train",NewSeat ,res->numTickets ,false );

}

/**
 *
 */
void cancelReservation(int connectionFd) {
	printf("Cancel Reservation");

	char client_message[2000];
	char *server_response;
	char resNumber[SMALL_BUFFER];
	char fileName[SMALL_BUFFER];
	char tmpBuffer[BUFFER];
	char s[2] = " ";
	char *token;
	char *tmp;
	FILE *fp1;
	FILE *fp2;
	DIR *d;
	struct dirent *dir;
	ssize_t r;
	char *line = NULL;
	size_t len = 0;
	int flag = 0;
	int tmpFlag = 0;
	int count;
	int cancelledSeats[TRAIN_ROWS * TRAIN_COLS];
	int i;
	int j;

	server_response = "What is the reservation number you would like to cancel:\n";
	send(connectionFd, server_response, strlen(server_response), 0);
	recv(connectionFd, client_message, 2000, 0);
	strcpy(resNumber, client_message);

	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strstr(dir->d_name, "Summary")) {
				fp1 = fopen(dir->d_name, "r");
				fp2 = fopen("temp.txt", "w");
				while ((r = getline(&line, &len, fp1)) != -1) {
					if (strstr(line, resNumber)) {
						// update file
						strcpy(tmpBuffer, line);

						send(connectionFd, "cancelled", 10, 0);
						flag = 1;
					} else {
						fprintf(fp2, "%s", line);
					}
				}
				remove(dir->d_name);
				rename("temp.txt", dir->d_name);
				fclose(fp1);
				fclose(fp2);
				if (flag == 1) {
					strcpy(fileName, dir->d_name);
					tmp = strchr(fileName, 'S');
					if (tmp) {
						fileName[tmp - fileName] = '\0';
						fp1 = fopen(fileName, "r");
						fp2 = fopen("tmp.txt", "w");

						token = strtok(tmpBuffer, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						if (strchr(token, '/') == NULL) {
							token = strtok(NULL, s);
						}
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						token = strtok(NULL, s);
						count = atoi(token);
						printf("count is %d\n", count);
						r = getline(&line, &len, fp1);
						printf("atoi is is %d\n", atoi(line));
						fprintf(fp2, "%d\n", atoi(line) + count);
						token = strtok(NULL, s);
						i = 0;
						while (token != NULL) {
							cancelledSeats[i++] = atoi(token);
							token = strtok(NULL, s);
						}
						i = 0;
						while ((r = getline(&line, &len, fp1)) != -1) {

							for (j = 0, tmpFlag = 0; j < count; j++) {
								if (cancelledSeats[j] == i) {
									printf("22\n");
									fprintf(fp2, "=\n");
									tmpFlag = 1;
									break;
								}
							}
							i++;
							if (tmpFlag == 0)
								fprintf(fp2, "%s", line);
						}
						fclose(fp1);
						fclose(fp2);
						remove(fileName);
						rename("tmp.txt", fileName);
					}
					break;
				}
			}
		}
		if (flag == 0)
			send(connectionFd, "not found", 10, 0);
		closedir(d);
	}
//	updateseats(t,"train",cancelledSeats,count,true);
}

/**
 *
 */
void writeSummary(struct Reservation *res, struct Train *t, int *selectedSeats) {
	printf("Write Summary");
	char *server_response[200];
	char server_ID[10];
	char reservation_Date[10];
	char numberOfTicket[10];
	char train_ID[10];
	char Selected_Seat[10];
	char toStr[10];
	FILE *fp1;
	struct dirent *dir;
	DIR *d;

 	/* Convert all data from init to string using itoa function */

	strcpy(reservation_Date,res->updateDate);

	snprintf(server_ID, "%d", &res->serverId);
//	itoa(res->serverId, server_ID, 10);

	snprintf(numberOfTicket, "%d", &res->numTickets);
//	itoa(res->numTickets, numberOfTicket, 10);

	snprintf(train_ID, "%d", &t->id);
//	itoa(t->id, train_ID, 10);

	snprintf(Selected_Seat, "%d", selectedSeats);
//	itoa(selectedSeats, Selected_Seat, 10);

	/* concatenate all strings in one signl strings */

	strcat(server_response,"Reservation id :");
	strcat(server_response,server_ID);
	strcat(server_response,"   ");
	strcat(server_response,"Reservation date:");
	strcat(server_response,reservation_Date);
	strcat(server_response,"   ");
	strcat(server_response,"train ID:");
	strcat(server_response,train_ID);
	strcat(server_response,"   ");
	strcat(server_response,"number Of Trvelers:");
	strcat(server_response,numberOfTicket);
	strcat(server_response,"   ");
	strcat(server_response,"Selected Seat:");
	strcat(server_response,Selected_Seat);

	/* look for summary file, then open it and add the new summary line*/
	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strstr(dir->d_name, "Summary")) {
				fp1 = fopen(dir->d_name, "w");
				fprintf(fp1, "%s", server_response);
				break;
			}
		}
	}

	/* close file and the directory */
	fclose(fp1);
	closedir(d);
}

/**
 *
 */
void readSeats(struct Train *t, char *fileName) {
	printf("Create Reservation");
	FILE *fp1;
	struct dirent *dir;
	DIR *d;
	ssize_t r;
	char *line = NULL;
	size_t len = 0;
	char *ptr= NULL;
	int i= 0;
	int j= 0;
	/* Look for seats file using the fileName and open it */
	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strstr(dir->d_name, fileName)) {
				fp1 = fopen(dir->d_name, "r");
				break;
			}
		}
	}
	if (fp1!=NULL)
	{
		/* read all lines and split each one using the strtok function*/
		while((r = getline(&line, &len, fp1)) != -1)
		{
			ptr = strtok(line, " ");
			while(ptr!=NULL)
			{
				/* read and set the status of each seat*/
				t->seats[i][j].status=ptr;
				i++;
				ptr = strtok(NULL, " ");
			}
			j++;
		}
	}
	/* close file and directory */
	fclose(fp1);
	closedir(d);
}

/**
 *
 */
//void updateseats(struct Train *t, char *fileName, int selectedSeats, int numberofseat, bool ToBeCancled)
//{
//	FILE *fp1;
//	struct dirent *dir;
//	DIR *d;
//	ssize_t r;
//	char *line = NULL;
//	size_t len = 0;
//	char *ptr= NULL;
//	int i= 0;
//	int j= 0;
//	int Rows[numberofseat];
//	int Cloms[numberofseat];
//	int Seats_[TRAIN_ROWS][TRAIN_COLS];
//	char *seatToWrite[200];
//	/* Look for seats file using the fileName and open it */
//	d = opendir(".");
//	if (d) {
//		while ((dir = readdir(d)) != NULL) {
//			if (strstr(dir->d_name, fileName)) {
//				fp1 = fopen(dir->d_name, "w");
//				break;
//			}
//		}
//	}
//
//
//	if (fp1!=NULL)
//	{
//		/* read all lines and split each one using the strtok function*/
//		while((r = getline(&line, &len, fp1)) != -1)
//		{
//			ptr = strtok(line, " ");
//			while(ptr!=NULL)
//			{
//				/* read and set the status of each seat and put it in the Seats matrix*/
//				t->seats[i][j].status=ptr;
//				Seats_[j][i]=ptr;
//				i++;
//				ptr = strtok(NULL, " ");
//			}
//			j++;
//		}
//	}
//	/* look for the position of each selected seat in the train*/
//	for( int k=0 ; k<numberofseat ; k++)
//	{
//		Rows=selectedSeats[k]/3;
//		Cloms=selectedSeats[k]%3;
//		if(ToBeCancled)
//		{
//			Seats_[Rows][Cloms]="O";
//		}
//		else
//		{
//			Seats_[Rows][Cloms]="X";
//		}
//	}
//	/* close file and directory */
//	fclose(fp1);
//	remove(fileName);
//	/* cleate the new train file with the new seaat position */
//	fp1 = fopen(fileName, "w");
//	for (i=0 ; i<TRAIN_ROWS ; i++)
//	{
//		seatToWrite="";
//		for(j=0 ; j<TRAIN_COLS; j++)
//		{
//			strcat(seatToWrite,Seats_[i][j]);
//			t->seats[i][j].status = Seats_[i][j];
//		}
//		strcat(seatToWrite,"\n");
//	}
//
//	fprintf(fp1, "%s", seatToWrite);
//	fclose(fp1);
//	closedir(d);
//}
