#include <stdlib.h>
#include <time.h>
#define MAXPRIME 1000
#include "rsa.h"

unsigned long int prime_product;
unsigned long int public_key, private_key;

unsigned long int encrypt(unsigned long int msg){
    int e = public_key;
    unsigned long int enc = 1;
    
    while (e--){
        enc *= msg;
        enc %= prime_product;
    }
    return enc;
}

unsigned long int decrypt(unsigned long int enc){
    int d = private_key;
    unsigned long int msg = 1;
    while (d--){
        msg *= enc;
        msg %= prime_product;
    }
    return msg;
}

int setkeys(){
    short num[MAXPRIME];
    unsigned long int primes[1000];

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
    unsigned long int c = 0;
    for (i = 0; i <= MAXPRIME; ++i){
        if (num[i] == 1){
            primes[c] = i;
            ++c;
        }
    }
    primes[c] = 0;
    
    //Selecting two random primes that are far apart
    
    unsigned long int p = primes[returnRandom(c/2,c)];
    unsigned long int q = primes[returnRandom(c/2,c)];
    
    unsigned long int d = diff(p,q);
    while (d < 100){
        p = primes[returnRandom(c/2,c)];
        q = primes[returnRandom(c/2,c)];
        d = diff(p,q);
    }
    
    
    prime_product = p*q;
    unsigned long int * fi = (unsigned long int*) malloc(sizeof(unsigned long int));
    unsigned long int* e = (unsigned long int*) malloc(sizeof(unsigned long int));
    *fi = (p-1)*(q-1);
    *e = 2;
    while (1){
        if (gcd(*fi,*e)==1){
            break;
        }
        ++e;
    }
    public_key = *e;
    unsigned long int* priv = (unsigned long int*) malloc(sizeof(unsigned long int));
    *(priv) = 2;
    while  (1){
        if ((*(priv)*(*e))%(*fi) == 1){
            break;
        }
        ++(*priv);
    }
    private_key = *(priv);
    return 0;
}

unsigned long int returnRandom(unsigned long int min, unsigned long int max){
    unsigned int* seed = (unsigned int*) malloc(sizeof(unsigned int));
    *seed = time(0);
    return (rand_r(seed) % (max-min+1) + min);
}

unsigned long int diff(unsigned long int x, unsigned long int y){
    unsigned long int * d = (unsigned long int *) malloc(sizeof(unsigned long int));
    if (x>y)
        *d = x-y;
    else
        *d = y-x;
    return *d;
}

unsigned long int gcd(unsigned long int x, unsigned long int y){
    unsigned long int * temp = (unsigned long int *) malloc(sizeof(unsigned long int));
    *temp = 0;
    
    while (1){
        *temp = x % y;
        if (*temp == 0){
            free(temp);
            return y;
        }
        else {
            x = y;
            y = *temp;
        }
    }
    free(temp);
    return -1;
}