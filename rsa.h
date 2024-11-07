#pragma once

#include <stdio.h>

unsigned long int returnRandom(unsigned long int min, unsigned long int max);
unsigned long int diff(unsigned long int x, unsigned long int y);
unsigned long int gcd(unsigned long int x, unsigned long int y);
int setkeys();
unsigned long int encrypt(unsigned long int msg);
unsigned long int decrypt(unsigned long int enc_text);