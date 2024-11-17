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
    int cx, cy, screenrows, screencols, rowoff, numResults;
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
            if (E.sIdx != NULL && E.page == BOOK_VIEW) {
                E.page = SEARCH;
            } else {
                E.page = NORMAL;
                free(E.sIdx);
                E.sIdx = NULL;
            }
            break;
        case '/':
            if (E.page == NORMAL) searchPrompt();
            break;
        case 'i':
            if (E.page == BOOK_VIEW && E.userPriv != ADMIN) {
                if (E.sIdx == NULL) {
                    issuePrompt(E.cy);
                } else {
                    issuePrompt(E.sIdx[E.cy]);
                }
            }
            break;
        case 'e':
            if (E.page == BOOK_VIEW && E.userPriv == ADMIN) {
                if (E.sIdx == NULL) {
                    editPrompt(E.cy);
                } else {
                    editPrompt(E.sIdx[E.cy]);
                }
            }
            break;
        case 'a':
            if (E.page == NORMAL && E.userPriv == ADMIN) addPrompt();
            break;
        default:
            break;
    }
}

int idtoIdx(int id) {
    int a = 0;
    int b = E.nbooks - 1;
    if (E.books[a].id == id) return a;
    if (E.books[b].id == id) return b;
    while (a != b) {
        int n = (a + b)/2;
        if (E.books[n].id == id) return n;
        if (id > E.books[n].id) a = n;
        if (id < E.books[n].id) b = n;
    }
    return -1;
}
void moveCursor(int c) {
    switch (c) {
        case ARROW_UP:
            if (E.cy > 0) E.cy--;
            break;
        case ARROW_DOWN:
            if ((E.page == NORMAL && E.cy < E.nbooks - 1) || (E.page == SEARCH && E.cy < E.numResults - 1)) E.cy++;
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
void reloadBooks() {
    free(E.books);
    E.books = NULL;
    E.books = fetchBooks("books-clean.csv", &E.nbooks);
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
void drawBooksTable() {
    for (int y = 0 ; y < E.screenrows; ++y) {
        int filerow = y + E.rowoff;
        if (filerow < E.nbooks) {
            if (E.cy == filerow) write(STDOUT_FILENO, "\x1b[7m", 4);
            char rec[320];
            int len = snprintf(rec, sizeof(rec), "%-7d|%-55.55s|%-55.55s|%-55.55s|%d", E.books[filerow].id, E.books[filerow].title, E.books[filerow].authors, E.books[filerow].publisher, E.books[filerow].qty);
            write(STDOUT_FILENO, rec, len);
            if (E.cy == filerow) write(STDOUT_FILENO, "\x1b[m", 3);
        }
        write(STDOUT_FILENO, "\x1b[K", 3);
        write(STDOUT_FILENO, "\r\n", 2);
    }
}
void drawSearchResults() {
    for (int i = 0; i < E.screenrows; ++i) {
        int filerow = i + E.rowoff;
        if (filerow < E.numResults) {
            if(E.cy == filerow) write(STDOUT_FILENO, "\x1b[7m", 4);
            char rec[320];
            int len = snprintf(rec, sizeof(rec), "%-7d|%-55.55s|%-55.55s|%-55.55s|%d", E.books[E.sIdx[filerow]].id, E.books[E.sIdx[filerow]].title, E.books[E.sIdx[filerow]].authors, E.books[E.sIdx[filerow]].publisher, E.books[E.sIdx[filerow]].qty);
            write(STDOUT_FILENO, rec, len);
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

void addPrompt() {
    char* title = NULL;
    title = commandPrompt("Enter title: %s");
    if (title == NULL) {
        setCommandMsg("Operation Aborted");
        return;
    }
    char* authors = NULL;
    authors = commandPrompt("Enter Author(s) (separate multiple authors using a '/'): %s");
    if (authors == NULL) {
        setCommandMsg("Operation Aborted");
        return;
    }
    char* publisher = NULL;
    publisher = commandPrompt("Enter publisher: %s");
    if (publisher == NULL) {
        setCommandMsg("Operation Aborted");
        return;
    }
    char* pubDate = NULL;
    pubDate = commandPrompt("Enter publication date: %s");
    if (pubDate == NULL) {
        setCommandMsg("Operation Aborted");
        return;
    }
    char* pages = NULL;
    pages = commandPrompt("Enter number of pages: %s");
    if (pages == NULL) {
        setCommandMsg("Operation Aborted");
        return;
    }
    char* copies = NULL;
    copies = commandPrompt("Enter copies: %s");
    if (copies == NULL) {
        setCommandMsg("Operation Aborted");
        return;
    }
    Book book;
    book.title = strdup(title);
    book.authors = strdup(authors);
    book.publisher = strdup(publisher);
    book.pubDate = strdup(pubDate);
    book.id = E.books[E.nbooks - 1].id + 1;
    book.qty = atoi(copies);
    book.pages = atoi(copies);
    E.books = realloc(E.books, (E.nbooks + 1)*sizeof(Book));
    E.books[E.nbooks++] = book;
    updateBooks(E.books, E.nbooks);
    reloadBooks();
    setCommandMsg("Added Book calld %s with ID %d",book.title, book.id);
}
void editPrompt(int i) {
    char* field = NULL;
    field = commandPrompt("Enter the Field to edit [title: 1, authors: 2, publisher: 3, publication date: 4, pages: 5, copies: 6]: %s");
    if (field == NULL) {
        setCommandMsg("Operation Aborted editing %d %d", i, E.nbooks);
        return;
    }
    int choice = atoi(field);
    if (choice < 1 || choice > 6) {
        setCommandMsg("Invalid Input");
        return;
    }
    char* val = NULL;
    val = commandPrompt("Enter the new value for this field: %s");
    switch (choice) {
        case 1:
            E.books[i].title = strdup(val);
            break;
        case 2:
            E.books[i].authors = strdup(val);
            break;
        case 3:
            E.books[i].publisher = strdup(val);
            break;
        case 4:
            E.books[i].pubDate = strdup(val);
            break;
        case 5:
            E.books[i].pages = atoi(val);
            break;
        case 6:
            E.books[i].qty = atoi(val);
            break;
        default:
            break;
    }
    updateBooks(E.books, E.nbooks);
    setCommandMsg("Successfully Edited the book");
}
void issuePrompt(int i) {
    if (E.books[i].qty < 1) {
        setCommandMsg("Can't issue this book. Not enough copies available");
        return;
    }
    if (E.books[i].qty <= 3 && E.userPriv != FACULTY) {
        setCommandMsg("Less than 3 copies available. Only Faculty members can issue this book");
        return;
    }
    int ret = issueBook(E.username, E.books[i].id);
    if (ret == 0) {
        setCommandMsg("Successfully issued book number %d for user %s", E.books[i].id, E.username);
        E.books[i].qty--;
        updateBooks(E.books, E.nbooks);
        reloadBooks();
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
    for (int i = 0; i < E.numResults; ++i) {
        E.sIdx[i] = idtoIdx(E.sIdx[i]);
    }
    setCommandMsg("You searched for: %s, %d matches found.", searchStr, E.numResults);
    if (E.numResults > 0) {
        E.page = SEARCH; 
        E.rowoff = 0;
        E.cy = 0;
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
    if (E.page == NORMAL) drawBooksTable();
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
    E.numResults = E.page = E.rowoff = E.cx = E.cy = 0;
    E.sIdx = NULL;
    snprintf(E.commandBuf, sizeof(E.commandBuf), "Press / to start a search");
    E.commandTime = time(NULL);
    if (login(&E.userPriv, &E.username) == LOGIN_FAILURE) {
        printf("Login Failed!!");
        exit(1);
        return;
    }
    reloadBooks();
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
