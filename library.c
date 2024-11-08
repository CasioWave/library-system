#include "utils.h"
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum bookDataFields  {
    ID = 0,
    TITLE,
    AUTH,
    PAGES,
    PUBDATE,
    PUBLISHER,
    QTY
};

Book* fetchBooks(char * fname, int* nbooks) {
    Book* books;
    FILE* fp;

    fp = fopen(fname, "r");
    CSV bookData;

    bookData = readCSV(fp);
    fclose(fp);
    *nbooks = bookData.nrows;
    books = calloc(bookData.nrows, sizeof(Book));

    for (int i = 0; i < bookData.nrows; ++i) {
        books[i].id = atoi(bookData.data[i][ID]);
        books[i].title = strdup(bookData.data[i][TITLE]);
        books[i].authors = strdup(bookData.data[i][AUTH]);
        books[i].pages = atoi(bookData.data[i][PAGES]);
        // TODO: PubDate
        books[i].publisher = strdup(bookData.data[i][PUBLISHER]);
        books[i].qty = atoi(bookData.data[i][QTY]);

    }
    return books;
}

void search(int** idx, int *numResults, Book** books, int nbooks, char* searchStr) {
    *idx = NULL;
    int c = 0;
    for (int i = 0; i < nbooks; ++i) {
        if (strcasestr((*books)[i].title, searchStr) != NULL) {
            if (*idx == NULL) *idx = calloc(c + 1, sizeof(int));
            else *idx = reallocarray(*idx, c + 1, sizeof(int));
            (*idx)[c++] = i;
        }
    }
    for (int i = 0; i < nbooks; ++i) {
        if (strcasestr((*books)[i].authors, searchStr) != NULL) {
            c++;
            if (*idx == NULL) *idx = calloc(c + 1, sizeof(int));
            else *idx = reallocarray(*idx, c + 1, sizeof(int));
            (*idx)[c++] = i;
        }
    }
    for (int i = 0; i < nbooks; ++i) {
        if (strcasestr((*books)[i].publisher, searchStr) != NULL) {
            if (*idx == NULL) *idx = calloc(c + 1, sizeof(int));
            else *idx = reallocarray(*idx, c + 1, sizeof(int));
            (*idx)[c++] = i;
        }
    }
    *numResults = c;
}
