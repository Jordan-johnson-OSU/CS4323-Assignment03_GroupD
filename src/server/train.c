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
 *
 */
int initTrain(struct Train *train, char *nameFile) {
	printf("initTrain\n");

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
	}

	//Map the file to memory
	void *train_mmap = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	//Open some shared memory
	int smloc = shm_open(SHM_TRAIN, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	ftruncate(smloc, sb.st_size);

	for (int i = 0; i < sb.st_size; i++) {
		printf("%c", ((char*) train_mmap)[i]);
	}

	//Copy the mmap file to the shared memory.
//	memcpy(smloc, train_mmap, sb.st_size);

	printf("\n");

	close(fd);

	struct Seat seats[TRAIN_ROWS][TRAIN_COLS]; // = malloc((sizeof(struct Seat)*TRAIN_ROWS*TRAIN_COLS));

	int i = 0;
//	for (i = 0; i < sb.st_size; i++) {
//		//pull new line from each file
//		seats[i][1].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//		seats[i][2].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//		seats[i][3].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//		seats[i][4].status = ((char*) train_mmap)[i];
//		i++;
//		i++;
//	}
//
//	train->seats = seats;
	printf("printing train seats\n");
//	for (i = 0; i < TRAIN_ROWS; i++) {
//		printf("%s %s %s %s\n",train->seats[i][1].status,train->seats[i][2].status,train->seats[i][3].status,train->seats[i][4].status);
//	}

	//TODO: how do we use the shared memory instead of just reading the file?

	FILE *file = fopen(nameFile, "rw");

	if (file == NULL) {
		perror("File is null");
		exit(EXIT_FAILURE);
	}

	char chunk[256];
	size_t len = sizeof(chunk);
	char *line = malloc(len);

	if (line == NULL) {
		perror("Unable to allocate memory for the line buffer.");
		exit(1);
	}

	//pull new line from each file
	fscanf(file, "%[^\n]", line);
	fgetc(file);
	train->availableSeats = atoi(line);

//	struct Seat seats[TRAIN_ROWS][TRAIN_COLS]; // = malloc((sizeof(struct Seat)*TRAIN_ROWS*TRAIN_COLS));

//	int i = 0;
	for (i = 0; i <= TRAIN_ROWS; i++) {
		//pull new line from each file
		fscanf(file, "%[^\n]", line);
		fgetc(file);
		seats[i][1].status = strtok(line, " ");
		seats[i][2].status = strtok(NULL, " ");
		seats[i][3].status = strtok(NULL, " ");
		seats[i][4].status = strtok(NULL, "\r\n");
	}

	train->seats = seats;

	// close file
	fclose(file);
	printf("initTrain Finish\n");
	return 0;
}

/**
 * TODO: Is there a way to check if the train was just updated by another client?
 *
 *
 */
int updateTrain(struct Train *train, char *nameFile) {
	printf("updateTrain");

	sem_t *semTWriter = sem_open(SEM_TRAIN_WRITER, O_CREAT, 0660, 0);
	if (semTWriter == SEM_FAILED) {
		printf("\tSemaphore failed to open.");
		return EXIT_FAILURE;
	}

	//TODO: Lock
	sem_wait(&semTWriter);

	//TODO: Read and Check for new Reservations?

	FILE *trainFile = fopen(nameFile, "w+");

	fprintf(trainFile, "%d\n", train->availableSeats);

	int i;
	for (i = 0; i < 20; i++) {
		fprintf(trainFile, "%s %s %s %s\n", train->seats[i][1].status, train->seats[i][2].status,
				train->seats[i][3].status, train->seats[i][4].status);
	}

	fclose(trainFile);

	//TODO: Unlock
	sem_post(&semTWriter);
	printf("updateTrain Finish");
	return 0;
}
