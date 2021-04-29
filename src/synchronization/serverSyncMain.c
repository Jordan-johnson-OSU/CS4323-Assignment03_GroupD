/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson
 Email		 : jjohn84@ostatemail.okstate.edu,
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "serverSyncHeader.h"


/**
 * Arguments:
 * 		argc - count of input arguments to your program.
 * 		argv - pointer to all the input arguments.
 *
 * 		arg[1] - Port to start the server
 * 		arg[2] - thread count for the server
 *
 * Return:
 *		0 for Success.
 *
 */
int main(int argc, char **argv) {

	//Semaphore initialization
	sem_unlink(SEM_TRAIN_READER);
	sem_unlink(SEM_TRAIN_WRITER);

	sem_t *semTReader = sem_open(SEM_TRAIN_READER, O_CREAT, 0660, 1);
	if (semTReader == SEM_FAILED) {
		printf("\tSemaphore failed to open.");
		return EXIT_FAILURE;
	}


	sem_t *semTWriter = sem_open(SEM_TRAIN_WRITER, O_CREAT, 0660, 0);
	if (semTWriter == SEM_FAILED) {
		printf("\tSemaphore failed to open.");
		return EXIT_FAILURE;
	}

	int val;
	sem_getvalue(semTReader, &val);
	printf("Train Reader: %d\n", val);
	sem_getvalue(semTWriter, &val);
	printf("Train Writer: %d\n", val);

	while (1) {
		char command[10];
		printf("Enter 'shutdown' to stop the serverSyncronizer, otherwise hold: ");
		fgets(command, 10, stdin);
		if(strcmp("shutdown",command)==0){
			break;
		}
	}

	sem_close(SEM_TRAIN_READER);
	sem_close(SEM_TRAIN_WRITER);

	return 0;
}
