#include <stdio.h>
#include <stdlib.h>
#include "damerau-levenshtein.h"
#include "search-utils.h"
#include "soundex.h"

int* fuzzy_search(char* query, FILE* dict);
int read_line(FILE* file, char* ret);
char** synonyms(FILE* thesaurus, char** queries, int len_query);