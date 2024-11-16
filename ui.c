#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include "ui.h"
#include "library.h"
#include "login.h"

#define MSGTIMEOUT 3
#define MAXCHARLIM 5000

enum PAGES {
    NORMAL=0,
    BOOK_VIEW,
    SEARCH
};
struct state {
    int numSRows, cx, cy, screenrows, screencols, numrows, rowoff, numResults;
    erow* row;
    erow* sRow;
    struct termios orig_term;
    Book* books;
    int nbooks;
    char* username;
    int userPriv;
    int page;
    char commandBuf[MAXCHARLIM];
    time_t commandTime;
    int* sIdx;
};
struct state E;

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_term) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = E.orig_term;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void die(char *s) {
    resetScreen();
    perror(s);
    exit(1);
}
void disableRawMode() {
    write(STDOUT_FILENO, "\x1b[?25h", 6); 
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_term) == -1) die("tcsetattr");
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
        case '\r':
            E.page = BOOK_VIEW;
            break;
        case '\x1b':
            if (E.page == SEARCH) E.rowoff = 0;
            if (E.sIdx != NULL) E.page = SEARCH;
            E.page = NORMAL;
            break;
        case '/':
            if (E.page == NORMAL) searchPrompt();
            break;
        case 'i':
            if (E.page == NORMAL) issuePrompt(E.cy);
            break;
        default:
            break;
    }
}

void moveCursor(int c) {
    switch (c) {
        case ARROW_UP:
            if (E.cy > 0) E.cy--;
            break;
        case ARROW_DOWN:
            if ((E.page == NORMAL && E.cy < E.numrows - 1) || (E.page == SEARCH && E.cy < E.numSRows - 1)) E.cy++;
            break;
        case ARROW_LEFT:
            if (E.cx > 0) E.cx--;
            break;
        case ARROW_RIGHT:
            if (E.cx < E.screencols) E.cx++;
            break;
    }
    return;
}
void appendSearchRow(char *s, size_t len) {
    E.sRow = realloc(E.sRow, sizeof(erow) * (E.numSRows + 1));
    int at = E.numSRows;
    E.sRow[at].size = len;
    E.sRow[at].chars = malloc(len + 1);
    memcpy(E.sRow[at].chars, s, len);
    E.sRow[at].chars[len] = '\0';
    E.numSRows++;
}
void appendRow(char *s, size_t len) {
    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
    int at = E.numrows;
    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';
    E.numrows++;
}
void renderBooks() {
    free(E.books);
    for (int i = 0; i < E.numrows; ++i) {
        free(E.row[i].chars);
    }
    free(E.row);
    E.row = NULL;
    E.numrows = 0;
    E.books = NULL;
    E.books = fetchBooks("books-clean.csv", &E.nbooks);
    for (int i = 0; i < E.nbooks; ++i) {
        char rec[320];
        int len = snprintf(rec, sizeof(rec), "%-7d|%-55.55s|%-55.55s|%-55.55s|%d", i, E.books[i].title, E.books[i].authors, E.books[i].publisher, E.books[i].qty);
        appendRow(rec, len);
    }
}
void renderSearchResults() {
    for (int i = 0; i < E.numSRows; ++i) {
        free(E.sRow[i].chars);
        E.sRow[i].size = 0;
    }
    E.numSRows = 0;
    E.sRow = NULL;
    for (int i = 0; i < E.numResults; ++i) {
        char bookRec[MAXCHARLIM];
        /* int len = snprintf(bookRec, sizeof(bookRec), "%-7d|%-55.55s|%-55.55s|%-55.55s|%d", E.sIdx[i], E.books[E.sIdx[i]].title, E.books[E.sIdx[i]].authors, E.books[E.sIdx[i]].publisher, E.books[E.sIdx[i]].qty); */
        int len = snprintf(bookRec, sizeof(bookRec), "%-7d|HAHAHA", E.sIdx[i]);
        appendSearchRow(bookRec, len);
    }
}
void statusBar() {
    write(STDOUT_FILENO, "\x1b[1;7m", 6);
    char status[80];
    int len = snprintf(status, sizeof(status), "Library Management System");
    if (len > E.screencols) len = E.screencols;
    write(STDOUT_FILENO, status, len);
    char buf[255];
    int lenBuf = snprintf(buf, sizeof(buf), "Logged in as: '%s' [%d]", E.username, E.userPriv);
    for (int i = 0; i < E.screencols - len - lenBuf;++i) {
        write(STDOUT_FILENO, " ", 1);
    }
    write(STDOUT_FILENO, buf, lenBuf);
    write(STDOUT_FILENO, "\x1b[m", 3);
    write(STDOUT_FILENO, "\r\n", 2);
}

void topBar() {
    write(STDOUT_FILENO, "\x1b[1;7m", 6);
    char top[320];
    int len = 0;
    if (E.page == BOOK_VIEW) {
        len = snprintf(top, sizeof(top), "Book Details");
    } else {
        len = snprintf(top, sizeof(top), "%-7s|%-55s|%-55s|%-55s|%s", "ID", "Title", "Authors", "Publishers", "Qty.");
    }
    write(STDOUT_FILENO, top, len);
    for (int i = 0; i < E.screencols - len; ++i) write(STDOUT_FILENO, " ", 1);
    write(STDOUT_FILENO, "\x1b[m", 3);
    write(STDOUT_FILENO, "\r\n", 2);
}
void drawRows() {
    for (int y = 0 ; y < E.screenrows; ++y) {
        int filerow = y + E.rowoff;
        if (filerow < E.numrows) {
            if (E.cy == filerow) write(STDOUT_FILENO, "\x1b[7m", 4);
            write(STDOUT_FILENO, E.row[filerow].chars, E.row[filerow].size);
            if (E.cy == filerow) write(STDOUT_FILENO, "\x1b[m", 3);
        }
        write(STDOUT_FILENO, "\x1b[K", 3);
        write(STDOUT_FILENO, "\r\n", 2);
    }
}
void drawSearchResults() {
    for (int i = 0; i < E.screenrows; ++i) {
        int filerow = i + E.rowoff;
        if (filerow < E.numSRows) {
            if(E.cy == filerow) write(STDOUT_FILENO, "\x1b[7m", 4);
            write(STDOUT_FILENO, E.sRow[filerow].chars, E.sRow[filerow].size);
            if (E.cy == filerow) write(STDOUT_FILENO, "\x1b[m", 3);
        }
        write(STDOUT_FILENO, "\x1b[K", 3);
        write(STDOUT_FILENO, "\r\n", 2);
    }
}
void drawBook() {
    char bookID[MAXCHARLIM], bookTitle[MAXCHARLIM], bookAuthor[MAXCHARLIM], bookPublisher[MAXCHARLIM], bookPages[MAXCHARLIM], bookQty[MAXCHARLIM], bookDate[MAXCHARLIM];
    int idx = E.sIdx == NULL ? E.cy : E.sIdx[E.cy];
    int len = snprintf(bookID, sizeof(bookID), "\x1b[33mBook ID:\x1b[m %d\r\n", E.books[idx].id);
    write(STDOUT_FILENO, bookID, len);
    len = snprintf(bookTitle, sizeof(bookTitle), "\x1b[33mBook Title:\x1b[m %s\r\n", E.books[idx].title);
    write(STDOUT_FILENO, bookTitle, len);
    len = snprintf(bookAuthor, sizeof(bookAuthor), "\x1b[33mAuthor(s):\x1b[m %s\r\n", E.books[idx].authors);
    write(STDOUT_FILENO, bookAuthor, len);
    len = snprintf(bookPublisher, sizeof(bookPublisher), "\x1b[33mPublisher:\x1b[m %s\r\n", E.books[idx].publisher);
    write(STDOUT_FILENO, bookPublisher, len);
    len = snprintf(bookPages, sizeof(bookPages), "\x1b[33mNumber of Pages:\x1b[m %d\r\n", E.books[idx].pages);
    write(STDOUT_FILENO, bookPages, len);
    len = snprintf(bookDate, sizeof(bookDate), "\x1b[33mPublication Date:\x1b[m %s\r\n", E.books[idx].pubDate);
    write(STDOUT_FILENO, bookDate, len);
    len = snprintf(bookQty, sizeof(bookQty), "\x1b[33mCopies Available:\x1b[m %d\r\n", E.books[idx].qty);
    write(STDOUT_FILENO, bookQty, len);
    write(STDOUT_FILENO, "\x1b[m", 4);
    for (int y = 0 ; y < E.screenrows - 7; ++y) {
        write(STDOUT_FILENO, "\r\n", 2);
    }
}

void drawCommand() {
    char buf[MAXCHARLIM];
    int len = 0;
    write(STDOUT_FILENO, "\x1b[33m", 5);
    if (time(NULL) - E.commandTime < MSGTIMEOUT) {
        len = snprintf(buf, sizeof(buf), "%s\r\n", E.commandBuf);
    } else {
        len = snprintf(buf, sizeof(buf), "Press / to start a search\r\n");
    }
    write(STDOUT_FILENO, buf, len);
    write(STDOUT_FILENO, "\x1b[0m", 4);
}

void setCommandMsg(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.commandBuf, sizeof(E.commandBuf), fmt, ap);
    va_end(ap);
    E.commandTime = time(NULL);
}
void drawHelp() {
    char buf[MAXCHARLIM];
    int len = 0;
    write(STDOUT_FILENO, "\x1b[32m", 5);
    if (E.page == NORMAL) len  = snprintf(buf, sizeof(buf), "Press [ENTER] to view Book Details\r\n");
    if (E.page == BOOK_VIEW) len = snprintf(buf, sizeof(buf), "Press [ESC] to go back\r\n");
    write(STDOUT_FILENO, buf, len);
    write(STDOUT_FILENO, "\x1b[0m", 4);
}

char *commandPrompt(char *prompt) {
    size_t bufsize = MAXCHARLIM;
    char *buf = (char*) malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';
    
    while (1) {
        setCommandMsg(prompt, buf);
        refreshScreen();

        int c = readKeyPress();
        if (c == BACKSPACE || c == CTRL_Key('h')) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (c == '\x1b') {
            setCommandMsg("");
            free(buf);
            return NULL;
        } else if (c == '\r') {
            if (buflen != 0) {
                setCommandMsg("");
                return buf;
            }
        } else if(!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
    }
}

void issuePrompt(int i) {
    int ret = issueBook(E.username, E.cy);
    if (ret == 0) {
        setCommandMsg("Successfully issued book number %d for user %s", i, E.username);
        E.books[i].qty--;
        updateBooks(E.books, E.nbooks);
        renderBooks();
    } else {
        setCommandMsg("Coudln't issue book. Error Code: %d", ret);
    }
}
void searchPrompt() {
    // This is a dummy search to test the command prompt feature.
    char* searchStr = NULL;
    searchStr = commandPrompt("Search: %s");
    if (searchStr == NULL) {
        setCommandMsg("Search Aborted.");
        return;
    }
    int len = strlen(searchStr);
    while (searchStr[len - 1] == ' ' || searchStr[len - 1] == '\t') searchStr[--len] = '\0';
    E.numResults = 0;
    E.sIdx = NULL;
    search(&E.sIdx, &E.numResults, &E.books, E.nbooks, searchStr);
    setCommandMsg("You searched for: %s, %d matches found.", searchStr, E.numResults);
    if (E.numResults > 0) {
        E.page = SEARCH; 
        E.rowoff = 0;
        E.cy = 0;
        renderSearchResults();
    }
}

void scroll() {
    if (E.cy < E.rowoff) {
        E.rowoff = E.cy;
        return;
    }
    if (E.cy >= E.rowoff + E.screenrows) {
        E.rowoff = E.cy - E.screenrows + 1;
    }
}
void goToxy(int x, int y) {
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", y, x);
    write(STDOUT_FILENO, buf, strlen(buf));
    return;
}

void resetScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void refreshScreen() {
    scroll();
    write(STDOUT_FILENO, "\x1b[?25l", 6);
    resetScreen();
    topBar();
    if (E.page == NORMAL) drawRows();
    if (E.page == BOOK_VIEW) drawBook();
    if (E.page == SEARCH) drawSearchResults();
    statusBar();
    drawCommand();
    drawHelp();
    goToxy(E.cx, E.cy - E.rowoff);
    /* write(STDOUT_FILENO, "\x1b[?25h", 6); */
    return;
}

void init() {
    E.numSRows = E.numResults = E.page = E.rowoff = E.cx = E.cy = E.numrows = 0;
    E.sIdx = NULL;
    snprintf(E.commandBuf, sizeof(E.commandBuf), "Press / to start a search");
    E.commandTime = time(NULL);
    if (login(&E.userPriv, &E.username) == LOGIN_FAILURE) {
        printf("Login Failed!!");
        exit(1);
        return;
    }
    renderBooks();
    enableRawMode();
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
    E.screenrows -= 5;
    resetScreen();
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getCursorPosition(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

int getCursorPosition(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
    return 0;
}
