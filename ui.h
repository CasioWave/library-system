#pragma once

#include <stdlib.h>

#define CTRL_Key(x) (x & 31)

enum cursorKeys {
    TAB_KEY = 9,
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY
};

typedef struct erow {
  int size;
  char *chars;
} erow;

void disableRawMode();
void enableRawMode();
void die(char *);
int readKeyPress();
void handleKeyPress();
void moveCursor(int);
void refreshScreen();
void resetScreen();
void goToxy(int, int);
void init();
int getWindowSize(int *, int *);
int getCursorPosition(int*, int*);
void drawRows();
void renderBooks();
void appendRow(char*, size_t);
void scroll();
void statusBar();
void topBar();
void drawBook();
void drawCommand();
void drawHelp();
char* commandPrompt(char*);
void setCommandMsg(const char*, ...);
void searchPrompt();
void appendSearchRow(char*, size_t);
void renderSearchResults();
void drawSearchResults();
