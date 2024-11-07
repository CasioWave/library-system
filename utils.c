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
            csv.data[r - 1] = calloc(c, sizeof(char *));
            int cid = 0;
            while (cell != NULL) {
                csv.data[r - 1][cid] = strdup(cell);
                cell = strtok(NULL, ",");
                cid++;
            }
        }
        r++;
    }
    csv.nrows = r - 1;
    csv.ncols = c;
    return csv;
}

// Writes the data from a CSV Struct at row,col to a string read
int readData(CSV data, int row, int col, char read[])
{
	int c = 0;

	for (c = 0; *(data.data[row][col]+c) != '\0'; ++c)
	{
		read[c] = *(data.data[row][col]+c);
	}

	read[c] = *(data.data[row][col]+c);
	++c;
	printf("%s\n",read);

	return c;
}

int pickler(CSV data, char file[]){
    int i, j = 0;
    FILE *fl;
    fl = fopen(file, "w");
    char temp[MAXCHAR];
    for (i = 0; i < data.nrows; ++i){
        for (j = 0; j < data.ncols; ++j){
            readData(data, i, j, temp);
            fprintf(fl, temp);
            if (j==data.ncols-1){
                continue;
            }
            fprintf(fl, ",");
        }
    }
    fclose(fl);
    return 0;
}