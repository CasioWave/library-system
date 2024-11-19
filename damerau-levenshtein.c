#include "damerau-levenshtein.h"
//Find the minimum of three integers - used in the dynamic filling up of the Levenshtein matrix
int min3(int a, int b, int c){
    if (a <= b && a <=c){
        return a;
    }
    else if (b <= a && b <= c){
        return b;
    }
    else{
        return c;
    }
    return -1;
}

// This is the restricted Damerau-Levenshtein distance - not a true metric. It does not allow for a substring to be modified by an elementarry operation twice
// I decided to go through with this because it is unlikely for a human to, say, make a transposition error AND a deletion/substitution error at the same location simultaneously
int damLevMatrix(char *s1, char *s2){
    //This implementation uses the dynamic approach to computing the metric instead of the recursive one - improves time complexity at minimal increase in space complexity
    int n = len(s1);
    int m = len(s2);
    //The matrix that will hold the Levenshtein distances
    int d[m+1][n+1];
    
    int i,j;
    //Initialising the outermost row and column
    for (i = 0; i <= m; ++i){
        d[i][0] = i;
    }
    for (i = 0; i <= n; ++i){
        d[0][i] = i;
    }
    
    int sub; //Keeps track of whether a substitution has occured or not
    for (i = 1; i <= n; ++i){
        for (j = 1; j <= m; ++j){
            if (*(s1+j-1) == *(s2+i-1)){
               sub = 0; //No substitution is required
            }
            else{
                //printf("Substitution cost at (%d,%d)\n",j,i);
                sub = SUBSCOST; //Fixed cost incurred by the substitution operation
            }
            //Uses the dynamically stored evaluations of the Levenshtein distances to satisfy the recursive definition of the distance at this cell
            d[j][i] = min3(
                d[j-1][i] + DELCOST, //Deletion operation is carried out
                d[j][i-1] + INSCOST, //Insertion operation is carried out
                d[j-1][i-1] + sub //Substition operation is carried out
            );
            //Checks whether a transposition operation matches the substring
            if (i > 1 && j > 1 && *(s1+j-1) == *(s2+i-2) && *(s1+j-2) == *(s2+i-1)){
                d[j][i] = (d[j][i] < (d[j-2][i-2] + TRANCOST)) ? d[j][i] : (d[j-2][i-2] + TRANCOST); //Transposition event is carried out
            }
        }
    }
    return d[m][n];
}