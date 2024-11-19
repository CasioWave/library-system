#include <stdio.h>
#include <stdlib.h>
#include "damerau-levenshtein.h"
#include "search-utils.h"
#include "soundex.h"
#define LEVWEIGHT 10
#define EXACT_BONUS 100
#define SOUNDWEIGHT 0.5
#define MAXLEV 1 //Important - this integer controls how fuzzy it gets
#define SOUND_LEN 5
#define SYN_PENALTY 5
#define MAXQUERY 20
#define MAXRES 1000
//Cat - 
//0 = No category specification
//1 = Title
//2 = Author
//3 = Publisher
int* fuzzy_search(char* query, int cat, char* dict_file);
int* advanced_search(char* title, char* author, char* pub, char* dict_file);
int read_line(FILE* file, char* ret);