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
int NBOOKS;

Book* fetchBooks(char * fname, int* nbooks) {
    Book* books;
    FILE* fp;

    fp = fopen(fname, "r");
    CSV bookData;

    bookData = readCSV(fp);
    fclose(fp);
    NBOOKS = bookData.nrows;
    *nbooks = NBOOKS;
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

void displayBooks(Book* books) {
    Book b;
    for (int i = 0; i < NBOOKS; ++i) {
        b = books[i];
        printf("%d\t\t%s\t\t\t%d\t\t%s\t\t\t%d\n", b.id, b.title, b.pages, b.publisher, b.qty);
    }
    return;
}
