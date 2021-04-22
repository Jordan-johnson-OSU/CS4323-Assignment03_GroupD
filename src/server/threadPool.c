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

qItem *head = NULL;
qItem *tail = NULL;


/**
 * Queue Implementation
 */
void putQueue(int *clientSocket) {
	qItem *newItem = malloc(sizeof(qItem));
	newItem->next = NULL;
	newItem->clientSocket = clientSocket;

	if (tail == NULL) {
		head = newItem;
	} else {
		tail->next = newItem;
	}
	tail = newItem;
}

/**
 * Removes the first item from Queue;
 */
int* readQueue() {
	if (head == NULL) {
		return NULL;
	} else {
		int *clientSocket = head->clientSocket;
		//Free up space on the queue;
		qItem *temp = head;
		head = head->next;
		if (head == NULL) {
			tail = NULL;
		}
		free(temp);

		return clientSocket;
	}
}

/**
 *
 */
static int initThread(struct tpool *pool, struct thread **thread, int id) {

	*thread = (struct thread*) malloc(sizeof(struct thread));
	if (*thread == NULL) {
		perror("Could not allocate memory for thread\n");
		return -1;
	}

	(*thread)->thpool_p = pool;
	(*thread)->id = id;

	pthread_mutex_init(&(pool->threadCountLock), NULL);
	pthread_cond_init(&(pool->threadIdle), NULL);

	pthread_create(&(*thread)->pthread, NULL, (void* (*)(void*)) serverThread, (*thread));
	pthread_detach((*thread)->pthread);
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
