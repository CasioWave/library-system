#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soundex.h"
#include <ctype.h>
#include "search-utils.h"

#define MAX_LINE_LENGTH 10000
#define MAX_TOKENS 100000

char* stitch(int* arr, int count);
int preprocess();