#include "chatbot.h"

// Function to assign values to a Context struct
void assignContext(Context* ctx, const char* answer, const float* vector) {
    strncpy(ctx->answer, answer, sizeof(ctx->answer) - 1);
    ctx->answer[sizeof(ctx->answer) - 1] = '\0'; // Ensure null termination
    for (int i = 0; i < NO_CLASSES; ++i) {
        ctx->vector[i] = vector[i];
    }
}

// Function to write a list of Context structs to a binary file
void writeContextsToFile(const char* filename, Context* contexts, int count) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }
    fwrite(&count, sizeof(int), 1, file); // Write the count of structs
    fwrite(contexts, sizeof(Context), count, file);
    fclose(file);
    //printf("Contexts written to file successfully.\n");
}

// Function to read a list of Context structs from a binary file
int readContextsFromFile(const char* filename, Context* contexts) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        return 0;
    }
    int count;
    fread(&count, sizeof(int), 1, file); // Read the count of structs
    fread(contexts, sizeof(Context), count, file);
    fclose(file);
    //printf("Contexts read from file successfully.\n");
    return count;
}

// Function to print a Context struct for verification
void printContext(const Context* ctx) {
    printf("Answer: %s\n", ctx->answer);
    printf("Vector: ");
    for (int i = 0; i < NO_CLASSES; ++i) {
        printf("%.2f ", ctx->vector[i]);
    }
    printf("\n");
}

// Function to create a new Trie Node
TrieNode* createNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (!node) {
        perror("Failed to allocate memory for TrieNode");
        exit(1);
    }
    node->is_end_of_word = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL; //All root alphabet nodes point to null initially
    }
    for (int i = 0; i < FLOAT_LIST_SIZE; i++) {
        node->meaning[i] = 0.0; //Here, the meanings are the weight vectors, where every element of the vector is the probability that token word predicts the corresponding answer
    }
    return node;
}

// Function to insert a word and its meaning into the trie
void insert(TrieNode* root, const char* word, float meaning[FLOAT_LIST_SIZE]) {
    TrieNode* current = root;
    while (*word) {
        int index = *word - 'a'; // Assumes only lowercase letters - we sanitize strings before passing it to this
        if (index < 0 || index >= ALPHABET_SIZE) {
            printf("Invalid character '%c' in word. Skipping...\n", *word);
            return;
        }
        if (!current->children[index]) {
            current->children[index] = createNode();
        }
        current = current->children[index];
        word++;
    }
    current->is_end_of_word = 1; //terminates the word
    for (int i = 0; i < FLOAT_LIST_SIZE; i++) {
        current->meaning[i] = meaning[i];
    }
}

// Function to search for a word in the trie
TrieNode* search(TrieNode* root, const char* word) {
    TrieNode* current = root;
    while (*word) {
        int index = *word - 'a';
        if (index < 0 || index >= ALPHABET_SIZE || !current->children[index]) {
            return NULL; //Word not found in trie
        }
        current = current->children[index];
        word++;
    }
    return current->is_end_of_word ? current : NULL; //if the trie ends, sendd back the word
}

// Function to save the trie to a file - is used in the savetrietofile function
void saveTrie(FILE* file, TrieNode* node, char* buffer, int depth) {
    if (!node) return;

    if (node->is_end_of_word) {
        buffer[depth] = '\0';
        fprintf(file, "%s ", buffer);
        for (int i = 0; i < FLOAT_LIST_SIZE; i++) {
            fprintf(file, "%.2f ", node->meaning[i]);
        }
        fprintf(file, "\n");
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            buffer[depth] = 'a' + i;
            saveTrie(file, node->children[i], buffer, depth + 1);
        }
    }
}

// Function to save the entire trie to a file
void saveTrieToFile(const char* filename, TrieNode* root) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    char buffer[100];
    saveTrie(file, root, buffer, 0);

    fclose(file);
    //printf("Trie saved to file '%s'.\n", filename);
}

// Function to load the trie from a file
void loadTrieFromFile(const char* filename, TrieNode* root) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    char word[100];
    float meaning[FLOAT_LIST_SIZE];
    while (fscanf(file, "%s", word) != EOF) {
        for (int i = 0; i < FLOAT_LIST_SIZE; i++) {
            fscanf(file, "%f", &meaning[i]);
        }
        insert(root, word, meaning);
    }

    fclose(file);
    //printf("Trie loaded from file '%s'.\n", filename);
}

// Function to free the memory of the trie
void freeTrie(TrieNode* root) {
    if (!root) return;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(root->children[i]);
    }
    free(root);
}
//function to conveniently read string inputs from stdin
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
    //printf("Data saved to '%s'.\n", filename);
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
    //printf("Data read from '%s'.\n", filename);
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
//Normalises the array of floats given to it, so that the sum of all the elements in the array = 1
void inPlaceNormalize(float* li, int l){
    float sum = 0.0;
    for (int i = 0; i < l; ++i){
        sum += li[i];
    }
    for (int i = 0; i < l; ++i){
        li[i] = li[i]/sum;
    }
}
///Mallocs and initialises an AnswerNode struct
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
//Pushes information into an AnswerNode struct
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