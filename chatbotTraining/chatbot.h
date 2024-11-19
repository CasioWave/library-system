#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NO_CLASSES 6 //Redundant, was initially planned to be used for context continuation in the chatbot between prompts

#define ALPHABET_SIZE 26 //Required for the trie struct
#define FLOAT_LIST_SIZE 500

char** returnTokenList(char* input, int* count); //Takes a string, splits it by whitespace, sanitizes each token, and returns the list
float mean(float* l,int le); //calculates the mean of an array of floats
float stdDeviation(float* l, int le); //calculates the standard deviation of an array of floats


// Trie Node Structure - this is used to store and efficiently lookup the tokens and their weight vectors
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    float meaning[FLOAT_LIST_SIZE];
    int is_end_of_word; // Flag to indicate the end of a word
} TrieNode;
//This is the struct that holds the answer texts - since it would not be possible for us to generate them I thought it would be best to associate tokens with them
typedef struct Context {
    char answer[500];
    float vector[NO_CLASSES]; //Redundant, was planned for context continuation - not removing because it will break many things
} Context;
//These are described in chat-utils
void assignContext(Context* ctx, const char* answer, const float* vector); 
void writeContextsToFile(const char* filename, Context* contexts, int count);
int readContextsFromFile(const char* filename, Context* contexts);
void printContext(const Context* ctx);
//these are described in chat-utils
TrieNode* createNode();
void insert(TrieNode* root, const char* word, float meaning[FLOAT_LIST_SIZE]);
TrieNode* search(TrieNode* root, const char* word);
void saveTrie(FILE* file, TrieNode* node, char* buffer, int depth);
void saveTrieToFile(const char* filename, TrieNode* root);
void loadTrieFromFile(const char* filename, TrieNode* root);
void freeTrie(TrieNode* root);
//This is the struct that deals with the raw data from the trainer
typedef struct AnswerNode {
    char answer[500];
    float class_vector[NO_CLASSES];
    char prompts[50][200];
} AnswerNode;
//these are described in chat-utils
void scan(char* ret, int s);
void saveToFile(const char* filename, AnswerNode* nodes, int count);
AnswerNode* readFromFile(const char* filename, int* count);
void displayNodes(AnswerNode* nodes, int count);
void inPlaceNormalize(float* li, int l);
AnswerNode* createAnswer();
void prepareAnswer(char* ans, float* class_vec, char** prompts, AnswerNode* node);
