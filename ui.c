#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "ui.h"

struct termios orig_term;

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_term) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_term;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void die(char *s) {
    resetScreen();
    disableRawMode();
    perror(s);
    exit(1);
}
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term) == -1) die("tcsetattr");
}
int readKeyPress() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if (nread == -1) die("read");
    }

    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
    }
    return c;
}

void handleKeyPress() {
    int c = readKeyPress();
    switch (c) {
        case CTRL_Key('q'):
            resetScreen();
            disableRawMode();
            exit(0);
            break;
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            moveCursor(c);
            break;
    }
}

void moveCursor(int c) {
    //DO something
    return;
}

void resetScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void refreshScreen() {
    resetScreen();
}

void mvCursorTo(int x, int y) {
    // TODO
    // add the escape sequence
}
