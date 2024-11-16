#include "preprocess.h"

char* stitch(int* arr, int count) {
    char* result = malloc(count * 12);  // Assuming each number is at most 10 digits + 1 for '-' + 1 for '\0'
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        char temp[12];
        sprintf(temp, "%d-", arr[i]);
        strcat(result, temp);
    }
    result[strlen(result) - 1] = '\0';  // Remove the last '-'
    return result;
}

int preprocess() {
    FILE* file = fopen("books-clean.csv", "r");
    if (file == NULL) {
        printf("Could not open file\n");
        return -1;
    }
    char** forbid = (char**) malloc(7*sizeof(char*));
    forbid[0] = "a";
    forbid[1] = "and";
    forbid[2] = "the";
    forbid[3] = "or";
    forbid[4] = "an";
    forbid[5] = "is";
    forbid[6] = "in";
    char* line = (char*) malloc(MAX_LINE_LENGTH*sizeof(char));
    int line_count = 0;
    char** col = string_arr_mallocer(7, 500);
    
    char** tokens = string_arr_mallocer(MAX_TOKENS, 50);
    int** indices = (int**) malloc(MAX_TOKENS*sizeof(int*));
    for (int i = 0; i < MAX_TOKENS; ++i){
        indices[i] = (int*) malloc(1000*sizeof(int));
    }
    int* indices_end = (int*) malloc(MAX_TOKENS*sizeof(int));
    for (int i = 0; i < MAX_TOKENS; ++i){
        indices_end[i] = 0;
    }
    char** types = string_arr_mallocer(MAX_TOKENS, 10);
    char** soundex = string_arr_mallocer(MAX_TOKENS, 10);
    int* no_unique_tokens = (int*) malloc(sizeof(int));
    *(no_unique_tokens) = 0;
    
    while (fgets(line, 10000, file)) {
        if (line_count > 0) {  // Skip header
            printf("%s\n",line);
            str_split(line, ',', col);
            
            //Processing tokens from the TITLE - has to be repeated for author and publisher
            char** token_split = string_arr_mallocer(100, 100);
            char** interm = string_arr_mallocer(100, 100);
            int no_titles = str_split(col[1], '/', interm);
            int counter = 0;
            for (int i = 0; i < no_titles; ++i){
                char** l = string_arr_mallocer(100, 100);
                int x = str_split(interm[i], ' ', l);
                for (int j = counter; j < counter+x; ++j){
                    copy_str(l[j], token_split[j]);
                }
                counter += x;
            }
            char** san_token_split = string_arr_mallocer(counter, 100);
            char** strong_san_token_split = string_arr_mallocer(counter, 100);
            
            //Sanitizing the strings
            for (int i = 0; i < counter; ++i){
                sanitize(token_split[i], san_token_split[i]);
                strong_sanitize(token_split[i], strong_san_token_split[i]);
            }
            
            //Looping over the sanitized tokens from the line
            for (int i = 0; i < counter; ++i){
                if (isdigit(san_token_split[i][0])){
                    continue;
                }
                else if (san_token_split[i][0] == '\0'){
                    continue;
                }
                else if (in_str_list(san_token_split[i],forbid,7) != -1){
                    continue;
                }
                //printf("HERE! - %s (title san_token)\n",san_token_split[i]);
                int loc = in_str_list(san_token_split[i], tokens, *no_unique_tokens);
                
                if (loc == -1){
                    printf("NEW TITLE TOKEN! -> %s\n",san_token_split[i]);
                    //New unique token found
                    soundex_hash(strong_san_token_split[i], soundex[*no_unique_tokens], 5); // Creating the new soundex hash
                    //printf("SOUNDEX ASSIGNED\n");
                    printf("no_unique_tokens:%d\n",*no_unique_tokens);
                    copy_str(san_token_split[i], tokens[*no_unique_tokens]); //Copying the token that is unique
                    printf("COPIED NEW TOKEN!\n");
                    types[*no_unique_tokens] = "title"; //Setting tthe type of this token
                    indices[*no_unique_tokens][0] = line_count; //Setting the first index instance of the token
                    indices_end[*no_unique_tokens] += 1;
                    *no_unique_tokens += 1;
                }
                else{
                    printf("OLD TITLE TOKEN FOUND! -> %s\n",san_token_split[i]);
                    //The token here already exists in the unique token list
                    indices[loc][indices_end[loc]] = line_count;
                    indices_end[loc] += 1; //Adding the token index to the existing index for indices
                }
            }
            //Now, we process the authors
            token_split = string_arr_mallocer(100, 100);
            char** inter = string_arr_mallocer(100, 100);
            int no_authors = str_split(col[2], '/', inter);
            counter = 0;
            for (int i = 0; i < no_authors; ++i){
                char** l = string_arr_mallocer(100, 100);
                int x = str_split(inter[i], ' ', l);
                for (int j = counter; j < counter+x; ++j){
                    copy_str(l[j], token_split[j]);
                }
                counter += x;
            }
            san_token_split = string_arr_mallocer(100, 100);
            strong_san_token_split = string_arr_mallocer(100, 100);
            //Sanitizing the strings
            for (int i = 0; i < counter; ++i){
                sanitize(token_split[i], san_token_split[i]);
                strong_sanitize(token_split[i], strong_san_token_split[i]);
            }
            free(token_split);
            //Looping over the sanitized tokens from the line
            for (int i = 0; i < counter; ++i){
                if (isdigit(san_token_split[i][0])){
                    continue;
                }
                else if (san_token_split[i][0] == '\0'){
                    continue;
                }
                else if (in_str_list(san_token_split[i],forbid,7) != -1){
                    continue;
                }
                int loc = in_str_list(san_token_split[i], tokens, *no_unique_tokens);
                if (loc == -1){
                    printf("NEW AUTHOR TOKEN! -> %s\n",san_token_split[i]);
                    //New unique token found
                    soundex_hash(strong_san_token_split[i], soundex[*no_unique_tokens], 5); // Creating the ew soundex hash
                    copy_str(san_token_split[i], tokens[*no_unique_tokens]); //Copying the token that is unique
                    types[*no_unique_tokens] = "author"; //Setting tthe type of this token
                    indices[*no_unique_tokens][0] = line_count; //Setting the first index instance of the token
                    indices_end[*no_unique_tokens] += 1;
                    *no_unique_tokens += 1;
                }
                else{
                    printf("OLD AUTHOR TOKEN FOUND! -> %s\n",san_token_split[i]);
                    //The token here already exists in the unique token list
                    indices[loc][indices_end[loc]] = line_count;
                    indices_end[loc] += 1; //Adding the token index to the existing index for indices
                }
            }
            
            //Now, the publishers
            token_split = string_arr_mallocer(100, 100);
            int token_count = str_split(col[5], ' ', token_split);
            san_token_split = string_arr_mallocer(token_count, 100);
            strong_san_token_split = string_arr_mallocer(token_count, 100);
            //Sanitizing the strings
            for (int i = 0; i < token_count; ++i){
                sanitize(token_split[i], san_token_split[i]);
                strong_sanitize(token_split[i], strong_san_token_split[i]);
            }
            free(token_split);
            //Looping over the sanitized tokens from the line
            for (int i = 0; i < token_count; ++i){
                if (isdigit(san_token_split[i][0])){
                    continue;
                }
                else if (san_token_split[i][0] == '\0'){
                    continue;
                }
                else if (in_str_list(san_token_split[i],forbid,7) != -1){
                    continue;
                }
                int loc = in_str_list(san_token_split[i], tokens, *no_unique_tokens);
                if (loc == -1){
                    printf("NEW PUB TOKEN! -> %s\n",san_token_split[i]);
                    //New unique token found
                    soundex_hash(strong_san_token_split[i], soundex[*no_unique_tokens], 5); // Creating the ew soundex hash
                    copy_str(san_token_split[i], tokens[*no_unique_tokens]); //Copying the token that is unique
                    types[*no_unique_tokens] = "pub"; //Setting tthe type of this token
                    indices[*no_unique_tokens][0] = line_count; //Setting the first index instance of the token
                    indices_end[*no_unique_tokens] += 1;
                    *no_unique_tokens += 1;
                }
                else{
                    printf("OLD PUB TOKEN FOUND! -> %s\n",san_token_split[i]);
                    //The token here already exists in the unique token list
                    indices[loc][indices_end[loc]] = line_count;
                    indices_end[loc] += 1; //Adding the token index to the existing index for indices
                }
            }
        }
        ++line_count;
    }
    
    for (int i = 0; i < *no_unique_tokens; ++i){
        printf("%s (token) - %s (type) - %s (hash)\n",tokens[i],types[i],soundex[i]);
    }

    fclose(file);

    printf("DONE\n");
    
    FILE* output = fopen("dict_soundex.csv", "w");
    if (output == NULL) {
        printf("Could not open output file\n");
        return 1;
    }
    
    for (int i = 0; i < *no_unique_tokens; ++i) {
        char* ind = stitch(indices[i], indices_end[i]);
        fprintf(output, "%s,%s,%s,%s\n", tokens[i], types[i], ind, soundex[i]);
    }

    fclose(output);
    
    return 0;
}