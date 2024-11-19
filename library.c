#include "utils.h"
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fuzzy.h"
#include "preprocess.h"

#define MAXCHARLIM 4000
#define ISSUE 1
#define RETURN -1
#define WEEK 604800

enum bookDataFields  {
    ID = 0,
    TITLE,
    AUTH,
    PAGES,
    PUBDATE,
    PUBLISHER,
    QTY
};

// fetches the books from the database and returns a malloced array
Book* fetchBooks(char * fname, int* nbooks) {
    Book* books;
    FILE* fp;

    fp = fopen(fname, "r");
    CSV bookData;

    bookData = readCSV(fp);
    fclose(fp);
    if (bookData.nrows == 0) return NULL;
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

// function to perform a free-text fuzzy search
void search(int** idx, int *numResults, Book** books, int nbooks, char* searchStr) {
    /* *numResults = fuzzy_search(searchStr, "dict_soundex.csv", idx); */
    /* *numResults = 5; */
    *idx = NULL;
    int* results = fuzzy_search(searchStr, 0, "dict_soundex.csv");
    int i = 0;
    while(results[i] != -1){
        if (*idx == NULL){
            *idx = calloc(i+1, sizeof(int));
        }
        else{
            *idx = reallocarray(*idx, i+1, sizeof(int));
        }
        (*idx)[i] = results[i];
        ++i;
    }
    *numResults = i;
    free(results);
    /* int c = 0; */
    /* for (int i = 0; i < nbooks; ++i) { */
    /*     if (strcasestr((*books)[i].title, searchStr) != NULL) { */
    /*         if (*idx == NULL) *idx = calloc(c + 1, sizeof(int)); */
    /*         else *idx = reallocarray(*idx, c + 1, sizeof(int)); */
    /*         (*idx)[c++] = i; */
    /*     } */
    /* } */
    /* *numResults = c; */
}

// A function to perform advanced search 
void advancedSearch(int** idx, int *numResults, Book** books, int nbooks, char* title, char* author, char* publisher) {
    *idx = NULL;
    int* results = advanced_search(title, author, publisher, "dict_soundex.csv");
    int i = 0;
    while(results[i] != -1){
        if (*idx == NULL){
            *idx = calloc(i+1, sizeof(int));
        }
        else{
            *idx = reallocarray(*idx, i+1, sizeof(int));
        }
        (*idx)[i] = results[i];
        ++i;
    }
    *numResults = i;
    free(results);
}

// Write a book issue record to the database
int issueBook(char* uname, int bookID, int nweeks) {
    time_t t;
    t = time(NULL);
    time_t dt = t + (nweeks * WEEK);
    FILE *fp = NULL;
    fp = fopen("transanctions.csv", "a");
    if (fp == NULL) return -1;
    char row[255];
    int len = snprintf(row, sizeof(row), "%s,%d,%ld,%ld", uname, bookID, t, dt);
    fwrite("\n", 1, 1, fp);
    fwrite(row, len, 1, fp);
    fclose(fp);
    return 0;
}

// Update the books database
void updateBooks(Book* books, int nbooks, int prep) {
    char row[MAXCHARLIM];
    int len = snprintf(row, sizeof(row), "%s,%s,%s,%s,%s,%s,%s", "bookID", "title", "authors", "num_pages", "publication_date", "publisher", "qty");
    FILE* fp = fopen("books-clean.csv", "w");
    fwrite(row, len, 1, fp);
    for (int i = 0; i < nbooks; i++) {
        fprintf(fp, "\n%d,%s,%s,%d,%s,%s,%d", books[i].id, books[i].title, books[i].authors, books[i].pages, books[i].pubDate, books[i].publisher, books[i].qty);
    }
    fclose(fp);
    if (prep == 1) preprocess();
    return;
}
