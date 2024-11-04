#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "ui.h"
#include "library.h"
#include "login.h"
#include "utils.h"


int main() {
    /* enableRawMode(); */
    
    /* while(1) { */
    /*     refreshScreen(); */
    /*     handleKeyPress(); */
    /* } */
    /* disableRawMode(); */
    FILE* fp;
    CSV books;
    fp = fopen("books-clean.csv", "r");
    books = readCSV(fp);
    printf("Successfully read %d rows and %d columns from the CSV file.\nBelow are the first 10 entries.\n", books.nrows, books.ncols);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < books.ncols; ++j) {
            printf("%s\t", books.data[i][j]);
        }
        printf("\n");
    }
    return 0;
}
