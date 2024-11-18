#include "chatbot.h"

void scan(char* ret, int s){
    if (fgets(ret, s, stdin) != NULL) {
            // Remove the newline character if present
            size_t len = strlen(ret);
            if (len > 0 && ret[len - 1] == '\n') {
                ret[len - 1] = '\0';
            }
        }
}

// Function to save an array of AnswerNode structs to a file
void saveToFile(const char* filename, AnswerNode* nodes, int count) {
    FILE* file = fopen(filename, "ab");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // Write the number of structs first
    fwrite(&count, sizeof(int), 1, file);

    // Write each struct to the file
    for (int i = 0; i < count; i++) {
        fwrite(&nodes[i], sizeof(AnswerNode), 1, file);
    }

    fclose(file);
    printf("Data saved to '%s'.\n", filename);
}

// Function to read an array of AnswerNode structs from a file
AnswerNode* readFromFile(const char* filename, int* count) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }

    // Read the number of structs
    fread(count, sizeof(int), 1, file);

    // Allocate memory for the structs
    AnswerNode* nodes = (AnswerNode*)malloc((*count) * sizeof(AnswerNode));
    if (!nodes) {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    // Read each struct from the file
    fread(nodes, sizeof(AnswerNode), *count, file);

    fclose(file);
    printf("Data read from '%s'.\n", filename);
    return nodes;
}

// Helper function to display an array of AnswerNode structs
void displayNodes(AnswerNode* nodes, int count) {
    for (int i = 0; i < count; i++) {
        printf("AnswerNode %d:\n", i + 1);
        printf("  Answer: %s\n", nodes[i].answer);
        printf("  Class Vector: ");
        for (int j = 0; j < NO_CLASSES; j++) {
            printf("%.2f ", nodes[i].class_vector[j]);
        }
        printf("\n  Prompts:\n");
        for (int j = 0; j < 50 && nodes[i].prompts[j][0] != '\0'; j++) {
            printf("    %s\n", nodes[i].prompts[j]);
        }
        printf("\n");
    }
}

void inPlaceNormalize(float* li, int l){
    float sum = 0.0;
    for (int i = 0; i < l; ++i){
        sum += li[i];
    }
    for (int i = 0; i < l; ++i){
        li[i] = li[i]/sum;
    }
}

AnswerNode* createAnswer(){
    AnswerNode* node = (AnswerNode*) malloc(sizeof(AnswerNode));
    node->answer[0] = '\0';
    for (int i = 0; i < NO_CLASSES; ++i){
        node->class_vector[i] = 0.0;
    }
    for (int i = 0; i < 50; ++i){
        node->prompts[i][0] = '\0';
    }
    return node;
}

void prepareAnswer(char* ans, float* class_vec, char** prompts, AnswerNode* node){
    for (int i = 0; ans[i] != '\0'; ++i){
        node->answer[i] = ans[i];
        node->answer[i+1] = '\0';
    }
    for (int i = 0; i < NO_CLASSES; ++i){
        node->class_vector[i] = class_vec[i];
    }
    for (int i = 0; prompts[i][0] != '\0'; ++i){
        for (int j = 0; prompts[i][j] != '\0'; ++j){
            node->prompts[i][j] = prompts[i][j];
            node->prompts[i][j+1] = '\0';
        }
    }
}