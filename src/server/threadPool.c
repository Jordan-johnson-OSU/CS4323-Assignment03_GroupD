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
