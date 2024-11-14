#include "preprocess.h"
#include "utils.h"
#include "search-utils.h"
#include "soundex.h"

//Will finish this after class tests are dealt with - ciao
void preprocess(char *database, char *eng_thesaurus){
    FILE* dat = fopen(database,"r");
    FILE* the = fopen(eng_thesaurus,"r");
    
    //First, we read the database
    CSV data = readCSV(dat);
    fclose(dat);
    
    //Now, we generate the dictionary
    char excluded_words[7][5] = {"a","an","the","in","is","and","or"};
    char** unique_tokens = string_arr_mallocer(data.nrows*10, 50); //Each token has a maximum of 50 characters, and we assume each row contributes max 10 unique tokens (over-estimation)
    for (int i = 0; i < data.nrows*10; ++i){
	    unique_token[i] = "\0";
    }
    //Corresponding list of indices where the unique token appears
    int** indices = (int**) malloc(data.nrows*10*sizeof(int*));
    for (int i = 0; i < data.nrows*10; ++i){
        indices[i] = (int*) malloc(1000*sizeof(int)); //Assuming that each token is mentioned in 1000 different ids
    }
    //Corresponding types of token
    char** type = string_arr_mallocer(data.nrows*10, 10);
    //Corresponding soundex hashes
    char** hash = string_arr_mallocer(data.nrows*10, MAX_SOUNDEX_HASH);
    
    for (int i = 0; i < data.nrows; ++i){ //Loop over rows
        for (int j = 0; j < data.ncols; ++j){ //Loop over cols
            
        }
    }

    for (int i = 0; i < data.nrows; ++i){
	    for (int j = 0; j < data.ncols; ++j){
		    char * san_term = (char*) malloc(:)
	    }
    }
}
