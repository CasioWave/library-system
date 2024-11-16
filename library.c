#include "utils.h"
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fuzzy.h"

#define MAXCHARLIM 4000
#define ISSUE 1
#define RETURN -1

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
        books[i].pubDate = strdup(bookData.data[i][PUBDATE]);
        books[i].publisher = strdup(bookData.data[i][PUBLISHER]);
        books[i].qty = atoi(bookData.data[i][QTY]);

    }
    return books;
}

void search(int** idx, int *numResults, Book** books, int nbooks, char* searchStr) {
    *numResults = fuzzy_search(searchStr, "dict_soundex.csv", idx);
    /* *numResults = 5; */
    /* *idx = NULL; */
    /* int* results = fuzzy_search(searchStr, "dict_soundex.csv"); */
    /* int i = 0; */
    /* while(results[i] != -1){ */
    /*     if (*idx == NULL){ */
    /*         *idx = calloc(i+1, sizeof(int)); */
    /*     } */
    /*     else{ */
    /*         *idx = reallocarray(*idx, i+1, sizeof(int)); */
    /*     } */
    /*     (*idx)[i] = results[i]; */
    /*     ++i; */
    /* } */
    /*
    int c = 0;
    for (int i = 0; i < nbooks; ++i) {
        if (strcasestr((*books)[i].title, searchStr) != NULL) {
            if (*idx == NULL) *idx = calloc(c + 1, sizeof(int));
            else *idx = reallocarray(*idx, c + 1, sizeof(int));
            (*idx)[c++] = i;
        }
    }
    */
    /* *numResults = i; */
}

int issueBook(char* uname, int bookID) {
    time_t t;
    t = time(NULL);
    struct tm* ptr;
    ptr = localtime(&t);
    char date[55];
    int len = snprintf(date, sizeof(date), "%s", asctime(ptr));
    FILE *fp = NULL;
    fp = fopen("transanctions.csv", "a");
    if (fp == NULL) return -1;
    char row[255];
    len = snprintf(row, sizeof(row), "%s,%d,%d,%s", uname, bookID, ISSUE, date);
    fwrite("\n", 1, 1, fp);
    fwrite(row, len, 1, fp);
    fclose(fp);
    return 0;
}

void updateBooks(Book* books, int nbooks) {
    char row[MAXCHARLIM];
    int len = snprintf(row, sizeof(row), "%s,%s,%s,%s,%s,%s,%s", "bookID", "title", "authors", "num_pages", "publication_date", "publisher", "qty");
    FILE* fp = fopen("books-clean.csv", "w");
    fwrite(row, len, 1, fp);
    for (int i = 0; i < nbooks; i++) {
        fprintf(fp, "\n%d,%s,%s,%d,%s,%s,%d", books[i].id, books[i].title, books[i].authors, books[i].pages, books[i].pubDate, books[i].publisher, books[i].qty);
    }
    fclose(fp);
    return;
}
