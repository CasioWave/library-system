#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NO_CLASSES 6

#define ALPHABET_SIZE 26
#define FLOAT_LIST_SIZE 6

// Trie Node Structure
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    float meaning[FLOAT_LIST_SIZE];
    int is_end_of_word; // Flag to indicate the end of a word
} TrieNode;

typedef struct Context {
    char answer[500];
    float vector[NO_CLASSES];
} Context;

TrieNode* createNode();
void insert(TrieNode* root, const char* word, float meaning[FLOAT_LIST_SIZE]);
TrieNode* search(TrieNode* root, const char* word);
void saveTrie(FILE* file, TrieNode* node, char* buffer, int depth);
void saveTrieToFile(const char* filename, TrieNode* root);
void loadTrieFromFile(const char* filename, TrieNode* root);
void freeTrie(TrieNode* root);

typedef struct AnswerNode {
    char answer[500];
    float class_vector[NO_CLASSES];
    char prompts[50][200];
} AnswerNode;

void scan(char* ret, int s);
void saveToFile(const char* filename, AnswerNode* nodes, int count);
AnswerNode* readFromFile(const char* filename, int* count);
void displayNodes(AnswerNode* nodes, int count);
void inPlaceNormalize(float* li, int l);
AnswerNode* createAnswer();
void prepareAnswer(char* ans, float* class_vec, char** prompts, AnswerNode* node);
