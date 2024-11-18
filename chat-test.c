#include "chatbot.h"
#include <stdio.h>

int main(){
    char* query = "How do I search?";
    char* ans = generateAnswer(query);
    printf("ANS-> %s\n",ans);
}