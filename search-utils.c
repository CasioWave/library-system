#include "search-utils.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char** string_arr_mallocer(int no_strings, int string_len){
    char** ret = (char**) malloc(no_strings*sizeof(char*));
    for (int i = 0; i < no_strings; ++i){
        ret[i] = (char*) malloc(string_len*sizeof(char));
    }
    return ret;
}

int hash_string(char *str){
    int ret = 0;
    for (int i = 0; *(str+i) != '\0'; ++i){
        ret += (int) *(str+i);
    }
    return ret;
}

int count_char(char *str, char ch){
    int i, count;
    count = 0;
    for (i = 0; *(str+i) != '\0'; ++i){
        if (ch == *(str+i)){
            ++count;
        }
    }
    return count;
}

int strip(char *str, char *ret){
    int left;
    for (left = 0; *(str+left) == ' '; ++left){
        ;
    }
    int x;
    int right = 0;
    int l = len(str);
    for (x = l-1; *(str+x) == ' '; --x){
        ++right;
    }
    //printf("Left->%d,Right->%d\n",left,right);
    int i, j;
    for (i = left, j = 0; i < l-right; ++i, ++j){
        *(ret+j) = *(str+i);
    }
    *(ret+j) = '\0';
    return 0;
}

int in(char c, const char *set) {
    while (*set) {
        if (c == *set) return 1;
        set++;
    }
    return 0;
}

int* in_where(int *l, int x, int length){
    int i, j;
    j = 0;
    int* loc = (int*) malloc(1000*sizeof(int));
    for (i = 0; i < length; ++i){
        if (l[i] == x){
            loc[j] = i;
            ++j;
        }
        else{
            continue;
        }
    }
    if (j == 0){
        loc[0] = -1;
        return loc;
    }
    loc[j] = -1;
    return loc;
}


// Function to swap two rows in a 2D array
void swapRows(float** arr, int i, int j, int cols) {
    for (int k = 0; k < cols; k++) {
        float temp = arr[i][k];
        arr[i][k] = arr[j][k];
        arr[j][k] = temp;
    }
}

// Bubble sort function to sort in descending order based on the 2nd coordinate
void bubbleSortDescending(float** arr, int rows, int cols) {
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < rows - i - 1; j++) {
            if (arr[j][1] < arr[j + 1][1]) { // Compare for descending order
                swapRows(arr, j, j + 1, cols);
            }
        }
    }
}

int in_str_list(char *str, char **l, int len_l){
    for (int i = 0; i < len_l; ++i){
        //printf("%d\n",i);
        if (str_equal(str, l[i])){
            return i;
        }
        else{
            continue;
        }
    }
    return -1;
}

int str_equal(char *s1, char *s2){
    int i = len(s1);
    int j = len(s2);
    if (i != j){
        return 0;
    }
    else{
        for (int x = 0; x < i; ++x){
            if (s1[x] != s2[x]){
                return 0;
            }
        }
    }
    return 1;
}

int sanitize(char *str, char *ret){
    int c, i;
    c = i = 0;
    while (*(str+c) != '\0'){
        //printf("%d\n",c);
        if ((*(str+c) <= 'Z' && *(str+c) >= 'A') || (*(str+c) <= 'z' && *(str+c) >= 'a')){
            //printf("character!\n");
            *(ret+i) = *(str+c);
            ++i;
        }
        else if (in(*(str+c),"áÁàÀâÂäÄãÃåÅæÆ")){
            *(ret+i) = 'a';
            ++i;
        }
        else if (in(*(str+c),"çÇ")){
            *(ret+i) = 'c';
            ++i;
        }
        else if (in(*(str+c),"éÉèÈêÊëË")){
            *(ret+i) = 'e';
            ++i;
        }
        else if (in(*(str+c),"íÍìÌîÎïÏ")){
            *(ret+i) = 'i';
            ++i;
        }
        else if (in(*(str+c),"ñÑ")){
            *(ret+i) = 'n';
            ++i;
        }
        else if (in(*(str+c),"óÓòÒôÔöÖõÕøØœŒ")){
            *(ret+i) = 'o';
            ++i;
        }
        else if (in(*(str+c),"úÚùÙûÛüÜ")){
            *(ret+i) = 'u';
            ++i;
        }
        else if (*(str+c) >= '0' && *(str+c) <= '9'){
            //printf("Number!\n");
            *(ret+i) = *(str+c);
            ++i;
        }
        ++c;
    }
    *(ret+i) = '\0';
    lower(ret,ret);
    return 0;
}

int copy_str(char *str, char *copy){
    int i;
    for (i = 0; *(str+i) != '\0'; ++i){
        *(copy+i) = *(str+i);
    }
    *(copy+i) = '\0';
    return 0;
}

int strong_sanitize(char *str, char *ret){
    int c, i;
    c = i = 0;
    while (*(str+c) != '\0'){
        //printf("%d\n",c);
        if ((*(str+c) <= 'Z' && *(str+c) >= 'A') || (*(str+c) <= 'z' && *(str+c) >= 'a')){
            //printf("character!\n");
            *(ret+i) = *(str+c);
            ++i;
        }
        else if (in(*(str+c),"áÁàÀâÂäÄãÃåÅæÆ")){
            *(ret+i) = 'a';
            ++i;
        }
        else if (in(*(str+c),"çÇ")){
            *(ret+i) = 'c';
            ++i;
        }
        else if (in(*(str+c),"éÉèÈêÊëË")){
            *(ret+i) = 'e';
            ++i;
        }
        else if (in(*(str+c),"íÍìÌîÎïÏ")){
            *(ret+i) = 'i';
            ++i;
        }
        else if (in(*(str+c),"ñÑ")){
            *(ret+i) = 'n';
            ++i;
        }
        else if (in(*(str+c),"óÓòÒôÔöÖõÕøØœŒ")){
            *(ret+i) = 'o';
            ++i;
        }
        else if (in(*(str+c),"úÚùÙûÛüÜ")){
            *(ret+i) = 'u';
            ++i;
        }
        ++c;
    }
    *(ret+i) = '\0';
    lower(ret,ret);
    return 0;
}

int lower(char *str, char *ret){
    int c;
    for (c = 0; *(str+c) != '\0'; ++c){
        *(ret+c) = tolower(*(str+c));
    }
    *(ret+c) = '\0';
    return 0;
}

int len(char *str){
    int c;
    for (c = 0; *(str+c) != '\0'; ++c){
        ;
    }
    return c;
}

int str_split(char *str, char delim, char** l){
    int i, j, k;
    j = k = 0;
    for (i = 0; i < len(str); ++i){
        if (str[i] == delim){
            l[k][j] = '\0';
            ++k;
            j = 0;
        }
        else{     
            l[k][j] = str[i];
            ++j;
        }
    }
    l[k][j] = '\0';
    return k+1;
}
