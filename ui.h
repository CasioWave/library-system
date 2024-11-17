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
void drawBooksTable();
void loadBooks();
void scroll();
void statusBar();
void topBar();
void drawBook();
void drawCommand();
void drawHelp();
char* commandPrompt(char*);
void setCommandMsg(const char*, ...);
void searchPrompt();
void drawSearchResults();
void issuePrompt(int);
void addPrompt();
void editPrompt(int);
int idtoIdx(int);
int searchForID(int,int,int);
void deletePrompt(int);
