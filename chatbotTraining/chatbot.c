#include "chatbot.h"
#include "search-utils.h"
#include <math.h>

char** returnTokenList(char* input, int* count){
    char** tokens = (char**) malloc(100*sizeof(char*));
    for (int i = 0; i < 100; ++i){
        tokens[i] = (char*) malloc(50*sizeof(char));
    }
    int no_tokens = str_split(input, ' ', tokens);
    *(count) = no_tokens;
    char** san_tokens = (char**) malloc(no_tokens*sizeof(char*));
    for (int i = 0; i < no_tokens; ++i){
        san_tokens[i] = (char*) malloc(50*sizeof(char));
        sanitize(tokens[i], san_tokens[i]);
    }
    return san_tokens;
}

float mean(float* l,int le){
    float sum = 0;
    for (int i = 0; i < le; ++i){
        sum += l[i];
    }
    return sum/le;
}

float stdDeviation(float* l, int le){
    float m = mean(l,le);
    float deviations[le];
    for (int i = 0; i < le; ++i){
        deviations[i] = (m - l[i])*(m - l[i]);
    }
    float sum_dev = 0.0;
    for (int i = 0; i< le; ++i){
        sum_dev += deviations[i];
    }
    float std = sqrt(sum_dev/le);
    return std;
}

//Culmination of my work - Hail Satan
char* generateAnswer(char* input){
    int no_ans = 0;
    //Reading the context data structs from the file
    Context* cntx = (Context*) malloc(500*sizeof(Context));
    no_ans = readContextsFromFile("context-test.bin", cntx);
    //Reading the trie from the file
    TrieNode* root = createNode();
    loadTrieFromFile("trie-test.bin", root);
    //char* input = "how do I search for books?";
    int no_tokens = 0;
    char** tokens = returnTokenList(input, &no_tokens);
    float** token_weights = (float**) malloc(no_tokens*sizeof(float*));
    for (int i = 0; i < no_tokens; ++i){
        token_weights[i] = (float*) malloc(no_ans*sizeof(float));
        for (int j = 0; j < no_ans; ++j){
            token_weights[i][j] = 0.0;
        }
    }
    for (int i = 0; i < no_tokens; ++i){
        TrieNode* res = search(root, tokens[i]);
        //printf("SEARCH DONE for %s!\n",tokens[i]);
        for (int j = 0; j < no_ans; ++j){
            if (res == NULL){
                token_weights[i][j] = 0.0;
            }
            else{
                token_weights[i][j] = res->meaning[j];
            }
        }
    }
    float* total_weight = (float*) malloc(no_ans*sizeof(float));
    for (int i = 0; i < no_ans; ++i){
        total_weight[i] = 0.0;
    }
    for (int i = 0; i < no_ans; ++i){
        for (int j = 0; j < no_tokens; ++j){
            total_weight[i] += token_weights[j][i];
        }
    }
    free(token_weights);
    inPlaceNormalize(total_weight, no_ans);
    float** unsorted = (float**) malloc(no_ans*sizeof(float*));
    for (int i = 0; i < no_ans; ++i){
        unsorted[i] = (float*) malloc(2*sizeof(float));
        unsorted[i][0] = i;
        unsorted[i][1] = total_weight[i];
    }
    bubbleSortDescending(unsorted, no_ans, 2);
    //Now we find the standard deviation of the top result from the mean of the total_weight array
    int top = (int) unsorted[0][0];
    //printf("The ans is (predicted) -> %s\n",cntx[top].answer);
    freeTrie(root);
    float std = stdDeviation(total_weight, no_ans);
    float m = mean(total_weight, no_ans);
    float diff = unsorted[0][1] - m;
    char* ret = (char*) malloc(500*sizeof(char));
    if (diff < std/2 || isnan(unsorted[0][1])){
        ret = "I'm sorry, but I do not know how to answer that!";
    }
    else{
        strncpy(ret,cntx[top].answer,500);
    }
    free(unsorted);
    free(total_weight);
    //printf("ANSWER-> %s\n",ret);
    return ret;
}