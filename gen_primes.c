#include <stdio.h>
#define MAXPRIME 100000

int main(){
    unsigned long int num[MAXPRIME];
    unsigned long int primes[MAXPRIME/100];

    //Initialise the array
    unsigned long int i,j = 0;
    
    for (i = 0; i <= MAXPRIME; ++i){
        num[i] = 1;
    }
    num[0] = 0;
    num[1] = 0;
    
    for (i = 2; i <= MAXPRIME; ++i){
        if (num[i] == 0)
            continue;
        for (j = i*2; j <= MAXPRIME; j += i){
            num[j] = 0;
        }
    }
    int c = 0;
    for (i = 0; i <= MAXPRIME; ++i){
        if (num[i] == 1){
            primes[c] = i;
            ++c;
        }
    }
    primes[c] = 0;
    
    FILE *pr;
    char str[MAXPRIME];
    
    pr = fopen("primes.dat","w");
    for (i = 0; primes[i] != 0; ++i){
        sprintf(str, "%ld", primes[i]);
        fprintf(pr, str);
        fprintf(pr, "\n");
    }
    return 0;
}