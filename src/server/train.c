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
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <semaphore.h>

#include "serverHeader.h"

/**
 *  Initialize the Train file into memory to be used by the program.
 *
 *  Parameters:
 *  	struct Train *train -> already allocated memory space where we are going to update the train information from the file.
 *  	char *nameFile -> specific train file on the server.
 *
 *  Return:
 *  	1 = failure
 *  	0 = success;
 */
int initTrain(struct Train *train, char *nameFile) {
	printf("initTrain - |%s|\n", nameFile);

	//TODO: Load the File into shared Memory Shared Memory
//	const char *name = nameFile;
//	int smloc = shm_open(nameFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
//	ftruncate(smloc, 4096);

//	sem_t *semTReader = sem_open(SEM_TRAIN_READER, O_CREAT, 0660, 1);
//	if (semTReader == SEM_FAILED) {
//		printf("\tSemaphore failed to open.");
//		return EXIT_FAILURE;
//	}

//Open the train file
	int fd = open(nameFile, O_RDWR, S_IRUSR | S_IWUSR);

	struct stat sb;

	if (fstat(fd, &sb) == -1) {
		perror("couldn't find file size");
		return 1;
	}

	//Map the file to memory
	void *train_mmap = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	//Open some shared memory
	int smloc = shm_open(SHM_TRAIN, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	ftruncate(smloc, sb.st_size);

	for (int i = 0; i < sb.st_size; i++) {
		printf("%c", ((char*) train_mmap)[i]);
	}

	//TODO: how do we use the shared memory instead of just reading the file?
//	Copy the mmap file to the shared memory.
//	memcpy(smloc, train_mmap, sb.st_size);

	printf("\n");

	close(fd);

//	struct Seat seats[TRAIN_ROWS][TRAIN_COLS]; // = malloc((sizeof(struct Seat)*TRAIN_ROWS*TRAIN_COLS));

//	for (int i = 0; i < sb.st_size; i++) {
//		//pull new line from each file
//		train->seats[i][1].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//		train->seats[i][2].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//		train->seats[i][3].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//		train->seats[i][4].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//	}
//
//	train->seats = seats;
//	printf("printing train seats\n");
//	for (i = 0; i < TRAIN_ROWS; i++) {
//		printf("%s %s %s %s\n",train->seats[i][1].status,train->seats[i][2].status,train->seats[i][3].status,train->seats[i][4].status);
//	}

	FILE *file = fopen(nameFile, "r");

	if (file == NULL) {
		perror("File is null");
		exit(EXIT_FAILURE);
	}

	char chunk[256];
	size_t len = sizeof(chunk);
	char *line = malloc(len);

	if (line == NULL) {
		perror("Unable to allocate memory for the line buffer.");
		exit(EXIT_FAILURE);
	}

	//pull new line from each file
	fscanf(file, "%[^\n]", line);
	fgetc(file);

	train->availableSeats = atoi(line);

	for (int i = 0; i < TRAIN_ROWS; i++) {
//		printf("Loading Seat: %d", i);
		//pull new line from each file
		fscanf(file, "%[^\n]", line);
		fgetc(file);
		int j = 0;
		train->seats[i][j].status = strtok(line, " ");
		j++;
		train->seats[i][j].status = strtok(NULL, " ");
		j++;
		train->seats[i][j].status = strtok(NULL, " ");
		j++;
		train->seats[i][j].status = strtok(NULL, "\r\n");
	}

	// close file
	fclose(file);
	printf("initTrain Finish\n");
	return 0;
}

/**
 * Update Train, updates the train file on the server implementing a semaphore to lock.  It has a rudimentary algorithm of comparing states to see if the someone else has updated the train since last checked.
 *
 * Parameters:
 * 		struct Train *train -> updated train record with new seats reserved, or cancelled.
 *		char *nameFile -> name of the train file on the server.
 *
 * return
 * 		1 = failure updating train due to sync issues.
 * 		0 = successful
 */
int updateTrain(struct Train *train, char *nameFile) {
	printf("updateTrain - |%s|\n", nameFile);

	sem_t *semTWriter = sem_open(SEM_TRAIN_WRITER, O_CREAT, 0660, 1);
	if (semTWriter == SEM_FAILED) {
		printf("\tSemaphore failed to open.");
		return EXIT_FAILURE;
	}

	int errors = 0;

	//Lock while we are writing the record.
	sem_wait(semTWriter);

	/**
	 * Read in the file again.
	 * Compare the whole record to see if the difference of number of available seats matches to the swaps of X's and O's
	 */
	struct Train *oldTrain = malloc(sizeof(struct Train) + (sizeof(struct Seat) * TRAIN_ROWS * TRAIN_COLS));
	initTrain(oldTrain, nameFile);

	printf("train->availableSeats %d\n", train->availableSeats);
	printf("oldTrain->availableSeats %d\n", oldTrain->availableSeats);

	int seatDiff = train->availableSeats - oldTrain->availableSeats;
	printf("seatDiff %d\n", seatDiff);

	if (seatDiff < 0) {
		printf("seatDiff < 0");
		//Adding a Reservation
		for (int i = 0; i < TRAIN_ROWS; i++) {
			for (int j = 0; j < TRAIN_COLS; j++) {
				char *newStatus = train->seats[i][j].status;
				char *oldStatus = oldTrain->seats[i][j].status;

				printf("\ntrain->seat[%d][%d] = %s", i, j, newStatus);
				printf(" = %s", oldStatus);

				if (strcmp(newStatus, oldStatus) == 0) {

				} else {
					seatDiff++;
					printf(" +1 ");
				}
			}
		}
		//If things don't balance out, there are issues here and we are not going to write the file.
		if (seatDiff != 0) {
			errors = 1;
		}
	} else if (seatDiff > 0) {
		printf("seatDiff > 0");
		//Canceling a Reservation
		for (int i = 0; i < TRAIN_ROWS; i++) {
			for (int j = 0; j < TRAIN_COLS; j++) {
				char *newStatus = train->seats[i][j].status;
				char *oldStatus = oldTrain->seats[i][j].status;

				printf("\ntrain->seat[%d][%d] = %s", i, j, newStatus);
				printf(" = %s", oldStatus);

				if (strcmp(newStatus, oldStatus) == 0) {

				} else {
					seatDiff--;
					printf(" -1 ");
				}
			}
		}
		//If things don't balance out, there are issues here and we are not going to write the file.
		if (seatDiff != 0) {
			errors = 1;
		}
	} else {
		printf("seatDiff == 0");
	}

	printf("\nseatDiff %d\n", seatDiff);

	//If no errors, write the files, otherwise return with an error.
	if (errors == 0) {
		printf("Files in sync....lets write it.\n");
		FILE *trainFile = fopen(nameFile, "w");
		//Write the new stuff?
		fprintf(trainFile, "%d\n", train->availableSeats);

		int i;
		for (i = 0; i < TRAIN_ROWS; i++) {
			fprintf(trainFile, "%s %s %s %s\n", train->seats[i][0].status, train->seats[i][1].status,
					train->seats[i][2].status, train->seats[i][3].status);
		}
		fclose(trainFile);
		//post to the semaphore to unlock for someone else.
		sem_post(semTWriter);

		initTrain(train, nameFile);

		printf("updateTrain Finished!\n");
		return 0;
	} else {
		printf("The train file was out of synch so unsuccessful.\n");
		sem_post(semTWriter);

		initTrain(train, nameFile);
		return 1;		//reload the train to try to reserve new seats.
	}
}
