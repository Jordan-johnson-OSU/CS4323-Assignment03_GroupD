/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson
 Email		 : jjohn84@ostatemail.okstate.edu
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :  Main client program which connects to a server via the supplied port and defaults to 9090.  This is a shell to have multiple client threads if needed for testing.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

#include "clientHeader.h"

/**
 * Arguments:
 * 		argc - count of input arguments to your program.
 * 		argv - pointer to all the input arguments.
 *
 * 		arg[1] - Port to start the server
 *
 * Return:
 *		0 for Success.
 *
 */
int main(int argc, char **argv) {
	int i = 0;
	pthread_t tid[51];
	while (i < 1) {
		if (pthread_create(&tid[i], NULL, clientThread, argv[1]) != 0)
			printf("Failed to create thread\n");
		i++;
	}
	sleep(5);
	i = 0;
	while (i < 1) {
		pthread_join(tid[i++], NULL);
		printf("Thread Finished: %d\n", i);
	}
	printf("Client Main End\n");
	return 0;
}
