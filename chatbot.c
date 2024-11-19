#include "chatbot.h"
#include "search-utils.h"
#include <math.h> //Required just for sqrt() REMEMBER TO ADD -lm flag when compiling

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

//Culmination of my work - this function takes an input and generates an output text from the corpus of answers that it already knows
char* generateAnswer(char* input){
    int no_ans = 0; //No of answers that will be read from the Context file
    //Reading the context data structs from the file
    Context* cntx = (Context*) malloc(500*sizeof(Context)); //Holds the context structs
    no_ans = readContextsFromFile("context-test.bin", cntx); //reading the context structs from the file
    //Reading the trie from the file
    TrieNode* root = createNode(); //Root node for the trie struct
    loadTrieFromFile("trie-test.bin", root); //reading the processed trie from the file
    //char* input = "how do I search for books?";
    int no_tokens = 0;
    char** tokens = returnTokenList(input, &no_tokens); //get the tokens from the input string
    float** token_weights = (float**) malloc(no_tokens*sizeof(float*)); //Will hold the weight vectors for each token from the trie lookup
    for (int i = 0; i < no_tokens; ++i){
        token_weights[i] = (float*) malloc(no_ans*sizeof(float));
        for (int j = 0; j < no_ans; ++j){
            token_weights[i][j] = 0.0;
        }
    }
    for (int i = 0; i < no_tokens; ++i){
        TrieNode* res = searchTrie(root, tokens[i]);
        //printf("SEARCH DONE for %s!\n",tokens[i]);
        for (int j = 0; j < no_ans; ++j){
            if (res == NULL){ //The token was not found in the trie
                token_weights[i][j] = 0.0;
            }
            else{ //Token was found, getting its weight vectors
                token_weights[i][j] = res->meaning[j];
            }
        }
    }
    float* total_weight = (float*) malloc(no_ans*sizeof(float));
    for (int i = 0; i < no_ans; ++i){
        total_weight[i] = 0.0;
    }
    //We add the weight vectors of all the individual tokens from the input string to get the total weight vector
    //This vector when normalised essentially tells us what the probability of each answer being the correct/appropriate answer is
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
    //We take the answers (IDed by their position in list of Context structs), attach it to the corresponding computed total weight, and then sort the 2D array by the 2nd axis (weights)
    bubbleSortDescending(unsorted, no_ans, 2);
    //Now we find the standard deviation of the top result from the mean of the total_weight array
    //This is important, as for one particular output to be strongly preferred the standard deviation in the probabilities must be high
    int top = (int) unsorted[0][0]; //The index of the preferred answer
    //printf("The ans is (predicted) -> %s\n",cntx[top].answer);
    freeTrie(root);
    float std = stdDeviation(total_weight, no_ans);
    float m = mean(total_weight, no_ans); //find the central tendency of the weights
    float diff = unsorted[0][1] - m; //Find out how far the predicted answer's weight is from the central tendency of the weight vector
    char* ret = (char*) malloc(500*sizeof(char));
    if (diff < std/2 || isnan(unsorted[0][1])){ //If the ffirst condition is true, it means that the weights are too "fuzzy" and nothing can be predicted for certain. If the second is true, it means that none of the tokens from the input were found in the dictionary (resulting in a div by 0, resulting in NaN)
        ret = "I'm sorry, but I do not know how to answer that!"; //Answer if it can not find the preferred answer reliably
    }
    else{
        strncpy(ret,cntx[top].answer,500); //The top answer is taken
    }
    free(unsorted);
    free(total_weight);
    //printf("ANSWER-> %s\n",ret);
    return ret; //returning the most probably answer - we are done!
}