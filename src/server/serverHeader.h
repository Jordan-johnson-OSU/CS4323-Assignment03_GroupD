/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson, Mohammad Musaqlab
 Email		 : jjohn84@ostatemail.okstate.edu, Mohammed.Musaqlab@okstate.edu
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */

#ifndef SRC_SERVER_HEADER_H_
#define SRC_SERVER_HEADER_H_

char *strtrim(char const *str);

struct Customer {
	int id;
	char *name;
	char *dateOfBirth;
	char *gender;
	char *governmentId;
};

struct Seat {
	int id;
	int serverId;
	char *updateDate;
	char *status; //Open, Reserved
};

struct Train {
	int id;
	int availableSeats;
	struct Seat **seats;
};


struct Ticket {
	int id;
	int serverId;
	int seatNumber;
	char *updateDate;
	char *travelDate;
	struct Customer customer;
};

struct Reservation {
	int id;
	int serverId;
	char *updateDate;
	int numTickets;
	struct Ticket *tickets;
};


void* threadMonitor (void *arg);
void* serverThread(void *arg);

void createReservation(int connectionFd);
void inquireReservation(int connectionFd);
void modifyReservation(int connectionFd);
void cancelReservation(int connectionFd);
void writeSummary(struct Reservation res, struct Train t, int *selectedSeats);
void readSeats(struct Train *t, char *fileName);

#endif /* SRC_SERVER_HEADER_H_ */
