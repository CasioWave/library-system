# RSA Encryption Program Documentation

## Overview
This program implements basic RSA encryption and decryption, providing functionality to encrypt and decrypt files. It also generates RSA key pairs using random prime numbers and performs modular arithmetic for encryption and decryption operations.

## Files and Functions

### **File Encryption/Decryption**
- **`encryptFile(char* file, char* out_file, int public_key, unsigned long prime_product)`**
  - Encrypts a text file character by character using the provided public key and prime product (`n`).
  - **Inputs:**  
    - `file`: Path to the input file (plaintext).  
    - `out_file`: Path to the output file (encrypted).  
    - `public_key`: RSA public key (`e`).  
    - `prime_product`: Product of two primes (`n`).

- **`decryptFile(char* infile, char* outfile, int private_key, unsigned long prime_product)`**
  - Decrypts an encrypted file using the provided private key and prime product (`n`).
  - **Inputs:**  
    - `infile`: Path to the input file (encrypted).  
    - `outfile`: Path to the output file (decrypted plaintext).  
    - `private_key`: RSA private key (`d`).  
    - `prime_product`: Product of two primes (`n`).

### **Encryption/Decryption Operations**
- **`encrypt(unsigned long int msg, int public_key, unsigned long prime_product)`**
  - Encrypts a single integer (`msg`) using modular exponentiation:  
    \[
    \text{encrypted\_msg} = \text{msg}^e \mod n
    \]

- **`decrypt(unsigned long int enc, int private_key, unsigned long prime_product)`**
  - Decrypts a single encrypted integer (`enc`) using modular exponentiation:  
    \[
    \text{decrypted\_msg} = \text{enc}^d \mod n
    \]

### **Key Generation**
- **`setkeys()`**
  - Generates an RSA key pair (public key, private key) and the prime product (`n`).
  - **Returns:**  
    An array containing:  
    - Public key (`e`)  
    - Private key (`d`)  
    - Prime product (`n`)

### **Utility Functions**
- **`returnRandom(unsigned long int min, unsigned long int max)`**
  - Generates a random integer in the range `[min, max]`.

- **`diff(unsigned long int x, unsigned long int y)`**
  - Returns the absolute difference between two integers.

- **`gcd(unsigned long int x, unsigned long int y)`**
  - Calculates the greatest common divisor (GCD) using the Euclidean algorithm.

### **Prime Number Generation**
- **Sieve of Eratosthenes**
  - Used within `setkeys()` to identify prime numbers up to `MAXPRIME`.
  - Two random primes are selected for key generation.

## Constants
- **`MAXPRIME`**
  - The upper limit for prime numbers used in the key generation process. Defaults to 1000.

## How to Use
1. **Key Generation:**
   - Call `setkeys()` to generate the public/private key pair and `n`.
   
2. **Encryption:**
   - Use `encryptFile()` to encrypt a plaintext file with the public key (`e`) and `n`.

3. **Decryption:**
   - Use `decryptFile()` to decrypt the file using the private key (`d`) and `n`.

## Example Workflow
1. Generate keys:
   ```c
   unsigned long* keys = setkeys();
   int public_key = keys[0];
   int private_key = keys[1];
   unsigned long prime_product = keys[2];
   ```
2. Encrypt a file:
   ```c
   encryptFile("plaintext.txt", "encrypted.txt", public_key, prime_product);
   ```
3. Decrypt the file:
   ```c
   decryptFile("encrypted.txt", "decrypted.txt", private_key, prime_product);
   ```

## Notes
- The code assumes small keys for simplicity. In real-world scenarios, use larger primes and more secure key lengths.
- Proper memory management (e.g., freeing allocated memory) is essential to avoid memory leaks.