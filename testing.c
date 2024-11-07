#include <stdio.h>
#include "utils.h"

int main(){
    FILE *csvptr;
    csvptr = fopen("books-clean.csv","r");
    CSV dat = readCSV(csvptr);
    fclose(csvptr);
    
    pickler(dat, "test.csv");
    return 0;
}