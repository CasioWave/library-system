#include "soundex.h"

int soundex_hash(char *str, char *hash, int max){
    //Assume that str has been strong sanitized
    char s1[len(str)];
    int j = 1;
    int i;
    //Remove hwy from the string and copy it to s1
    *(s1+0) = *(str+0);
    for (i = 1; *(str+i) != '\0'; ++i){
        if (in(*(str+i),"hwy")){
            ;
        }
        else{
            *(s1+j) = *(str+i);
            ++j;
        }
    }
    *(s1+j) = '\0';
    
    char s2[len(s1)];
    j = 0;
    //Remove the second letter when two adjacent letters have same number
    for (i = 0; *(s1+i) != '\0'; ++i){
        if (in(*(s1+i),"bfpv") && in(*(s1+i+1),"bfpv")){
            *(s2+j) = *(s1+i);
            ++i;
            ++j;
        }
        else if (in(*(s1+i),"cgjkqsxz") && in(*(s1+i+1),"cgjkqsxz")){
            *(s2+j) = *(s1+i);
            ++i;
            ++j;
        }
        else if (in(*(s1+i),"dt") && in(*(s1+i+1),"dt")){
            *(s2+j) = *(s1+i);
            ++i;
            ++j;
        }
        else if (*(s1+i) == 'l' && *(s1+i+1) == 'l'){
            *(s2+j) = *(s1+i);
            ++i;
            ++j;
        }
        else if (in(*(s1+i),"mn") && in(*(s1+i+1),"mn")){
            *(s2+j) = *(s1+i);
            ++i;
            ++j;
        }
        else if (*(s1+i) == 'r' && *(s1+i+1) == 'r'){
            *(s2+j) = *(s1+i);
            ++i;
            ++j;
        }
        else{
            *(s2+j) = *(s1+i);
            ++j;
        }
    }
    *(s2+j) = '\0';
    //Now we remove the vowels and copy to s1
    i = j = 0;
    *(s1+0) = *(s2+0);
    j = 1;
    for (i = 1; *(s2+i) != '\0'; ++i){
        if (in(*(s2+i),"aeiou")){
            ;
        }
        else{
            *(s1+j) = *(s2+i);
            ++j;
        }
    }
    *(s1+j) = '\0';
    //Now that the vowels and the repeats have been removed, we assign numbers and copy to s2
    i = j = 0;
    j = 1;
    *(s2+0) = *(s1+0);
    for (i = 1; *(s1+i) != '\0'; ++i){
        if (in(*(s1+i),"bfpv")){
            *(s2+j) = '1';
            ++j;
        }
        else if (in(*(s1+i),"cgjkqsxz")){
            *(s2+j) = '2';
            ++j;
        }
        else if (in(*(s1+i),"dt")){
            *(s2+j) = '3';
            ++j;
        }
        else if (*(s1+i) == 'l'){
            *(s2+j) = '4';
            ++j;
        }
        else if (in(*(s1+i),"mn")){
            *(s2+j) = '5';
            ++j;
        }
        else if (*(s1+i) == 'r'){
            *(s2+j) = '6';
            ++j;
        }
    }
    *(s2+j) = '\0';
    //Truncate and copy to hash function
    for (i = 0; i < max; ++i){
        if (*(s2+i) == '\0'){
            break;
        }
        *(hash+i) = *(s2+i);
    }
    for (; i < max; ++i){
        *(hash+i) = '0';
    }
    *(hash+i) = '\0';
    return 0;
}