#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 4000


CSV readCSV(FILE * fp) {
    CSV csv;
    char row[MAXCHAR];
    char *cell;
    int r, c;
    r = c = 0;
    while(feof(fp) != 1) {
        fgets(row, MAXCHAR, fp);
        cell = strtok(row, ",");
        if (r == 0) {
            while (cell != NULL) {
                cell = strtok(NULL, ",");
                c++;
            }
        }
        if (r > 0) {
            if (r == 1) {
                csv.data = calloc(r, sizeof(char **));
            } 
            if (r > 1) {
                csv.data = reallocarray(csv.data, r, sizeof(char**));
            }
            csv.data[r] = calloc(c, sizeof(char *));
            int cid = 0;
            while (cell != NULL) {
                strcpy(csv.data[r][cid], cell);
                cell = strtok(NULL, ",");
                cid++;
            }
        }
        printf("\n");
        r++;
    }
    csv.nrows = r - 1;
    csv.ncols = c;
    return csv;
}

