#include <stdio.h>
#include <stdlib.h>
#include "damerau-levenshtein.h"
#include "search-utils.h"
#include "soundex.h"
#include "fuzzy.h"
#define LEVWEIGHT 10
#define SOUNDWEIGHT 5
#define MAXLEV 0
#define SOUND_LEN 5

// Compile with soundex.c damerau-levenshtein.c search-utils.c and the main function uncommented to test the search
/*
int main(){
    FILE* dict;
    dict = fopen("dict_soundex.csv","r");
    int* results = (int*) malloc(1000*sizeof(int));
    results = fuzzy_search("academy award",dict);
    fclose(dict);
    for (int i = 0; results[i] > 0; ++i){
        //printf("%d -> %d\n",i,results[i]);
    }
    return 0;
}
*/

//Shelving synonyms for a while
/*
//Takes the sanitized list of search query terms and returns the possible synonyms - the list of returned syns is terminated by string "-1"
char** synonyms(FILE* thesaurus, char** queries, int len_query){
    char* row = (char*) malloc(500*sizeof(char));
    char** col = (char**) malloc(2*sizeof(char*));
    col[0] = (char*) malloc(100*sizeof(char));
    col[1] = (char*) malloc(400*sizeof(char));
    char** terms = (char**) malloc(10*sizeof(char*));
    for (int i = 0; i < 10; ++i){
        terms[i] = (char*) malloc(50*sizeof(char));
    }
    char** syn = (char**) malloc(10*sizeof(char*));
    for (int i = 0; i < 10; ++i){
        syn[i] = (char*) malloc(50*sizeof(char));
    }
    char** ret_syn = (char**) malloc(100*sizeof(char*));
    for (int i = 0; i < 10; ++i){
        ret_syn[i] = (char*) malloc(50*sizeof(char));
    }
    int no_syn = 0;
    while (feof(thesaurus) == 0){
        fgets(row, 500, thesaurus);
        str_split(row, ',', col);
        int n = len(col[1]);
        col[1][n] = '\0'; //Removing the \n at the end of the row
        int no_terms = str_split(col[0],'-',terms);
        int no_syn_terms = str_split(col[1],'-',syn);
        int check = 0;
        for (int i = 0; i < no_terms; ++i){
            if (in_str_list(terms[i],queries,len_query)){
                //printf("TERM %s from thesaurus in query!\n", terms[i]);
                ++check;
            }
        }
        //The query string list contains this word from the thesaurus
        if (check == no_terms){
            for (int x = 0; x < no_syn_terms; ++x){
                ret_syn[no_syn] = syn[x];
                printf("The term %s from thesaurus located in query!\n",col[0]);
                ++no_syn;
            }
        }
        else {
            continue;
        }
    }
    ret_syn[no_syn] = "-1";
    for (int i = 0; str_equal(ret_syn[i],"-1")!=1;++i){
        printf("Synonym -> %s\n",ret_syn[i]);
    }
    return ret_syn;
}
*/
//Returns an array of indices (matches with index in books-clean.csv) sorted by score, list terminated by -1
int* fuzzy_search(char* query,FILE* dict){
    int* res = (int*) malloc(1000 * sizeof(int));
    float* scores = (float*) malloc(1000 * sizeof(float));
    for (int i = 0; i < 1000; ++i){
        res[i] = -1;
        scores[i] = (float)-1;
    }
    char* strip_query = (char*) malloc(200*sizeof(char));
    strip(query,strip_query);
    char** terms = (char**) malloc(20*sizeof(char*));
    for (int i = 0; i < 20; ++i){
        terms[i] = (char*) malloc(100*sizeof(char));
    }
    int no_terms = str_split(strip_query, ' ', terms);
    char** san_terms = (char**) malloc(20*sizeof(char*));
    char** strong_san_terms = (char**) malloc(20*sizeof(char*));
    for (int i = 0; i < 20; ++i){
        san_terms[i] = (char*) malloc(100*sizeof(char));
    }
    for (int i = 0; i < 20; ++i){
        strong_san_terms[i] = (char*) malloc(100*sizeof(char));
    }
    for (int i = 0; i < no_terms; ++i){
        sanitize(terms[i],san_terms[i]);
        strong_sanitize(terms[i],strong_san_terms[i]);
    }
    free(terms);
    char ** sound_hashes = (char**) malloc(20*sizeof(char*));
    for (int i = 0; i < 20; ++i){
        sound_hashes[i] = (char*) malloc((SOUND_LEN+1)*sizeof(char));
    }
    for (int i = 0; i < no_terms; ++i){
        soundex_hash(strong_san_terms[i], sound_hashes[i], SOUND_LEN);
        //printf("%s\n",sound_hashes[i]);
    }
    free(strong_san_terms); //Only required for the soundex hash
    //Now that everything is prepared and sanitized, we go over the dict
    char* row = (char*) malloc(10000*sizeof(char));
    char** col = (char**) malloc(4*sizeof(char*));
    col[0] = (char*) malloc(20*sizeof(char)); //Holds the unique token
    col[1] = (char*) malloc(10*sizeof(char)); //Holds the the type of token
    col[2] = (char*) malloc(20000*sizeof(char)); //Holds the indexes
    col[3] = (char*) malloc((SOUND_LEN+1)*sizeof(char)); //Holds the soundex hash
    char** indexes = (char**) malloc(20000*sizeof(char*));
    for (int i = 0; i < 20000; ++i){
        indexes[i] = (char*) malloc(6*sizeof(char));
    }
    //Shelved synonym stuff
    /*
    char** syn = (char**) malloc(100*sizeof(char*));
    for (int i = 0; i < 100; ++i){
        syn[i] = (char*) malloc(50*sizeof(char));
    }
    FILE* thesaurus = fopen("thesaurus.csv","r");
    syn = synonyms(thesaurus, san_terms, no_terms);
    int no_syn = 0;
    while (str_equal(syn[no_syn], "-1") != 1){
        ++no_syn;
    }
    */
    //printf("There are %d synonyms, like -> %s\n",no_syn,syn[0]);
    //Main loop over dict
    int no_res = 0;
    while (feof(dict) == 0){
        fgets(row,10000,dict);
        str_split(row, ',', col);
        col[3][SOUND_LEN] = '\0'; //Mitigating the extra \n at the end
        int no_index = str_split(col[2],'-',indexes);
        //Damerau-Levenshtein Test and Score allocation
        int lev = 0;
        for (int i = 0; i < no_terms; ++i){
            lev = damLevMatrix(san_terms[i], col[0]);
            if (lev > MAXLEV){
                continue;
            }
            else{
                for (int j = 0; j < no_index; ++j){
                    int* loc = in_where(res, atoi(indexes[j]), 1000);
                    if (loc[0] == -1){
                        res[no_res] = atoi(indexes[j]);
                        scores[no_res] = LEVWEIGHT*(1.0/((float)(lev+1)));
                        //printf("(DM) NEW INDEX %d assigned score %f\n",res[no_res],scores[no_res]);
                        ++no_res;
                    }
                    else{
                        for (int r = 0; loc[r] != -1; ++r){
                            scores[loc[r]] += LEVWEIGHT*(1.0/((float)(lev+1)));
                        }
                    }
                    free(loc);
                }
            }
        }
        //Soundex Test and Score allocation
        for (int i = 0; i < no_terms; ++i){
            if (str_equal(sound_hashes[i],col[3])){
                //printf("SOUNDEX MATCH OF %s with %s\n",san_terms[i],col[0]);
                for (int j = 0; j < no_index; ++j){
                    int* loc = in_where(res,atoi(indexes[j]),1000);
                    int san_len = len(san_terms[i]);
                    int token_len = len(col[0]);
                    int len_diff = 0;
                    if (san_len > token_len){
                        len_diff = san_len - token_len;
                    }
                    else{
                        len_diff = token_len - san_len;
                    }
                    if (loc[0] == -1){
                        res[no_res] = atoi(indexes[j]);
                        scores[no_res] = SOUNDWEIGHT*(1.0/(len_diff+1));
                        //printf("(S) NEW INDEX %d assigned score %f - len(san) = %d len(token) = %d\n",res[no_res],scores[no_res],len(san_terms[i]),len(col[0]));
                        ++no_res;
                    }
                    else{
                        for (int r = 0; loc[r] != -1; ++r){
                            scores[loc[r]] += SOUNDWEIGHT*(1.0/(len_diff+1));
                        }
                    }
                    free(loc);
                }
            }
            else{
                continue;
            }
        }
    }
    free(row);
    free(col);
    free(sound_hashes);
    free(san_terms);
    int* ret = (int*) malloc((no_res+1)*sizeof(int));
    float** unsorted = (float**) malloc(no_res*sizeof(float*));
    int i;
    /*
    for (i = 0; i < no_res; ++i){
        printf("%d -> INDEX: %d, SCORE: %f\n",i,res[i],scores[i]);
    }
    */
    for (i = 0; i < no_res; ++i){
        //printf("ASSIGNED %d\n",i);
        unsorted[i] = (float*) malloc(2*sizeof(float));
        unsorted[i][0] = res[i];
        //printf("TEST VALUE OF unsorted at (%d,0) -> %f\n",i,unsorted[i][0]);
        unsorted[i][1] = scores[i];
        //printf("TEST VALUE OF unsorted at (%d,1) -> %f\n",i,unsorted[i][1]);
    }
    //printf("HERE\n");
    /*
    for (i = 0; i < no_res; ++i){
        printf("%d -> %f - %f\n",i,unsorted[i][0],unsorted[i][1]);
    }
    */
    
    //printf("LENGTH(UNSORTED) = %d\n",no_res);
    bubble2dsort(unsorted,no_res);
    //printf("SORTED!\n");
    
    for (int i = 0; i < no_res; ++i){
        ret[i] = (int) unsorted[i][0];
        //printf("%d\n",ret[i]);
    }
    ret[no_res] = -1;
    free(unsorted);
    printf("%d no of results\n",no_res);
    return ret;
}
