#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long int returnRandom(unsigned long int min, unsigned long int max);

unsigned long int pickRandPrime();

int n;
int public_key, private_key;

int main(){
    
}

unsigned long int returnRandom(unsigned long int min, unsigned long int max){
    unsigned int seed = time(0);
    return (rand_r(&seed) % (max-min+1) + min);
}

unsigned long int pickRandPrime(){
    FILE *primes;
    
    primes = fopen("primes.dat","r");
    
    
}