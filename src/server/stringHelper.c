/*
 ============================================================================
 Name        : Assignment03 - Final Project - Group D
 Author(s)   : Jordan Johnson, Mohammad Musaqlab
 Email		 : jjohn84@ostatemail.okstate.edu,
 Date		 : 4/02/2021
 Copyright   : Copyright 2021 MIT License

 Description :
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

/*
** Auxiliary function to skip white spaces
*/
int is_white_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

/*
** Iterate through the whitespaces at the beginning of the string
*/
int get_first_position(char const *str) {
    int i = 0;
    while (is_white_space(str[i])) {
        i += 1;
    }
    return (i);
}

/*
** Get the length of a string
*/
int get_str_len(char const *str) {
    int len = 0;
    while (str[len] != '\0') {
        len += 1;
    }
    return (len);
}
/*
** Find the last position in a string that is not a white space
*/
int get_last_position(char const *str) {
    int i = get_str_len(str) - 1;
    while (is_white_space(str[i])) {
        i -= 1;
    }
    return (i);
}

/*
** Returns the correct length of a trimmed string
*/
int get_trim_len(char const *str) {
    return (get_last_position(str) - get_first_position(str));
}

/*
** Allocates a new string with removed whitespace characters from the beginning of the source string `str`
** https://jraleman.medium.com/implementation-of-the-c-function-strtrim-1a1ea0c60b38
*/
char *strtrim(char const *str) {
// Variables declaration
    char *trim = NULL;
    int i, len, start;
    // Check if the string exists
    if (str != NULL) {
        i = 0;
        len = get_trim_len(str) + 1;
        trim = (char *) malloc(len);
        start = get_first_position(str);
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
