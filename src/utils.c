#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_source

#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>

#include "../include/editor.h"

void terminate(const char *s){
    // write the 4 byte erase in display to the screen
    write(STDOUT_FILENO, "\x1b[2J", 4);
    // move the cursor to the 1,1 position in the terminal
    write(STDOUT_FILENO, "\x1b[H", 3);
    // function to deal with the error outputs
    perror(s);
    exit(1);
}

int getCursorPosition(int* rows, int* columns){
    // man read to get the cursor position
    char buf[32];
    unsigned int i = 0;
    // write a cursor position report
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4){
        return -1;
    }

    // 
    while (i < sizeof(buf) - 1){
        if (read(STDIN_FILENO, &buf[i], 1)){
            break;
        }
        if(buf[i] == 'R'){
            break;
        }
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '['){
        return -1;
    }
    if (sscanf(&buf[2],"%d;%d", rows, columns) != 2){
        return -1;
    }
    return 0;
}

int editorReadKey(void) {
    int nread;
    char c;
    // read each key press
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN){
            terminate("read");
        }
    }
    // first char is the first char of ASCII escape code
    if (c == '\x1b'){
        // sequence of 3 chars first being the ASCII escape code
        char seq[3];

        // read the STDIN and set both the first and second characters in seq
        if (read(STDIN_FILENO, &seq[0], 1) != 1){
            return '\x1b';
        }
        if (read(STDIN_FILENO, &seq[1], 1) != 1){
            return '\x1b';
        }

        // if first char being the special char
        if(seq[0] == '['){
            // check that that seq falls into the bounds of our answer
            if (seq[1] >= '0' && seq[1] <= '9'){
                // read the STDIN to seq
                if (read(STDIN_FILENO, &seq[2], 1) != 1){
                    return '\x1b';
                }
                // check if last char is ~ denoting a special charater seq
                if (seq[2] == '~'){
                    // check the numeric value in the form of a char is equal to the following ASCII codes
                    switch (seq[1]){
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            // another check if the keyboard gives value of a alpha form
            } else{
                switch (seq[1]){
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        // if the value of the first char is a 0 also denoting a special charater
        } else if(seq[0] == 'O'){
            switch (seq[1]){
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
        return '\x1b';
    } else {
        // if no issue each key press
        return c;
    }
}

int getWindowSize(int* rows, int* columns){
    struct winsize ws;

    // easy way to do the getCursorPosition function
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12){
            return getCursorPosition(rows, columns);
        }
        // if the easy way fails use the man one
        editorReadKey();
        return -1;
    } else{
        *columns = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
/* Syntax highlighting */
int isSeparator(int c){
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c);
}

int min(int i1, int i2) {
    if (i1 < i2) return i1;
    else return i2;
}

int max(int i1, int i2) {
    if (i1 > i2) return i1;
    else return i2;
}
