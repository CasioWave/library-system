#pragma once

#include <stdio.h>


typedef struct {
    int ncols, nrows;
    char*** data;
} CSV;

CSV readCSV(FILE *);
