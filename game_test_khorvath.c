#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"


bool test_game_play_move(void){
    game g0 = game_default();
    game g1 = game_default();
    game g2 = game_default();
    game g3 = game_default();
    for(unsigned int i = 0; i < DEFAULT_SIZE; i++){
        for(unsigned int j = 0; j < DEFAULT_SIZE; j++){
            if (game_get_square(g0, i, j) != TREE){
                game_play_move(g1, i, j, EMPTY);
                game_play_move(g2, i, j, TENT);
                game_play_move(g3, i, j, GRASS);
            }
            if (game_get_square(g0, i, j) == TREE){
                if (game_get_square(g1, i, j) != TREE || game_get_square(g2, i, j) != TREE || game_get_square(g3, i, j) != TREE){
                    return false;
                }
            }else if(game_get_square(g1, i, j) != EMPTY || game_get_square(g2, i, j) != TENT || game_get_square(g3, i, j) != GRASS){
                return false;
            }
            if (!(i == DEFAULT_SIZE-1 || j == DEFAULT_SIZE-1)){
                for (unsigned int k = i+1; k < DEFAULT_SIZE; k++){
                    for (unsigned int l = j+1; l < DEFAULT_SIZE; l++){
                        if (game_get_square(g0, k, l) != game_get_square(g1, k, l) ||
                        game_get_square(g0, k, l) != game_get_square(g2, k, l) ||
                        game_get_square(g0, k, l) != game_get_square(g3, k, l)){
                            return false;
                        }
                    }
                }
            }
        }
    }
    game_delete(g0);
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    return true;
}

bool test_game_check_move(void){
    game g = game_default();
    //test if replacing tree is illegal
    if (game_check_move(g, 0, 4, TENT) != ILLEGAL || game_check_move(g, 0, 4, GRASS) != ILLEGAL || game_check_move(g, 0, 4, EMPTY) != ILLEGAL){
        return false;
    }
    /*
    //test if placing new tree is illegal
    if (game_check_move(g, 1, 1, TREE) != ILLEGAL){
        return false;
    }
    //test if placing n+1 tents in row is losing
    game_play_move(g, 6, 4, TENT);
    if (game_check_move(g, 6, 0, TENT) != LOSING){
        return false;
    }
    //test if placing n+1 tents in column is losing
    game_play_move(g, 0, 3, TENT);
    game_play_move(g, 2, 3, TENT);
    if (game_check_move(g, 5, 3, TENT) != LOSING){
        return false;
    }
    //test if placing tent adjacent to tent is losing
    if (game_check_move(g, 1, 2, TENT) != LOSING || game_check_move(g, 1, 3, TENT) != LOSING ||
    game_check_move(g, 1, 4, TENT) != LOSING || game_check_move(g, 3, 3, TENT) != LOSING ||
    game_check_move(g, 3, 2, TENT) != LOSING || game_check_move(g, 2, 2, TENT) != LOSING ||
    game_check_move(g, 3, 4, TENT) != LOSING){
        return false;
    }
    //test if placing tent in row and column with no tents required is losing
    if (game_check_move(g, 7, 1, TENT) != LOSING || game_check_move(g, 3, 7, TENT) != LOSING){
        return false;
    }
    //test if placing grass and not enough empty squares for tents is losing
    if (game_check_move(g, 0, 0, GRASS) != LOSING){
        return false;
    }
    //test if placing a regular move is regular
    game g1 = game_default();
    game g2 = game_default_solution();
    for (unsigned int i = 0; i < DEFAULT_SIZE; i++){
        for (unsigned int j = 0; j < DEFAULT_SIZE; j++){
            if (game_get_square(g2, i, j) == TENT && game_check_move(g1, i, j, TENT) != REGULAR){
                return false;
            }
            if (game_get_square(g2, i, j) == GRASS && game_check_move(g1, i, j, GRASS) != REGULAR && game_check_move(g1, i, j, EMPTY) != REGULAR){
                return false;
            }
        }
    }
    //test if surronding tree by grass is losing
    game g3 = game_default();
    game_play_move(g3, 5, 3, GRASS);
    game_play_move(g3, 6, 4, GRASS);
    game_play_move(g3, 5, 5, GRASS);
    if (game_check_move(g3, 4, 4, GRASS) != LOSING){
        return false;
    }
    //test if placing tent with no tree around is losing
    if (game_check_move(g3, 6, 5, TENT) != LOSING){
        return false;
    }
    //test if having more tents than trees is losing
    game_play_move(g3, 0, 0, TENT);
    game_play_move(g3, 2, 0, TENT);
    game_play_move(g3, 4, 0, TENT);
    game_play_move(g3, 6, 0, TENT);
    game_play_move(g3, 0, 1, TENT);
    game_play_move(g3, 1, 1, TENT);
    game_play_move(g3, 2, 1, TENT);
    game_play_move(g3, 3, 1, TENT);
    game_play_move(g3, 5, 1, TENT);
    game_play_move(g3, 6, 1, TENT);
    game_play_move(g3, 7, 1, TENT);
    game_play_move(g3, 0, 3, TENT);
    if (game_check_move(g3, 0, 6, TENT) != LOSING){
        return false;
    }
    game_delete(g);
    game_delete(g1);
    game_delete(g2);
    game_delete(g3);
    */
    return true;
}

bool test_game_is_over(void){
    game g = game_default_solution();
    //test if the correct solution returns an error
    if (!game_is_over(g)){
        return false;
    }
    //test if all tents connected to tree and vice versa
	square squares[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint nb_tents_row[] = {0, 1, 1, 0, 0, 0, 0, 0};
	uint nb_tents_col[] = {1, 0, 0, 0, 0, 1, 0, 0};
    game g2 = game_new(squares, nb_tents_row, nb_tents_col);
    game_play_move(g2, 2, 0, TENT);
    game_play_move(g2, 1, 5, TENT);
    if(game_is_over(g2)){
        return false;
    }
    //test if nbtrees != nbtents
	square squares2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint nb_tents_row2[] = {0, 0, 0, 1, 0, 0, 0, 0};
	uint nb_tents_col2[] = {0, 0, 0, 1, 0, 0, 0, 0};
    game g3 = game_new(squares2, nb_tents_row2, nb_tents_col2);
    game_play_move(g3, 3, 3, TENT);
    if(game_is_over(g3)){
        return false;
    }
    //test if tents are adjacent
	square squares3[] = {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint nb_tents_row3[] = {2, 0, 1, 0, 0, 0, 0, 0};
	uint nb_tents_col3[] = {0, 0, 0, 1, 1, 0, 1, 0};
    game g4 = game_new(squares3, nb_tents_row3, nb_tents_col3);
    game_play_move(g4, 0, 3, TENT);
    game_play_move(g4, 0, 4, TENT);
    game_play_move(g4, 2, 6, TENT);
    if(game_is_over(g4)){
        return false;
    }
    //test if not having the right nb of tents in row/column still returns true
    game_play_move(g, 0, 0, GRASS);
    if (game_is_over(g)){
        return false;
    }
    game_play_move(g, 0, 0, TENT);
    //test if having nbTents!=nbTrees returns true
    game_set_square(g, 7, 0, GRASS);
    if (game_is_over(g)){
        return false;
    }
    game_set_square(g, 7, 0, TREE);
    //test if placing tent not next to tree still returns true
    game_play_move(g, 6, 0, GRASS);
    game_play_move(g, 6, 5, TENT);
    if(game_is_over(g)){
        return false;
    }
    return true;
}

bool test_game_fill_grass_row(void){
    game g = game_default();
    game_play_move(g, 0, 2, GRASS);
    game_play_move(g, 0, 6, TENT);
    game g2 = game_default();
    game_play_move(g2, 0, 2, GRASS);
    game_play_move(g2, 0, 6, TENT);
    game_play_move(g2, 0, 0, GRASS);
    game_play_move(g2, 0, 1, GRASS);
    game_play_move(g2, 0, 3, GRASS);
    game_play_move(g2, 0, 7, GRASS);
    game_fill_grass_row(g, 0);
    if (!game_equal(g, g2)){
        return false;
    }
    game_delete(g);
    game_delete(g2);
    return true;
}

bool test_game_fill_grass_col(void){
    game g = game_default();
    game_play_move(g, 2, 0, GRASS);
    game_play_move(g, 4, 0, TENT);
    game g2 = game_default();
    game_play_move(g2, 2, 0, GRASS);
    game_play_move(g2, 4, 0, TENT);
    game_play_move(g2, 0, 0, GRASS);
    game_play_move(g2, 6, 0, GRASS);
    game_fill_grass_col(g, 0);

    if (!game_equal(g, g2)){
        return false;
    }
    game_delete(g);
    game_delete(g2);
    return true;
}

bool test_game_restart(void){
    game g1 = game_default();
    game g2 = game_default_solution();
    game_restart(g2);
    for(unsigned int i = 0; i < DEFAULT_SIZE; i++){
        for(unsigned int j = 0; j < DEFAULT_SIZE; j++){
            if (game_get_square(g1, i, j) != game_get_square(g2, i, j)){
                return false;
            }
        }
    }
    game_delete(g1);
    game_delete(g2);
    return true;
}


int main(int argc, char *argv[]){

    printf("=> Start test \"%s\"\n", argv[1]);
    bool testPassed = false;

    if (strcmp("game_play_move", argv[1]) == 0){
        testPassed = test_game_play_move();
    }else if (strcmp("game_check_move", argv[1]) == 0){
        testPassed = test_game_check_move();
    }else if (strcmp("game_is_over", argv[1]) == 0){
        testPassed = test_game_is_over();
    }else if (strcmp("game_fill_grass_row", argv[1]) == 0){
        testPassed = test_game_fill_grass_row();
    }else if (strcmp("game_fill_grass_col", argv[1]) == 0){
        testPassed = test_game_fill_grass_col();
    }else if (strcmp("game_restart", argv[1]) == 0){
        testPassed = test_game_restart();
    }else {
        fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (testPassed){
        printf("Test \"%s\" finished: SUCCESS\n", argv[1]);
        return EXIT_SUCCESS;
    }else{
        fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
        return EXIT_FAILURE;
    }
}