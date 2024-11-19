#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXPRIME 1000 // Upper limit for prime numbers used in RSA

#include "rsa.h" // Header file containing function prototypes

// Function to encrypt a file using RSA
int encryptFile(char* file, char* out_file, int public_key, unsigned long prime_product) {
    FILE* fileptr = fopen(file, "r"); // Open input file in read mode
    FILE* outptr = fopen(out_file, "w"); // Open output file in write mode
    if (fileptr == NULL) {
        printf("FAILED TO OPEN\n");
        return 1; // Return error if file cannot be opened
    }
    char ch;
    while ((ch = fgetc(fileptr)) != EOF) { // Read each character from the input file
        int enc = encrypt(ch, public_key, prime_product); // Encrypt character
        fprintf(outptr, "%d\n", enc); // Write encrypted value to output file
    }
    fclose(fileptr);
    fclose(outptr);
    return 0; // Success
}

// Function to decrypt a file using RSA
int decryptFile(char* infile, char* outfile, int private_key, unsigned long prime_product) {
    FILE* inptr = fopen(infile, "r"); // Open encrypted file in read mode
    FILE* outptr = fopen(outfile, "w"); // Open output file in write mode
    if (inptr == NULL) {
        return 1; // Return error if file cannot be opened
    }
    while (!feof(inptr)) { // Read encrypted values from the input file
        int clear;
        fscanf(inptr, "%d\n", &clear); // Read each encrypted value
        fputc(decrypt(clear, private_key, prime_product), outptr); // Decrypt and write to output file
    }
    fclose(inptr);
    fclose(outptr);
    return 0; // Success
}

// Function to perform RSA encryption on a single value
unsigned long int encrypt(unsigned long int msg, int public_key, unsigned long prime_product) {
    int e = public_key; // Public key (exponent)
    unsigned long int enc = 1;
    while (e--) { // Perform modular exponentiation
        enc *= msg;
        enc %= prime_product;
    }
    return enc;
}

// Function to perform RSA decryption on a single value
unsigned long int decrypt(unsigned long int enc, int private_key, unsigned prime_product) {
    int d = private_key; // Private key (exponent)
    unsigned long int msg = 1;
    while (d--) { // Perform modular exponentiation
        msg *= enc;
        msg %= prime_product;
    }
    return msg;
}

// Function to generate RSA key pair (public and private keys)
unsigned long* setkeys() {
    short num[MAXPRIME];
    unsigned long int primes[1000];

    // Initialize the array for the Sieve of Eratosthenes
    unsigned long int i, j = 0;
    for (i = 0; i <= MAXPRIME; ++i) {
        num[i] = 1; // Assume all numbers are prime initially
    }
    num[0] = 0; // 0 is not prime
    num[1] = 0; // 1 is not prime

    // Sieve of Eratosthenes to find all primes up to MAXPRIME
    for (i = 2; i <= MAXPRIME; ++i) {
        if (num[i] == 0) continue; // Skip non-prime numbers
        for (j = i * 2; j <= MAXPRIME; j += i) {
            num[j] = 0; // Mark multiples as non-prime
        }
    }

    // Collect primes into the primes array
    unsigned long int c = 0;
    for (i = 0; i <= MAXPRIME; ++i) {
        if (num[i] == 1) {
            primes[c] = i;
            ++c;
        }
    }
    primes[c] = 0;

    // Select two random primes that are sufficiently far apart
    unsigned long int p = primes[returnRandom(c / 2, c)];
    unsigned long int q = primes[returnRandom(c / 2, c)];
    unsigned long int d = diff(p, q);
    while (d < 100) { // Ensure a significant difference between p and q
        p = primes[returnRandom(c / 2, c)];
        q = primes[returnRandom(c / 2, c)];
        d = diff(p, q);
    }

    unsigned long prime_product = p * q; // Calculate n = p * q
    unsigned long int* fi = (unsigned long int*) malloc(sizeof(unsigned long int));
    unsigned long int* e = (unsigned long int*) malloc(sizeof(unsigned long int));
    *fi = (p - 1) * (q - 1); // Calculate φ(n)
    *e = 2; // Start with the smallest possible public key exponent

    // Find a valid public key exponent e such that gcd(e, φ(n)) = 1
    while (1) {
        if (gcd(*fi, *e) == 1) {
            break;
        }
        ++(*e);
    }
    int public_key = *e;

    unsigned long int* priv = (unsigned long int*) malloc(sizeof(unsigned long int));
    *(priv) = 2;

    // Find a valid private key exponent d such that (d * e) % φ(n) = 1
    while (1) {
        if ((*(priv) * (*e)) % (*fi) == 1) {
            break;
        }
        ++(*priv);
    }
    int private_key = *(priv);

    // Return the key pair (public key, private key, prime product)
    unsigned long* key_pair = (unsigned long*) malloc(3 * sizeof(unsigned long));
    key_pair[0] = public_key;
    key_pair[1] = private_key;
    key_pair[2] = prime_product;
    return key_pair;
}

// Function to generate a random number in a range [min, max]
unsigned long int returnRandom(unsigned long int min, unsigned long int max) {
    unsigned int* seed = (unsigned int*) malloc(sizeof(unsigned int));
    *seed = time(0); // Seed with current time
    return (rand_r(seed) % (max - min + 1) + min); // Generate random number
}

// Function to calculate the absolute difference between two numbers
unsigned long int diff(unsigned long int x, unsigned long int y) {
    unsigned long int* d = (unsigned long int*) malloc(sizeof(unsigned long int));
    if (x > y)
        *d = x - y;
    else
        *d = y - x;
    return *d;
}

// Function to calculate the greatest common divisor (GCD)
unsigned long int gcd(unsigned long int x, unsigned long int y) {
    unsigned long int* temp = (unsigned long int*) malloc(sizeof(unsigned long int));
    *temp = 0;

    // Euclidean algorithm for GCD
    while (1) {
        *temp = x % y;
        if (*temp == 0) {
            free(temp);
            return y; // Return the GCD
        } else {
            x = y;
            y = *temp;
        }
    }
    free(temp);
    return -1; // Should never reach here
}
