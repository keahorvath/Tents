#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"

/**test_game_print **/
bool test_game_print(void){
	game gm = game_default();
	if(!game_print(gm)){
		return false;
	}
	game_delete(gm);
	return true;
}
/**test_game_default **/
bool test_game_default(void);
	game gm = game_default();
	if(!gm){
		return false;
	}
	game_delete(gm);
	return true;

/**test_game_default_solution **/
bool test_game_default_solution(void){
	game gms = game_default_solution()
	if(!gms){
		return false;
	}
	game_delete(gms);
	return true;

/**
 * @brief Creates a new game with default size and initializes it.
 * @param squares an array describing the initial square values (row-major
 * storage)
 * @param nb_tents_row an array with the expected number of tents in each row
 * @param nb_tents_col an array with the expected number of tents in each column
 * @pre @p squares must be an initialized array of default size squared.
 * @pre @p nb_tents_row must be an initialized array of default size.
 * @pre @p nb_tents_col must be an initialized array of default size.
 * @return the created game
 **/
bool test_game_new(square *squares, uint *nb_tents_row, uint *nb_tents_col){
	
}

/**
 * @brief Creates a new empty game with defaut size.
 * @details All squares are initialized with empty squares, and the expected
 * numbers of tents for each row/column are set to zero.
 * @return the created game
 **/
game game_new_empty(void);

/**
 * @brief Duplicates a game.
 * @param g the game to copy
 * @return the copy of the game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
game game_copy(cgame g);

int main(int argc, char *argv[]){
	fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
	bool testPassed = false;
	if (strcmp("game_print", argv[1]) == 0){
		testPassed = test_game_print();
	}else if (strcmp("game_default", argv[1]) == 0){
		testPassed = test_game_default();
	}else if (strcmp("game_default_solution", argv[1]) == 0){
		testPassed = test_game_default_solution();
	}else if (strcmp("game_new", argv[1]) == 0){
		testPassed = test_game_new();
	}else if (strcmp("game_new_empty", argv[1]) == 0){
		testPassed = test_game_empty();
	}else if (strcmp("game_copy", argv[1]) == 0){
 		testPassed = test_game_copy();
	}else {
		fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
 232         exit(EXIT_FAILURE);
 233     }
 234 
 235     if (testPassed){
 236         printf("Test \"%s\" finished: SUCCESS\n", argv[1]);
 237         return EXIT_SUCCESS;
 238     }else{
 239         fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
 240         return EXIT_FAILURE;
 241     }
 242 }
