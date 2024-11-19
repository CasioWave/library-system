#pragma once

#include <stdio.h>


typedef struct {
    int ncols, nrows;
    char*** data;
} CSV;

CSV readCSV(FILE *);
int readData(CSV data, int row, int col, char read[]);
int pickler(CSV data, char file[]);