/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson
 Email		 : jjohn84@ostatemail.okstate.edu,
 Date		 : 4/17/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "serverHeader.h"

/**
 *
 */
static int initThread(struct tpool *thpool_p, struct thread **thread_p, int id) {

	*thread_p = (struct thread*) malloc(sizeof(struct thread));
	if (*thread_p == NULL) {
		perror("Could not allocate memory for thread\n");
		return -1;
	}

	(*thread_p)->thpool_p = thpool_p;
	(*thread_p)->id = id;

	pthread_create(&(*thread_p)->pthread, NULL, (void* (*)(void*)) serverThread, (*thread_p));
	pthread_detach((*thread_p)->pthread);
	return 0;
}

/**
 *
 */
struct tpool* initThreadPool(int num_threads) {

	struct tpool *pool;
	pool = (struct tpool*) malloc(sizeof(struct tpool));
	if (pool == NULL) {
		perror("Could not allocate memory for thread pool\n");
		return NULL;
	}

	pool->threadsAlive = num_threads;

	pool->threads = (struct thread**) malloc(num_threads * sizeof(struct thread*));

	pthread_mutex_init(&(pool->threadCountLock), NULL);

	/* Thread init */
	int n;
	for (n = 0; n < num_threads; n++) {
		initThread(pool, &pool->threads[n], n);
	}

	/* Wait for threads to initialize */
	while (pool->threadsAlive != num_threads) {
	}

	return pool;
}
