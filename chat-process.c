#include "chatbot.h"
#include "search-utils.h"

int main(){
    AnswerNode* dict_ans;
    int count = 0;
    dict_ans = readFromFile("chat-test.txt", &count);
    displayNodes(dict_ans,count);
    //Now, we start processing the data into tries and context files
    //First, trie
    TrieNode* root = createNode();
    char** unique_tokens = (char**) malloc(1000*sizeof(char*));
    for (int i = 0; i < 1000; ++i){
        unique_tokens[i] = (char*) malloc(50*sizeof(char));
        unique_tokens[i][0] = '\0';
    }
    float** weights = (float**) malloc(1000*sizeof(float*));
    for (int i = 0; i < 1000; ++i){
        weights[i] = (float*) malloc(count*sizeof(float));
        for (int j = 0; j < count; ++j){
            weights[i][j] = 0.0;
        }
    }
    
    //Will have to loop over all the AnswerNodes, find all unique tokens, assign +1 score for every time a token appears for a given Answer, normalise this score list, and write to trie
    int no_unique = 0;
    for (int i = 0; i < count; ++i){
        char** tokens = (char**) malloc(100*sizeof(char*));
        for(int j = 0; j < 100; ++j){
            tokens[j] = (char*) malloc(50*sizeof(char));
            tokens[j][0] = '\0';
        }
        for (int z = 0; dict_ans[i].prompts[z][0] != '\0'; ++z){
            int no_tokens = str_split(dict_ans[i].prompts[z], ' ', tokens);
            char** san_tokens = (char**) malloc(no_tokens*sizeof(char*));
            for (int j = 0; j < no_tokens; ++j){
                san_tokens[j] = (char*) malloc(50*sizeof(char));
                sanitize(tokens[j],san_tokens[j]);
            }
            for (int j = 0; j < no_tokens; ++j){
                int loc = in_str_list(san_tokens[j], unique_tokens, no_unique);
                if (loc == -1){
                    printf("NEW TOKEN FOUND! %s\n",san_tokens[j]);
                    strncpy(unique_tokens[no_unique],san_tokens[j],50);
                    weights[no_unique][i] += 1;
                    ++no_unique;
                }
                else{
                    weights[loc][i] += 1;
                }
            }
        }
        
    }
    for (int i = 0; i < no_unique; ++i){
        inPlaceNormalize(weights[i], count);
    }
    //List of unique tokens and their weights have been prepared. Now we make the trie.
    for (int i = 0; i < no_unique; ++i){
        insert(root, unique_tokens[i], weights[i]);
        printf("Added the unique token -> %s\n",unique_tokens[i]);
    }
    saveTrieToFile("trie-test.bin", root);
    TrieNode* res = search(root, "hello");
    for (int i = 0; i < count; ++i){
        printf("Weight for %d answer -> %.2f\n",i,(double)res->meaning[i]);
    }
    return 0;
}