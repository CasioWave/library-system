#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXPRIME 1000000

unsigned long int returnRandom(unsigned long int min, unsigned long int max);
unsigned long int diff(unsigned long int x, unsigned long int y);
unsigned long int gcd(unsigned long int x, unsigned long int y);

unsigned long int n;
unsigned long int public_key, private_key;

int main(){
    short num[MAXPRIME];
    unsigned long int primes[100000];

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
    
    while (d < 1000){
        p = primes[returnRandom(c/2,c)];
        q = primes[returnRandom(c/2,c)];
        d = diff(p,q);
    }
    
    n = p*q;
    unsigned long int fi = (p-1)*(q-1);
    
}

unsigned long int returnRandom(unsigned long int min, unsigned long int max){
    unsigned int seed = time(0);
    return (rand_r(&seed) % (max-min+1) + min);
}

unsigned long int diff(unsigned long int x, unsigned long int y){
    unsigned long int d;
    if (x>y)
        d = x-y;
    else
        d = y-x;
    return d
}

unsigned long int gcd(unsigned long int x, unsigned long int y){
    if (x == 1 || y == 1){
        return 1;
    }
    else {
        unsigned long int temp = 0;
        while (1){
            
        }
    }
}