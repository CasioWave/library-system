#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "ui.h"
#include "library.h"
#include "login.h"

int main() {
    /* enableRawMode(); */
    
    /* while(1) { */
    /*     refreshScreen(); */
    /*     handleKeyPress(); */
    /* } */
    /* disableRawMode(); */
    Book* books = fetchBooks("books-clean.csv");
    displayBooks(books);
    return 0;
}
