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

#define TRAIN_ROWS 20
#define TRAIN_COLS 4
#define BUFFER 1024
#define SMALL_BUFFER 100

struct Customer {
	int id;
	char *name;
	char *dateOfBirth;
	char *gender;
	char *governmentId;
};

struct Seat {
	int id;
//	int serverId;
//	char *updateDate;
	char *status; //Open, Reserved, or O / X
};

struct Train {
	int id;
	int availableSeats;
	struct Seat **seats;
};

struct Ticket {
	int id; //Generated
	int seatNumber;
	char *updateDate;
	char *travelDate;
	struct Customer customer;
};

struct Reservation {
	int number; //Generated
	int serverId;
	char *updateDate;
	int numTickets;
	struct Ticket *tickets;
};

struct thread {
	int id;
	pthread_t pthread;
	struct tpool *thpool_p;
};

struct tpool {
	int total_threads;
	struct thread *threads;
	volatile int threadsAlive;
	pthread_mutex_t threadCountLock;
//	pthread_cond_t  threadsAllIdle;
};

void* threadMonitor (void *arg);
void* serverThread(void *arg);

struct tpool* initThreadPool(int num_threads);

int initTrain(struct Train *train, char *nameFile);

void createReservation(int connectionFd);
void inquireTicket(int connectionFd);
void modifyReservation(int connectionFd);
void cancelReservation(int connectionFd);
void writeSummary(struct Reservation *res, struct Train *t, int *selectedSeats);
void readSeats(struct Train *t, char *fileName);

#endif /* SRC_SERVER_HEADER_H_ */
