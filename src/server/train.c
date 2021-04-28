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

#include "serverHeader.h"

static pthread_mutex_t trainLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t trainCond = PTHREAD_COND_INITIALIZER;

/**
 *
 */
int initTrain(struct Train *train, char *nameFile) {
	printf("initTrain");
	FILE *file = fopen(nameFile, "rw");
	struct stat sb;

	if(fstat(file, &sb) == -1){
		perror("couldn't find file size");
	}

	if (file == NULL) {
		perror("File is null");
		exit(EXIT_FAILURE);
	}

	char *train_mmap = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);

	for(int i=0; i< sb.st_size; i++){
		printf("%c",train_mmap[i]);
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

	struct Seat seats[TRAIN_ROWS][TRAIN_COLS];// = malloc((sizeof(struct Seat)*TRAIN_ROWS*TRAIN_COLS));

	int i = 0;
	for (i = 0; i < TRAIN_ROWS; i++) {
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
	printf("initTrain Finish");
	return 0;
}

/**
 * TODO: Is there a way to check if the train was just updated by another client?
 *
 *
 */
int updateTrain(struct Train *train, char *nameFile) {
	printf("updateTrain");
	//TODO: Lock
	pthread_mutex_lock(&trainLock);

	//TODO: Read and Check for new Reservations?

    FILE *trainFile = fopen(nameFile,"w+");

    fprintf(trainFile, "%d\n", train->availableSeats);

    int i;
    for(i = 0; i < 20; i++) {
    	fprintf(trainFile, "%s %s %s %s\n", train->seats[i][1].status, train->seats[i][2].status, train->seats[i][3].status, train->seats[i][4].status );
    }

    fclose(trainFile);

    //TODO: Unlock
    pthread_mutex_unlock(&trainLock);
    printf("updateTrain Finish");
	return 0;
}
