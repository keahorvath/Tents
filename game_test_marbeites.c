#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"

/*Tests*/

bool test_get_expected_nb_tents_row(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error : this row doesn't exist\n");
        return false;
    }
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a = game_get_square(g,k,i);
        if(a==2){
            tents++;
        }
    }
    if(tents!=game_get_expected_nb_tents_row(g,k)){
        fprintf(stderr, "Error : unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_expected_nb_tents_col(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error : this column doesn't exist\n");
        return false;
    }
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a = game_get_square(g,i,k);
        if(a==2){
            tents++;
        }
    }
    if(tents!=game_get_expected_nb_tents_col(g,k)){
        fprintf(stderr, "Error : unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_expected_nb_tents_all(void){
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            square a=game_get_square(g,i,j);
            if(a==2){
                tents++;
            }
        }
    }
    if(tents!=game_get_expected_nb_tents_all(g)){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_current_nb_tents_row(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error : this row doesn't exist\n");
        return false;
    }
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a = game_get_square(g,k,i);
        if(a==2){
            tents++;
        }
    }
    if(tents!=game_get_current_nb_tents_row(g,k)){
        fprintf(stderr, "Error : unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_current_nb_tents_col(int k){
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a=game_get_square(g,i,k);
            if(a==2){
                tents++;
            }
        }
    if(tents!=game_get_current_nb_tents_col(g,k)){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_current_nb_tents_all(void){
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            square a=game_get_square(g,i,j);
            if(a==2){
                tents++;
            }
        }
    }
    if(tents!=game_get_current_nb_tents_all(g)){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

//Main routine
int main(int argc, char *argv[]){
    bool ok = false;
    if(strcmp("get_expected_nb_tents_row", argv[1])==0){
        ok = test_get_expected_nb_tents_row(0);
    }
    if(strcmp("get_expected_nb_tents_col", argv[1])==0){
        ok = test_get_expected_nb_tents_col(0);
    }
    if(strcmp("get_expected_nb_tents_all", argv[1])==0){
        ok = test_get_expected_nb_tents_all();
    }
    if(strcmp("get_current_nb_tents_row", argv[1])==0){
        ok = test_get_current_nb_tents_row(0);
    }
    if(strcmp("get_current_nb_tents_col", argv[1])==0){
        ok = test_get_current_nb_tents_col(0);
    }
    if(strcmp("get_current_nb_tents_all", argv[1])==0){
        ok = test_get_current_nb_tents_all();
    }
    #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"

/*Tests*/

bool test_get_expected_nb_tents_row(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error : this row doesn't exist\n");
        return false;
    }
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a = game_get_square(g,k,i);
        if(a==2){
            tents++;
        }
    }
    if(tents!=game_get_expected_nb_tents_row(g,k)){
        fprintf(stderr, "Error : unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_expected_nb_tents_col(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error : this column doesn't exist\n");
        return false;
    }
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a = game_get_square(g,i,k);
        if(a==2){
            tents++;
        }
    }
    if(tents!=game_get_expected_nb_tents_col(g,k)){
        fprintf(stderr, "Error : unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_expected_nb_tents_all(void){
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            square a=game_get_square(g,i,j);
            if(a==2){
                tents++;
            }
        }
    }
    if(tents!=game_get_expected_nb_tents_all(g)){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_current_nb_tents_row(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error : this row doesn't exist\n");
        return false;
    }
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a = game_get_square(g,k,i);
        if(a==2){
            tents++;
        }
    }
    if(tents!=game_get_current_nb_tents_row(g,k)){
        fprintf(stderr, "Error : unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_current_nb_tents_col(int k){
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        square a=game_get_square(g,i,k);
            if(a==2){
                tents++;
            }
        }
    if(tents!=game_get_current_nb_tents_col(g,k)){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

bool test_get_current_nb_tents_all(void){
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    int tents=0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            square a=game_get_square(g,i,j);
            if(a==2){
                tents++;
            }
        }
    }
    if(tents!=game_get_current_nb_tents_all(g)){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

//USAGE

void usage(int argc, char *argv[]){
    fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
    exit(EXIT_FAILURE);
}

//Main routine
int main(int argc, char *argv[]){
    if(argc==1) usage(argc, argv);
    //start test
    fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
    bool ok = false;
    if(strcmp("get_expected_nb_tents_row", argv[1])==0){
        ok = test_get_expected_nb_tents_row(0);
    }
    if(strcmp("get_expected_nb_tents_col", argv[1])==0){
        ok = test_get_expected_nb_tents_col(0);
    }
    if(strcmp("get_expected_nb_tents_all", argv[1])==0){
        ok = test_get_expected_nb_tents_all();
    }
    if(strcmp("get_current_nb_tents_row", argv[1])==0){
        ok = test_get_current_nb_tents_row(0);
    }
    if(strcmp("get_current_nb_tents_col", argv[1])==0){
        ok = test_get_current_nb_tents_col(0);
    }
    if(strcmp("get_current_nb_tents_all", argv[1])==0){
        ok = test_get_current_nb_tents_all();
    }
    else{
        fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    //print test result
    if(ok){
        fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
        return EXIT_SUCCESS;
    } else{
        fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
        return EXIT_FAILURE;
    }
}
    //print test result
    if(ok){
        fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
        return EXIT_SUCCESS;
    } else{
        fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
        return EXIT_FAILURE;
    }
}