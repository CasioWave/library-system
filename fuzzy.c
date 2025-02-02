#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "damerau-levenshtein.h"
#include "search-utils.h"
#include "soundex.h"
#include "fuzzy.h"
#include "synonyms.h"
//Advanced search function that takes the show inputs and returns the list of Book IDs that match, ranked by scores
//You must atleast pass a '\0' (NULL terminator) in each of these input string queries
int* advanced_search(char* title, char* author, char* pub, char* dict_file){
    int* res_title = (int*) malloc(1000*sizeof(int));
    int* res_author = (int*) malloc(1000*sizeof(int));
    int* res_pub = (int*) malloc(1000*sizeof(int));
    
    //Search for the title
    if (title[0] != '\0'){
        res_title = fuzzy_search(title, 1, dict_file);
    }
    else{
        res_title[0] = -1;
    }
    
    //Search for the author
    if (author[0] != '\0'){
        res_author = fuzzy_search(author, 2, dict_file);
    }
    else{
        res_title[0] = -1; //Terminates if nothing is being looked for
    }
    
    //Search for the publisher
    if (pub[0] != '\0'){
        res_pub = fuzzy_search(pub, 3, dict_file);
    }
    else{
        res_pub[0] = -1;
    }
    
    int i = 0, j = 0, k = 0;
    // Now, we have to find all the indices that are present in all of the result lists AND assign them scores weighed by where they were in their parent lists
    int* final_res = (int*) malloc(1000*sizeof(int));
    int res_size = 0;
    int size_title = 0, size_author = 0, size_pub = 0;
    for (int i = 0; res_title[i] != -1; ++i){
        size_title += 1;
    }
    for (int i = 0; res_author[i] != -1; ++i){
        size_author += 1;
    }
    for (int i = 0; res_pub[i] != -1; ++i){
        size_pub += 1;
    }
    
    int in_title = 0, in_author = 0, in_pub = 0;
    int* scores = (int*) malloc(1000*sizeof(int));
    //Ideal case where all the terms are supplied and results are non-zero
    if (size_title != 0 && size_author != 0 && size_pub != 0){
        for (int i = 0; i < size_title; ++i){
            int t = res_title[i];
            for (int j = 0; j < size_author; ++j){
                int a = res_author[j];
                if (t != a){
                    continue;
                }
                for (int k = 0; k < size_pub; ++k){
                    int p = res_pub[k];
                    if (t == p){
                        //A Book recorrd has been found that matches all the queries simultaneously
                        final_res[res_size] = t;
                        scores[res_size] = 1000 - i - j - k; //Score linearly decreases
                        ++res_size;
                        break;
                    }
                    else{
                        continue;
                    }
                }
            }
        }
    }
    //Case where one of the result sizes is zero
    // Case where size_title == 0
    else if (size_title == 0 && size_author != 0 && size_pub != 0){
        for (int i = 0; i < size_author; ++i){
            int a = res_author[i];
            for (int j = 0; j < size_pub; ++j){
                int p = res_pub[j];
                if (a == p){
                    final_res[res_size] = a;
                    scores[res_size] = 1000 - i - j;
                    ++res_size;
                    break;
                }
                else{
                    continue;
                }
            }
        }
    }
    //Case where size_author == 0
    else if (size_author == 0 && size_title != 0 && size_pub != 0){
        for (int i = 0; i < size_title; ++i){
            int t = res_title[i];
            for (int j = 0; j < size_pub; ++j){
                int p = res_pub[j];
                if (t == p){
                    final_res[res_size] = t;
                    scores[res_size] = 1000 - i - j;
                    ++res_size;
                    break;
                }
                else{
                    continue;
                }
            }
        }
    }
    //Case where size_pub == 0
    else if (size_pub == 0 && size_title != 0 && size_author != 0){
        for (int i = 0; i < size_title; ++i){
            int t = res_title[i];
            for (int j = 0; j < size_author; ++j){
                int a = res_author[j];
                if (t == a){
                    final_res[res_size] = t;
                    scores[res_size] = 1000 - i - j;
                    ++res_size;
                    break;
                }
                else{
                    continue;
                }
            }
        }
    }
    //Case where all except one search term is zero
    else if (size_title != 0 && size_author == 0 && size_pub == 0){
        return res_title;
    }
    else if (size_author != 0 && size_title == 0 && size_pub == 0){
        return res_author;
    }
    else if (size_pub != 0 && size_title == 0 && size_author == 0){
        return res_pub;
    }
    else{
        final_res[0] = -1;
        return final_res;
    }
    
    //Now that we have gone over the trivial cases, we sort the array to get the final result
    float** unsorted = (float**) malloc(1000*sizeof(float*));
    for (int i = 0; i < 1000; ++i){
        unsorted[i] = (float*) malloc(2*sizeof(float));
    }
    for (int i = 0; i < res_size; ++i){
        unsorted[i][0] = (float) final_res[i];
        unsorted[i][1] = (float) scores[i];
    }
    bubbleSortDescending(unsorted,res_size,2);
    int* resu = (int*) malloc((res_size+1)*sizeof(int));
    for (int i = 0; i < res_size; ++i){
        resu[i] = (int) unsorted[i][0];
    }
    resu[res_size] = -1;
    //free(res_title);
    //free(res_author);
    //free(res_pub);
    //free(unsorted);
    return resu;
}

//Returns an array of indices (matches with index in books-clean.csv) sorted by score, list terminated by -1
//the significance of cat is described in fuzzy.h
int* fuzzy_search(char* query, int cat, char* dict_file){
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
        //printf("Term -> %s\n",san_terms[i]);
    }
    free(terms);
    char ** sound_hashes = string_arr_mallocer(MAXQUERY, SOUND_LEN+1);
    for (int i = 0; i < no_terms; ++i){
        soundex_hash(strong_san_terms[i], sound_hashes[i], SOUND_LEN);
        //printf("%s\n",sound_hashes[i]);
    }
    sound_hashes[no_terms] = "-1";
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
        //printf("SYNONYM -> %s\n",syns[no_syns]);
        ++no_syns;
    }
    //Add the actual terms
    for (int i = 0; i < no_terms; ++i){
        final_terms[i] = strdup(san_terms[i]);
    }
    for (int i = no_terms; i < no_terms+no_syns; ++i){
        final_terms[i] = strdup(syns[i-no_terms]);
    }
    //printf("The set of final terms are ->\n");
    total_terms = no_syns+no_terms;
    
    //free(syns);
    //free(san_terms);
    //printf("Fine till here!\n");
    //Main loop over dict
    int no_res = 0;
    while (feof(dict) == 0){
        fgets(row,10000,dict);
        row[len(row)-1] = '\0'; //Removing the \n from the end
        //printf("Row is -> %s\n",row);
        str_split(row, ',', col);
        if (cat == 0){
            ;
        }
        else if (cat == 1){
            if (str_equal(col[1], "title")){
                ;
            }
            else{
                continue;
            }
        }
        else if (cat == 2){
            if (str_equal(col[1], "author")){
                ;
            }
            else{
                continue;
            }
        }
        else if (cat == 3){
            if (str_equal(col[1], "pub")){
                ;
            }
            else{
                continue;
            }
        }
        //printf("ROW data -> %s\n",col[0]);
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
        //printf("TOTAL TERMS -> %d\n",total_terms);
        for (int i = 0; i < total_terms; ++i){
            lev = damLevMatrix(final_terms[i], col[0]);
            //printf("%d\n",i);
            if (lev > MAXLEV){
                continue;
            }
            else{
                //printf("DIRECT DM MATCH -> %s\n",col[0]);
                int penalty = 0;
                if (i >= no_terms){
                    penalty = SYN_PENALTY;
                }
                for (int j = 0; j < no_index; ++j){
                    int* loc = in_where(res, atoi(indexes[j]), 1000);
                    if (loc[0] == -1){
                        res[no_res] = atoi(indexes[j]);
                        if (lev == 0){
                            scores[no_res] = LEVWEIGHT*(1.0/((float)(lev+1))) - penalty + EXACT_BONUS;
                        }
                        else{
                            scores[no_res] = LEVWEIGHT*(1.0/((float)(lev+1))) - penalty;
                        }
                        //printf("(DM) NEW INDEX %d assigned score %f\n",res[no_res],scores[no_res]);
                        ++no_res;
                    }
                    else{
                        for (int r = 0; loc[r] != -1; ++r){
                            if (lev == 0){
                                scores[loc[r]] += LEVWEIGHT*(1.0/((float)(lev+1))) + EXACT_BONUS;
                            }
                            else{
                                scores[loc[r]] += LEVWEIGHT*(1.0/((float)(lev+1)));
                            }
                        }
                    }
                    //free(loc);
                }
            }
        }
        //Soundex Test and Score allocation
        for (int i = 0; i < total_terms; ++i){
            //printf("HERE %d - %s\n",i,sound_hashes[i]);
            if (strcmp(sound_hashes[i], "-1") == 0){
                break;
            }
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
                    //free(loc);
                }
            }
            else{
                ;
            }
        }
    }
    //free(row);
    //free(col);
    //free(sound_hashes);
    //*ret = NULL;
    //*ret = (int*) malloc(no_res*sizeof(int));
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
    bubbleSortDescending(unsorted,no_res,2);
    //printf("SORTED!\n");
    int* resu = (int*) malloc((no_res+1)*sizeof(int));
    for (int i = 0; i < no_res; ++i){
        resu[i] = (int) unsorted[i][0];
        //printf("%d with weight %f\n",resu[i],unsorted[i][1]);
    }
    /* ret[no_res] = -1; */
    free(unsorted);
    //printf("%d no of results\n",no_res);
    fclose(dict);
    resu[no_res] = -1; //Terminate
    return resu;
}
