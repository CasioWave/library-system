#include <stdio.h>
#include <stdlib.h>
#include "damerau-levenshtein.h"
#include "search-utils.h"
#include "soundex.h"
#define LEVWEIGHT 10
#define EXACT_BONUS 100
#define SOUNDWEIGHT 0.5
#define MAXLEV 0 //Important - this integer controls how fuzzy it gets
#define SOUND_LEN 5
#define SYN_PENALTY 5
#define MAXQUERY 20
#define MAXRES 1000

int* fuzzy_search(char* query, char* dict_file);
int read_line(FILE* file, char* ret);