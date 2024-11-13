#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#define MAX_SOUNDEX_HASH 5

//Takes the filename of database and thesaurus, generates the soundex hashed dictionary and sanitized thesaurus 
void preprocess(char* database, char* eng_thesaurus);