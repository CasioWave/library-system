#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#define NO_CLASSES 6
//#include "synonyms.h"

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
