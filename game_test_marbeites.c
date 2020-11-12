#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"

/*Tests*/

//Test game_get_expected_nb_tents_row
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
    if(game_get_expected_nb_tents_row(g,0)!=3){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,1)!=0){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,2)!=4){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,3)!=0){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,4)!=4){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,5)!=0){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,6)!=1){
        return false;
    }
    if(game_get_expected_nb_tents_row(g,7)!=0){
        return false;
    }
    game_delete(g);
    return true;
};

//Test game_get_expected_nb_tents_col
bool test_get_expected_nb_tents_col(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error: this column doesn't exist\n");
        return false;
    }
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error: invalid game\n");
        return false;
    }
    if(game_get_expected_nb_tents_col(g,0)!=4){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,1)!=0){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,2)!=1){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,3)!=2){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,4)!=1){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,5)!=1){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,6)!=2){
        return false;
    }
    if(game_get_expected_nb_tents_col(g,7)!=1){
        return false;
    }
    game_delete(g);
    return true;
};

//Test game_get_expected_nb_tents_all
bool test_get_expected_nb_tents_all(void){
    game g = game_default();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    if(game_get_expected_nb_tents_all(g)!=12){
        return false;
    }
    game_delete(g);
    return true;
};

//Test game_get_current_nb_tents_row
bool test_get_current_nb_tents_row(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error: this row doesn't exist\n");
        return false;
    }
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error: invalid game\n");
        return false;
    }
    if(game_get_current_nb_tents_row(g,0)!=3){
        return false;
    }
    if(game_get_current_nb_tents_row(g,1)!=0){
        return false;
    }
    if(game_get_current_nb_tents_row(g,2)!=4){
        return false;
    }
    if(game_get_current_nb_tents_row(g,3)!=0){
        return false;
    }
    if(game_get_current_nb_tents_row(g,4)!=4){
        return false;
    }
    if(game_get_current_nb_tents_row(g,5)!=0){
        return false;
    }
    if(game_get_current_nb_tents_row(g,6)!=1){
        return false;
    }
    if(game_get_current_nb_tents_row(g,7)!=0){
        return false;
    }
    game_delete(g);
    return true;
};

//Test game_get_current_nb_tents_col
bool test_get_current_nb_tents_col(int k){
    if((k<0)|(k>7)){
        fprintf(stderr, "Error: this column doesn't exist\n");
        return false;
    }
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error: invalid game\n");
        return false;
    }
    if(game_get_current_nb_tents_col(g,0)!=4){
        return false;
    }
    if(game_get_current_nb_tents_col(g,1)!=0){
        return false;
    }
    if(game_get_current_nb_tents_col(g,2)!=1){
        return false;
    }
    if(game_get_current_nb_tents_col(g,3)!=2){
        return false;
    }
    if(game_get_current_nb_tents_col(g,4)!=1){
        return false;
    }
    if(game_get_current_nb_tents_col(g,5)!=1){
        return false;
    }
    if(game_get_current_nb_tents_col(g,6)!=2){
        return false;
    }
    if(game_get_current_nb_tents_col(g,7)!=1){
        return false;
    }
    game_delete(g);
    return true;
};

//Test game_get_current_nb_tents_all
bool test_get_current_nb_tents_all(void){
    game g = game_default_solution();
    if(g==NULL){
        fprintf(stderr, "Error : invalid game\n");
        return false;
    }
    if(game_get_current_nb_tents_all(g)!=12){
        fprintf(stderr, "Error: unexpected amount of tents\n");
        return false;
    }
    game_delete(g);
    return true;
};

//Main routine
int main(int argc, char *argv[]){
    //start tests
    fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
    bool ok = false;
    if(strcmp("get_expected_nb_tents_row", argv[1])==0){
        ok = test_get_expected_nb_tents_row(0);
    }
    else if(strcmp("get_expected_nb_tents_col", argv[1])==0){
        ok = test_get_expected_nb_tents_col(0);
    }
    else if(strcmp("get_expected_nb_tents_all", argv[1])==0){
        ok = test_get_expected_nb_tents_all();
    }
    else if(strcmp("get_current_nb_tents_row", argv[1])==0){
        ok = test_get_current_nb_tents_row(0);
    }
    else if(strcmp("get_current_nb_tents_col", argv[1])==0){
        ok = test_get_current_nb_tents_col(0);
    }
    else if(strcmp("get_current_nb_tents_all", argv[1])==0){
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