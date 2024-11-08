#include <stdio.h>

#define SUBSCOST 1
#define DELCOST 1
#define INSCOST 1
#define TRANCOST 1

int len(char str[]);
int damLevMatrix(char s1[], char s2[]);
int min3(int a, int b, int c);

int main(){
    printf("%d\n", damLevMatrix("apple", "appel"));
    return 0;
}

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

int len(char *str){
    int c;
    for (c = 0; *(str+c) != '\0'; ++c){
        ;
    }
    return c;
}
// This is the restricted Damerau-Levenshtein distance - not a true metric
int damLevMatrix(char *s1, char *s2){
    int n = len(s1);
    int m = len(s2);
    
    int d[m+1][n+1];
    
    int i,j;
    
    for (i = 0; i <= m; ++i){
        d[i][0] = i;
    }
    for (i = 0; i <= n; ++i){
        d[0][i] = i;
    }
    
    int sub;
    for (i = 1; i <= n; ++i){
        for (j = 1; j <= m; ++j){
            if (*(s1+j-1) == *(s2+i-1)){
               sub = 0; 
            }
            else{
                //printf("Substitution cost at (%d,%d)\n",j,i);
                sub = SUBSCOST;
            }
            d[j][i] = min3(
                d[j-1][i] + DELCOST,
                d[j][i-1] + INSCOST,
                d[j-1][i-1] + sub
            );
            if (i > 1 && j > 1 && *(s1+j-1) == *(s2+i-2) && *(s1+j-2) == *(s2+i-1)){
                d[j][i] = (d[j][i] < (d[j-2][i-2] + TRANCOST)) ? d[j][i] : (d[j-2][i-2] + TRANCOST);
            }
        }
    }
    return d[m][n];
}