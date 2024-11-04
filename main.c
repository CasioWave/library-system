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
    return 0;
}
