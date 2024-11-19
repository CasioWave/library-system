#include "chatbot.h"
#include "search-utils.h"

int main(){
    AnswerNode* dict_ans; //List of the AnswerNode structs that will hold the data
    int count = 0; // Will contain the number of AnswerNode structs
    dict_ans = readFromFile("chat-test.txt", &count); //Reads the AnswerNode structs from the file created by trainer
    displayNodes(dict_ans,count); //Displays the nodes in a formatted fashion for verification
    //Now, we start processing the data into tries and context files
    //First, trie
    TrieNode* root = createNode(); //Creates TrieNode that will hold the dictionary of weight vectors
    char** unique_tokens = (char**) malloc(1000*sizeof(char*)); //Will hold all unique tokens from the processing of AnswerNodes
    for (int i = 0; i < 1000; ++i){
        unique_tokens[i] = (char*) malloc(50*sizeof(char));
        unique_tokens[i][0] = '\0';
    }
    float** weights = (float**) malloc(1000*sizeof(float*)); //Holds the weights of the individual tokens
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
            tokens[j][0] = '\0'; //terminating the token
        }
        for (int z = 0; dict_ans[i].prompts[z][0] != '\0'; ++z){
            int no_tokens = str_split(dict_ans[i].prompts[z], ' ', tokens);
            char** san_tokens = (char**) malloc(no_tokens*sizeof(char*));
            for (int j = 0; j < no_tokens; ++j){
                san_tokens[j] = (char*) malloc(50*sizeof(char));
                sanitize(tokens[j],san_tokens[j]); //sanitizing the token for standardisation
            }
            for (int j = 0; j < no_tokens; ++j){
                int loc = in_str_list(san_tokens[j], unique_tokens, no_unique);
                if (loc == -1){
                    printf("NEW TOKEN FOUND! %s\n",san_tokens[j]); //New token from AnswerNode
                    strncpy(unique_tokens[no_unique],san_tokens[j],50);
                    weights[no_unique][i] += 1; //Note, the weight of every Answer increases by 1 for every instance of a token found
                    ++no_unique;
                }
                else{
                    weights[loc][i] += 1; //Updates score for tokens that have been found before
                }
            }
        }
        
    }
    for (int i = 0; i < no_unique; ++i){ //Normalises the weights
        inPlaceNormalize(weights[i], count);
    }
    //List of unique tokens and their weights have been prepared. Now we make the trie.
    for (int i = 0; i < no_unique; ++i){
        insert(root, unique_tokens[i], weights[i]); //Insert a word/token into the trie struct
        printf("Added the unique token -> %s\n",unique_tokens[i]);
    }
    saveTrieToFile("trie-test.bin", root); //Saves the processed trie to file so that chatbot can use it
    freeTrie(root);
    //Now we form the list of Context structs and write them to a file
    Context* cntx = (Context*) malloc(count*sizeof(Context)); //Originally, I meant for there to be context awareness, and hence the class_vectors. We scrapped it later - due to the fact that we do not have enough data to train on
    for (int i = 0; i < count; ++i){
        assignContext(&cntx[i], dict_ans[i].answer, dict_ans[i].class_vector); //Creates the Context struct (Read, the answer struct)
    }
    writeContextsToFile("context-test.bin", cntx, count); //Writes the context structs to file so that the chatbot can use it
    printf("DONE!\n"); //The data is processed and ready for use with the chatbot!
    return 0;
}