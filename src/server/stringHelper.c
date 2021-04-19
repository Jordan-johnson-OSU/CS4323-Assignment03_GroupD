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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serverHeader.h"

/*
 ** Allocates a new string with removed whitespace characters from the beginning of the source string `str`
 ** Based on: https://jraleman.medium.com/implementation-of-the-c-function-strtrim-1a1ea0c60b38
 */
char* strtrim(char const *str) {

	char *trim = NULL;
	int i, len, start;

	// Check if the string exists
	if (str != NULL) {
		i = 0;

		int lastPos = 0;
		//Find the first null out position (we have to null out our memory)
		while (str[lastPos] != '\0') {
			lastPos += 1;
		}
		//trim any whitespaces, tabs, or new lines on the last position
		while ((str[lastPos] == ' ' || str[lastPos] == '\t' || str[lastPos] == '\n')) {
			lastPos -= 1;
		}

		int firstPos = 0;
		//trim any whitespaces, tabs, or new lines on the first postion
		while ((str[firstPos] == ' ' || str[firstPos] == '\t' || str[firstPos] == '\n')) {
			firstPos += 1;
		}

		len = (lastPos - firstPos) + 1;
		trim = (char*) malloc(len);
		start = firstPos;

		// Copy content to trim string
		while (i < len) {
			trim[i] = str[start];
			i += 1;
			start += 1;
		}

		// Null terminate the trimmed string.
		trim[i] = '\0';
	}
	return (trim);
}
