#include <stdio.h>
#include <stdlib.h>
#include "damerau-levenshtein.h"
#include "search-utils.h"
#include "soundex.h"
#include "fuzzy.h"
#include "synonyms.h"

//Returns an array of indices (matches with index in books-clean.csv) sorted by score, list terminated by -1
int* fuzzy_search(char* query, char* dict_file){
    FILE* dict = fopen(dict_file,"r");
    int* res = (int*) malloc(MAXRES * sizeof(int));
    float* scores = (float*) malloc(MAXRES * sizeof(float));
    
    for (int i = 0; i < MAXRES; ++i){
        res[i] = -1;
        scores[i] = (float) -1;
    }
    char* strip_query = (char*) malloc(200*sizeof(char));
    strip(query,strip_query);
    char** terms = (char**) malloc(MAXQUERY*sizeof(char*));
    for (int i = 0; i < 20; ++i){
        terms[i] = (char*) malloc(100*sizeof(char));
    }
    int no_terms = str_split(strip_query, ' ', terms);
    char** san_terms = string_arr_mallocer(MAXQUERY, 100);
    char** strong_san_terms = string_arr_mallocer(MAXQUERY, 100);
    for (int i = 0; i < no_terms && i < MAXQUERY; ++i){
        sanitize(terms[i],san_terms[i]);
        strong_sanitize(terms[i],strong_san_terms[i]);
    }
    free(terms);
    char ** sound_hashes = string_arr_mallocer(MAXQUERY, SOUND_LEN+1);
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
    
    //Before we start looping over dict, we create list of synonyms
    char ** syns = string_arr_mallocer(100, 100);
    synonyms("thesaurus.csv", san_terms, no_terms, syns);
    
    //Now we create the final array of terms to lookup
    int total_terms = 0;
    char** final_terms = string_arr_mallocer(MAXQUERY + 100, 100);
    int no_syns = 0;
    //Count the number of synonyms
    while (str_equal(syns[no_syns], "-1") != 1){
        ++no_syns;
    }
    //Add the actual terms
    for (int i = 0; i < no_terms; ++i){
        final_terms[i] = strdup(san_terms[i]);
    }
    for (int i = no_terms; i < no_terms+no_syns; ++i){
        final_terms[i] = strdup(syns[i-no_terms]);
    }
    total_terms = no_syns+no_terms;
    //free(syns);
    free(san_terms);
    
    //Main loop over dict
    int no_res = 0;
    while (feof(dict) == 0){
        fgets(row,10000,dict);
        row[len(row)-1] = '\0'; //Removing the \n from the end
        //printf("Row is -> %s\n",row);
        str_split(row, ',', col);
        //printf("TOKEN -> %s\n",col[0]);
        //printf("TYPE -> %s\n",col[1]);
        //printf("INDICES -> %s\n",col[2]);
        //printf("SOUNDEX -> %s\n",col[3]);
        int no_index = str_split(col[2],'-',indexes);
        //printf("The split indices are ->\n");
        //for (int i = 0; i < no_index; ++i){
        //    printf("%s\n",indexes[i]);
        //}
        //Damerau-Levenshtein Test and Score allocation
        int lev = 0;
        for (int i = 0; i < total_terms; ++i){
            lev = damLevMatrix(final_terms[i], col[0]);
            if (lev > MAXLEV){
                continue;
            }
            else{
                int penalty = 0;
                if (i >= no_terms){
                    penalty = SYN_PENALTY;
                }
                for (int j = 0; j < no_index; ++j){
                    int* loc = in_where(res, atoi(indexes[j]), 1000);
                    if (loc[0] == -1){
                        res[no_res] = atoi(indexes[j]);
                        scores[no_res] = LEVWEIGHT*(1.0/((float)(lev+1))) - penalty;
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
        for (int i = 0; i < total_terms; ++i){
            if (str_equal(sound_hashes[i],col[3])){
                //printf("SOUNDEX MATCH OF %s with %s\n",san_terms[i],col[0]);
                int penalty = 0;
                if (i >= no_terms){
                    penalty = SYN_PENALTY;
                }
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
                        scores[no_res] = SOUNDWEIGHT*(1.0/(len_diff+1))-penalty;
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
    int* ret = (int*) malloc((no_res+1)*sizeof(int));
    float** unsorted = (float**) malloc(no_res*sizeof(float*));
    int i;
    for (i = 0; i < no_res; ++i){
        //printf("ASSIGNED %d\n",i);
        unsorted[i] = (float*) malloc(2*sizeof(float));
        unsorted[i][0] = res[i];
        //printf("TEST VALUE OF unsorted at (%d,0) -> %f\n",i,unsorted[i][0]);
        unsorted[i][1] = scores[i];
        //printf("TEST VALUE OF unsorted at (%d,1) -> %f\n",i,unsorted[i][1]);
    }
    
    //printf("LENGTH(UNSORTED) = %d\n",no_res);
    bubble2dsort(unsorted,no_res);
    //printf("SORTED!\n");
    
    for (int i = 0; i < no_res; ++i){
        ret[i] = (int) unsorted[i][0];
        //printf("%d\n",ret[i]);
    }
    ret[no_res] = -1;
    free(unsorted);
    //printf("%d no of results\n",no_res);
    fclose(dict);
    return ret;
}
