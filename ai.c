#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1000
#define MAX_TOKENS 10000

char* sanitize(const char* x) {
    char* result = malloc(strlen(x) + 1);
    int j = 0;
    for (int i = 0; x[i]; i++) {
        char c = tolower(x[i]);
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
            result[j++] = c;
        } else if (strchr("áàâäãåæ", c)) {
            result[j++] = 'a';
        } else if (strchr("çÇ", c)) {
            result[j++] = 'c';
        } else if (strchr("éèêë", c)) {
            result[j++] = 'e';
        } else if (strchr("íìîï", c)) {
            result[j++] = 'i';
        } else if (strchr("ñÑ", c)) {
            result[j++] = 'n';
        } else if (strchr("óòôöõøœ", c)) {
            result[j++] = 'o';
        } else if (strchr("úùûü", c)) {
            result[j++] = 'u';
        }
    }
    result[j] = '\0';
    return result;
}

char* strong_sanitize(const char* x) {
    char* result = malloc(strlen(x) + 1);
    int j = 0;
    for (int i = 0; x[i]; i++) {
        char c = tolower(x[i]);
        if (c >= 'a' && c <= 'z') {
            result[j++] = c;
        } else if (strchr("áàâäãåæ", c)) {
            result[j++] = 'a';
        } else if (strchr("çÇ", c)) {
            result[j++] = 'c';
        } else if (strchr("éèêë", c)) {
            result[j++] = 'e';
        } else if (strchr("íìîï", c)) {
            result[j++] = 'i';
        } else if (strchr("ñÑ", c)) {
            result[j++] = 'n';
        } else if (strchr("óòôöõøœ", c)) {
            result[j++] = 'o';
        } else if (strchr("úùûü", c)) {
            result[j++] = 'u';
        }
    }
    result[j] = '\0';
    return result;
}

typedef struct {
    char* token;
    int* indices;
    int count;
} UniqueToken;

UniqueToken* un_t = NULL;
UniqueToken* un_a = NULL;
UniqueToken* un_p = NULL;
int un_t_count = 0, un_a_count = 0, un_p_count = 0;

void add_unique_token(UniqueToken** tokens, int* count, const char* token, int index) {
    for (int i = 0; i < *count; i++) {
        if (strcmp((*tokens)[i].token, token) == 0) {
            (*tokens)[i].indices = realloc((*tokens)[i].indices, ((*tokens)[i].count + 1) * sizeof(int));
            (*tokens)[i].indices[(*tokens)[i].count++] = index;
            return;
        }
    }
    *tokens = realloc(*tokens, (*count + 1) * sizeof(UniqueToken));
    (*tokens)[*count].token = strdup(token);
    (*tokens)[*count].indices = malloc(sizeof(int));
    (*tokens)[*count].indices[0] = index;
    (*tokens)[*count].count = 1;
    (*count)++;
}

void process_tokens(char* line, UniqueToken** tokens, int* count, int index) {
    char* token = strtok(line, " /");
    while (token != NULL) {
        char* sanitized = sanitize(token);
        if (strcmp(sanitized, "a") != 0 && strcmp(sanitized, "an") != 0 && strcmp(sanitized, "the") != 0 &&
            strcmp(sanitized, "in") != 0 && strcmp(sanitized, "is") != 0 && strcmp(sanitized, "and") != 0 &&
            strcmp(sanitized, "or") != 0 && !isdigit(sanitized[0]) && strlen(sanitized) > 0) {
            add_unique_token(tokens, count, sanitized, index);
        }
        free(sanitized);
        token = strtok(NULL, " /");
    }
}

char* soundex(const char* x, int m) {
    char* hold = strong_sanitize(x);
    int len = strlen(hold);
    char* result = malloc(m + 1);
    result[0] = toupper(hold[0]);
    int r = 1;

    for (int i = 1; i < len && r < m; i++) {
        char code = '0';
        switch (hold[i]) {
            case 'b': case 'f': case 'p': case 'v': code = '1'; break;
            case 'c': case 'g': case 'j': case 'k': case 'q': case 's': case 'x': case 'z': code = '2'; break;
            case 'd': case 't': code = '3'; break;
            case 'l': code = '4'; break;
            case 'm': case 'n': code = '5'; break;
            case 'r': code = '6'; break;
        }
        if (code != '0' && code != result[r-1]) {
            result[r++] = code;
        }
    }

    while (r < m) {
        result[r++] = '0';
    }

    result[m] = '\0';
    free(hold);
    return result;
}

char* stitch(int* arr, int count) {
    char* result = malloc(count * 12);  // Assuming each number is at most 10 digits + 1 for '-' + 1 for '\0'
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        char temp[12];
        sprintf(temp, "%d-", arr[i]);
        strcat(result, temp);
    }
    result[strlen(result) - 1] = '\0';  // Remove the last '-'
    return result;
}

int main() {
    FILE* file = fopen("books-clean.csv", "r");
    if (file == NULL) {
        printf("Could not open file\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int line_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line_count > 0) {  // Skip header
            char* token = strtok(line, ",");
            int field = 0;
            while (token != NULL) {
                if (field == 1) {
                    process_tokens(token, &un_t, &un_t_count, line_count - 1);
                } else if (field == 2) {
                    process_tokens(token, &un_a, &un_a_count, line_count - 1);
                } else if (field == 4) {
                    process_tokens(token, &un_p, &un_p_count, line_count - 1);
                }
                token = strtok(NULL, ",");
                field++;
            }
        }
        line_count++;
    }

    fclose(file);

    printf("DONE\n");

    FILE* output = fopen("dict_soundex_trial.csv", "w");
    if (output == NULL) {
        printf("Could not open output file\n");
        return 1;
    }

    for (int i = 0; i < un_t_count; i++) {
        char* indices = stitch(un_t[i].indices, un_t[i].count);
        char* sdx = soundex(un_t[i].token, 5);
        fprintf(output, "%s,title,%s,%s\n", un_t[i].token, indices, sdx);
        free(indices);
        free(sdx);
    }

    for (int i = 0; i < un_a_count; i++) {
        char* indices = stitch(un_a[i].indices, un_a[i].count);
        char* sdx = soundex(un_a[i].token, 5);
        fprintf(output, "%s,author,%s,%s\n", un_a[i].token, indices, sdx);
        free(indices);
        free(sdx);
    }

    for (int i = 0; i < un_p_count; i++) {
        char* indices = stitch(un_p[i].indices, un_p[i].count);
        char* sdx = soundex(un_p[i].token, 5);
        fprintf(output, "%s,pub,%s,%s\n", un_p[i].token, indices, sdx);
        free(indices);
        free(sdx);
    }

    fclose(output);

    // Free allocated memory
    for (int i = 0; i < un_t_count; i++) {
        free(un_t[i].token);
        free(un_t[i].indices);
    }
    free(un_t);

    for (int i = 0; i < un_a_count; i++) {
        free(un_a[i].token);
        free(un_a[i].indices);
    }
    free(un_a);

    for (int i = 0; i < un_p_count; i++) {
        free(un_p[i].token);
        free(un_p[i].indices);
    }
    free(un_p);

    return 0;
}

