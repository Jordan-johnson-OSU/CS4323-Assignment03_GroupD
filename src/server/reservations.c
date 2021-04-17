/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Mohammad Musaqlab
 Email		 : Mohammed.Musaqlab@okstate.edu
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "serverHeader.h"

void createReservation(int connectionFd) {
	struct customerInfo c;
	char *yes = "yes";
	char *no = "no";
	struct Reservation res;
	struct Train t; // read from file
	int selectedSeats[ROWS * COLS];
	int i;

	//TODO: we can't get this info from the client like this.
	recv(connectionFd, &c, sizeof(struct customerInfo), 0);

	readSeats(&t, c.dateTravel.string);
	if(c.numOfTravellers < t.availableSeats) {
    	send(connectionFd, yes, strlen(yes) + 1, 0);
    	send(connectionFd, &t, sizeof(t), 0);
    	recv(connectionFd, selectedSeats, sizeof(int) * ROWS * COLS, 0);

    	t.availableSeats -= c.numOfTravellers;

    	res.modified = 0;
    	res.serverId = 100 + rand() % (1000 - 100 + 1);
    	res.ticketId = 10000 + rand() % (100000 - 10000 + 1); // generate random
    	res.c = c;

    	for(i = 0; i < c.numOfTravellers; i++) {
        	t.s[selectedSeats[i] / COLS][selectedSeats[i] % COLS].status = 'X';

        	t.s[selectedSeats[i] / COLS][selectedSeats[i] % COLS].serverId = res.serverId;
        	t.s[selectedSeats[i] / COLS][selectedSeats[i] % COLS].ticketId = res.ticketId;
    	}

        	for(i = 0; i < ROWS; i++) {
            	for(int j = 0; j < COLS; j++) {
          	      printf("%c\t", t.s[i][j].status);
            	}
            	printf("\n");
        	}
    	send(connectionFd, &res, sizeof(res), 0); // sending receipt to customer
    	writeSummary(res, t, selectedSeats);
	}
	else {
    	send(connectionFd, no, strlen(no) + 1, 0);

	}

}

void inquireReservation(int connectionFd) {
	char ticketNum[SMALL_BUFFER];
	FILE *fp;
	DIR *d;
	struct dirent *dir;
	ssize_t r;
	char *line = NULL;
	size_t len = 0;
	int flag = 0;

	recv(connectionFd, ticketNum, SMALL_BUFFER, 0);
	d = opendir(".");
	if(d) {
    	while ((dir = readdir(d)) != NULL) {
        	if(strstr(dir->d_name, "Summary")) {
            	fp = fopen(dir->d_name, "r");
            	while ((r = getline(&line, &len, fp)) != -1) {
                	if(strstr(line, ticketNum)) {
                    	send(connectionFd, line, strlen(line) + 1, 0);
                    	flag = 1;
                    	break;
                	}
            	}
            	fclose(fp);

            	if(flag == 1)
                	break;
        	}
    	}
    	if(flag == 0)
        	send(connectionFd, "not found", 10, 0);
    	closedir(d);
	}
}

void modifyReservation(int connectionFd) {
	printf(" ");
}

void cancelReservation(int connectionFd) {
	char ticketNum[SMALL_BUFFER];
	char fileName[SMALL_BUFFER];
	char tmpBuffer[BUFFER];
	char s[2] = " ";
	char *token;
	char *tmp;
	FILE *fp1;
	FILE *fp2;
	DIR *d;
	struct dirent *dir;
	ssize_t r;
	char *line = NULL;
	size_t len = 0;
	int flag = 0;
	int tmpFlag = 0;
	int count;
	int cancelledSeats[ROWS * COLS];
	int i;
	int j;

	recv(connectionFd, ticketNum, SMALL_BUFFER, 0);
	d = opendir(".");
	if(d) {
    	while ((dir = readdir(d)) != NULL) {
        	if(strstr(dir->d_name, "Summary")) {
    	        fp1 = fopen(dir->d_name, "r");
            	fp2 = fopen("temp.txt", "w");
            	while ((r = getline(&line, &len, fp1)) != -1) {
                	if(strstr(line, ticketNum)) {
                    	// update file
              	      strcpy(tmpBuffer, line);

                    	send(connectionFd, "cancelled", 10, 0);
                    	flag = 1;
                	}
                	else {
                    	fprintf(fp2, "%s", line);
                	}
 	           }
            	remove(dir->d_name);
            	rename("temp.txt", dir->d_name);
            	fclose(fp1);
            	fclose(fp2);
            	if(flag == 1) {
                	strcpy(fileName, dir->d_name);
                	tmp = strchr(fileName, 'S');
                	if(tmp) {
                    	fileName[tmp - fileName] = '\0';
                    	fp1 = fopen(fileName, "r");
                    	fp2 = fopen("tmp.txt", "w");

                    	token = strtok(tmpBuffer, s);
                    	token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	if(strchr(token, '/') == NULL) {
                        	token = strtok(NULL, s);
                    	}
                    	token = strtok(NULL, s);
                   	 token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	token = strtok(NULL, s);
                    	count = atoi(token);
                    	printf("count is %d\n", count);
                    	r = getline(&line, &len, fp1);
                    	printf("atoi is is %d\n", atoi(line));
                    	fprintf(fp2, "%d\n", atoi(line) + count);
                    	token = strtok(NULL, s);
                    	i = 0;
                    	while(token != NULL) {
                        	cancelledSeats[i++] = atoi(token);
                        	token = strtok(NULL, s);
                    	}
                    	i = 0;
                    	while ((r = getline(&line, &len, fp1)) != -1) {

                        	for(j = 0, tmpFlag = 0; j < count; j++) {
                                if(cancelledSeats[j] == i) {
                                	printf("22\n");
                                	fprintf(fp2, "=\n");
                                	tmpFlag = 1;
                                	break;
                            	}
                        	}
                        	i++;
                        	if(tmpFlag == 0)
                  	          fprintf(fp2, "%s", line);
                    	}
                    	fclose(fp1);
                    	fclose(fp2);
                    	remove(fileName);
                    	rename("tmp.txt", fileName);
                	}
                	break;
            	}
        	}
    	}
    	if(flag == 0)
        	send(connectionFd, "not found", 10, 0);
    	closedir(d);
	}
}

