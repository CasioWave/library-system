#include "chatbot.h"

char classes[NO_CLASSES][10] = {"ui","help","random","niceties","stats","function"}; //redundant, was originally planned to be used for context contionuation

int main(){
    AnswerNode* crop = (AnswerNode*) malloc(100*sizeof(AnswerNode)); //trainer can handle a max of 100 answers per round
    int no_crop = 0; //No of answers that were input
    printf("Welcome to CHATBOT TRAINER VER 0.1! \n\n");
    while(1){
        int c1 = 0;
        //printf("1 = Print the categories of conversation\n"); //removed as it made training cumbersome
        printf("2 = Start conversation training\n");
        printf("3 = Quit \n");
        printf("Enter choice -> ");
        scanf("%d",&c1);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { } // Flush the buffer
        if (c1 == 3){
            break;
        }
        else if (c1 == 1){
            for (int i = 0; i < NO_CLASSES; ++i){
                printf("%s\n",classes[i]);
            }
            printf("---------------\n");
            continue;
        }
        else if (c1 == 2){
            printf("If you type 'q' at 1st input prompt, prog quits\n"); //While writing the main prompt of the extra prompts
            while (1){
                char** pro = (char**) malloc(50*sizeof(char*));
                for (int i = 0; i < 50; ++i){
                    pro[i] = (char*) malloc(200*sizeof(char));
                    //pro[i][0] = '\0';
                }
                char* ans = (char*) malloc(500*sizeof(char));
                float* class_vec = (float*) malloc(NO_CLASSES*sizeof(float));
                for (int i = 0; i < NO_CLASSES; ++i){
                    class_vec[i] = 0.0;
                }
                printf("Enter prompt -> ");
                char* in = (char*) malloc(200*sizeof(char));
                scan(in,200);
                strncpy(pro[0],in,200);
                free(in);
                if (pro[0][0] == 'q'){
                    break;
                }
                printf("\nEnter the answer the bot should give -> ");
                scan(ans,500); //The answer can be at max be 500 characters
                printf("\n Are there other ways to ask the same prompt? (Enter 'q' to exit)\n");
                int count = 1;
                while (1){
                    printf("-> ");
                    char* input = (char*) malloc(200*sizeof(char));
                    scan(input,200);
                    if (input[0] == 'q'){
                        break;
                    }
                    else{
                        strncpy(pro[count], input, 200);
                        ++count;
                    }
                    printf("\n");
                }
                printf("\n");
                /* //Code that was made irrelevant by cutting out the context continuation feature
                printf("Now, give a score (out of 10) to each category depending on how relevant this prompt is to it - \n");
                for (int i = 0; i < NO_CLASSES; ++i){
                    int sc = 0;
                    printf("%s -> ",classes[i]);
                    scanf("%d",&sc);
                    while ((c = getchar()) != '\n' && c != EOF) { }
                    if (sc > 10 || sc < 0){
                        --i;
                        printf("Try that again moron\n");
                        continue;
                    }
                    class_vec[i] = (float) sc;
                    printf("\n");
                }
                */
                printf("This answer is done!\n");
                //inPlaceNormalize(class_vec, NO_CLASSES); //since they are not being assigned, no need to normalise
                crop[no_crop] = *createAnswer();
                prepareAnswer(ans, class_vec, pro, &crop[no_crop]);
                free(pro);
                free(ans);
                free(class_vec);
                ++no_crop;
            }
        }
        else{
            printf("Wrong input!\n");
            printf("-----------------\n");
            continue;
        }
    }
    if (no_crop == 0){
        return 0;
    }
    
    printf("A total of %d answers were created\n",no_crop);
    //Now, I will have to create two files - 
    //1 -> contexts.txt: This will store the answer strings and the class vectors fo them
    //2 -> trie.txt: This will store the tries and the weight vectors (weight to each answer text) assigned to each word
    //Here, I will just write the answerNode structs to file so that i can do what I want to with the training data later
    printf("Here are the answerNodes for review ->\n");
    displayNodes(crop, no_crop);
    saveToFile("chat-test.txt", crop, no_crop);
    return 0;
}