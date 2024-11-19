#include "search-utils.h"
//By default, all of these costs are set to one - meaning that all these types of typos are equally likely. That can be changed easily.
#define SUBSCOST 1
#define DELCOST 1
#define INSCOST 1
#define TRANCOST 1

int damLevMatrix(char s1[], char s2[]);
int min3(int a, int b, int c);