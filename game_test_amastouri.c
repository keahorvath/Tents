#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"


/*test_game_set_square*/
bool test_game_set_square (){
    game test_game = game_default();
    unsigned int i=0;
    unsigned int j=0;
    while ( (i<8) && (j<8)){
        game_set_square(test_game, i,j, EMPTY);
        if (game_get_square(test_game, i,j) != EMPTY){
            return false;
        }
        game_set_square(test_game, i,j, TREE);
        if (game_get_square(test_game, i,j) != TREE){
            return false;
        }
        game_set_square(test_game, i,j, TENT);
        if (game_get_square(test_game, i,j) != TENT){
            return false;
        }
        game_set_square(test_game, i,j, GRASS);
        if (game_get_square(test_game, i,j) != GRASS){
            return false;
        }   
    }
    return true;
}

/*test_game_get_square*/
bool test_game_set_square (){
    game test_game = game_default();
    if ( (game_get_square(test_game, 0,4) != TREE) 
       || (game_get_square(test_game, 0,0) != EMPTY) ) {
           return false;
       }
    game_set_square(test_game,0 ,0, TENT);
    if (game_get_square(test_game, 0,0) != TENT){
        return false;
    }
    game_set_square(test_game,0 ,1, GRASS);
    if (game_get_square(test_game, 0,1) != GRASS){
        return false;
    }
    return true;
}






int main(int argc, char *argv[]){

    if (argc == 1) usage(argc, argv);
    // start test
    fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
    bool testPassed = false;
    if (strcmp("game_set_square", argv[1]) == 0)
        testPassed = test_game_set_square();
    else if (strcmp("game_get_square", argv[1]) == 0)
        testPassed = test_game_get_square();
    else if (strcmp("game_equal", argv[1]) == 0)
        testPassed = test_game_equal();
    else if (strcmp("game_delete", argv[1]) == 0)
        testPassed = test_game_delete();
    else if (strcmp("game_set_expected_nb_tents_row", argv[1]) == 0)
        testPassed = test_game_set_expected_nb_tents_row();
    else if (strcmp("game_set_expected_nb_tents_col", argv[1]) == 0)
        testPassed = test_game_set_expected_nb_tents_col();
    else {
        fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // print test result
    if (testPassed){
        fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
        return EXIT_SUCCESS;
    }
    else{
        fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
        return EXIT_FAILURE;
    }
}