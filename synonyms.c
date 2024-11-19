#include "synonyms.h"
#include "search-utils.h"
#include "damerau-levenshtein.h"
#include "soundex.h"
#include "fuzzy.h"
#include <string.h>

//Takes the sanitized list of search query terms and returns the possible synonyms - the list of returned syns is terminated by string "-1"
int synonyms(char* thesaurus_file, char** queries, int len_query, char** ret){
    FILE* thesaurus = fopen(thesaurus_file,"r");
    //We return a maximum of 100 synonyms, each of len 100
    //char** ret = string_arr_mallocer(100, 100);
    int no_ret = 0;
    while (feof(thesaurus) == 0){
        char* row = (char*) malloc(500*sizeof(char));
        fgets(row,500,thesaurus);
        row[len(row)-1] = '\0';
        if (row[0] == '\0'){
            break;
        }
        //printf("ROW -> %s\n",row);
        
        char** col = (char**) malloc(2*sizeof(char*));
        col[0] = (char*) malloc(100*sizeof(char));
        col[1] = (char*) malloc(500*sizeof(char));
        int no_cols = str_split(row, ',', col);
        /*
        for (int i = 0; i < no_cols; ++i){
            printf("VAL OF COL %d -> %s\n",i,col[i]);
        }
        */
        char** terms = string_arr_mallocer(10, 50);
        int no_terms = str_split(col[0], '-', terms);
        /*
        for (int i = 0; i < no_terms; ++i){
            printf("TERM %d -> %s\n",i,terms[i]);
        }
        */
        char** syns = string_arr_mallocer(10, 50);
        int no_syns = str_split(col[1], '-', syns);
        /*
        for (int i = 0; i < no_syns; ++i){
            printf("SYN %d -> %s\n",i,syns[i]);
        }
        */
        //Initiate a loop over the query terms
        int check = 0;
        for (int i = 0; i < len_query; ++i){
            //Loop over all the terms in this row of thesaurus
            for (int j = 0; j < no_terms; ++j){
                int lev = damLevMatrix(queries[i], terms[j]);
                if (lev > MAXLEV){
                    continue;
                }
                else{
                    //This term from this row of thesaurus is present in query
                    ++check;
                }
            }
        }
        ///This triggers if it is found that all terms of this thesaurus row are present in the query
        if (check >= len_query){
            for (int i = 0; i < no_syns; ++i){
                strcpy(ret[no_ret], syns[i]);
                ++no_ret;
            }
        }
    }
    
    for (int i = 0; i < no_ret; ++i){
        //printf("Synonym %d -> %s\n",i,ret[i]);
    }
    
    ret[no_ret] = "-1";
    //printf("There are %d synonyms found in total\n",no_ret);
    fclose(thesaurus);
    return 0;
}