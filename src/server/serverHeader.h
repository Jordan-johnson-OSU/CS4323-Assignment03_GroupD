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

#define SEM_TRAIN_READER "/trainread"
#define SEM_TRAIN_WRITER "/trainwrite"

#define SHM_TRAIN "/trainSHM"

struct Customer {
	int id;
	char *name;
	char *dateOfBirth;
	char *gender;
	char *governmentId;
};

struct Seat {
	int id;
	char *status; //Open, Reserved, or O / X
};

struct Train {
	int id;
	int availableSeats;
	struct Seat seats[TRAIN_ROWS][TRAIN_COLS];
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
	pthread_cond_t  threadIdle;
};

struct queueItem {
	struct queueItem* next;
	int *clientSocket;
};
typedef struct queueItem qItem;

void* threadMonitor (void *arg);
void* monitorThread(void *arg);
void serverThread(void *arg);

void putQueue(int *clientSocket);
int* readQueue();

int initTrain(struct Train *train, char *nameFile);
int updateTrain(struct Train *train, char *nameFile);

void createReservationJordan(int connectionFd);
void writeSummaryJordan(struct Reservation *res, struct Train *t);

void createReservation(int connectionFd);
void inquireTicket(int connectionFd);
void modifyReservation(int connectionFd);
void cancelReservation(int connectionFd);
void writeSummary(struct Reservation *res, struct Train *t, int selectedSeats);
void readSeats(struct Train *t, char *fileName);

#endif /* SRC_SERVER_HEADER_H_ */
