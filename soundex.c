#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search-utils.h"

int soundex_hash(const char *str, char *hash, int max) {
    if (str == NULL || hash == NULL || max <= 0) return -1;

    // Allocate buffers dynamically
    int str_len = strlen(str);
    char *s1 = malloc(str_len + 1);
    char *s2 = malloc(str_len + 1);
    if (!s1 || !s2) {
        fprintf(stderr, "Memory allocation failed\n");
        free(s1);
        free(s2);
        return -1;
    }

    // Step 1: Remove 'h', 'w', 'y' from the string
    int j = 0;
    s1[j++] = str[0];
    for (int i = 1; str[i] != '\0'; i++) {
        if (!in(str[i], "hwy")) {
            s1[j++] = str[i];
        }
    }
    s1[j] = '\0';

    // Step 2: Remove duplicates of adjacent letters with the same Soundex code
    j = 0;
    for (int i = 0; s1[i] != '\0'; i++) {
        if (i > 0 &&
            ((in(s1[i], "bfpv") && in(s1[i - 1], "bfpv")) ||
             (in(s1[i], "cgjkqsxz") && in(s1[i - 1], "cgjkqsxz")) ||
             (in(s1[i], "dt") && in(s1[i - 1], "dt")) ||
             (s1[i] == 'l' && s1[i - 1] == 'l') ||
             (in(s1[i], "mn") && in(s1[i - 1], "mn")) ||
             (s1[i] == 'r' && s1[i - 1] == 'r'))) {
            continue;
        }
        s2[j++] = s1[i];
    }
    s2[j] = '\0';

    // Step 3: Remove vowels (except the first character)
    j = 0;
    s1[j++] = s2[0];
    for (int i = 1; s2[i] != '\0'; i++) {
        if (!in(s2[i], "aeiou")) {
            s1[j++] = s2[i];
        }
    }
    s1[j] = '\0';

    // Step 4: Map characters to Soundex digits
    j = 0;
    s2[j++] = s1[0];
    for (int i = 1; s1[i] != '\0'; i++) {
        if (in(s1[i], "bfpv")) {
            s2[j++] = '1';
        } else if (in(s1[i], "cgjkqsxz")) {
            s2[j++] = '2';
        } else if (in(s1[i], "dt")) {
            s2[j++] = '3';
        } else if (s1[i] == 'l') {
            s2[j++] = '4';
        } else if (in(s1[i], "mn")) {
            s2[j++] = '5';
        } else if (s1[i] == 'r') {
            s2[j++] = '6';
        }
    }
    s2[j] = '\0';

    // Step 5: Truncate or pad the hash to the required length
    int i;
    for (i = 0; i < max && s2[i] != '\0'; i++) {
        hash[i] = s2[i];
    }
    for (; i < max; i++) {
        hash[i] = '0';
    }
    hash[max] = '\0';

    // Free allocated memory
    free(s1);
    free(s2);

    return 0;
}