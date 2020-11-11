#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "game_aux.h"

/**test_game_print **/
bool test_game_print(void){
	game gm = game_default();
	game_print(gm);
	return true;
}

/**test_game_default **/
bool test_game_default(void){
	game gm = game_default();
	if(!gm){
		return false;
	}
	game_delete(gm);
	return true;
}

/**test_game_default_solution **/
bool test_game_default_solution(void){
	game gms = game_default_solution();
	if(!gms){
		return false;
	}
	game_delete(gms);
	return true;
}

/**test_game_new **/
bool test_game_new(void){
	game gm = game_default();
	unsigned int squares[] = {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
	unsigned int nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
	unsigned int nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
	if(!game_new(squares, nb_tents_row, nb_tents_col)){
		return false;
	}
	game_delete(gm);
	return true;
}

/**test_game_new_empty **/
bool test_game_new_empty(void){
	game gm = game_default();
	for(int i = 0; i<=8; i++){
		for(int j = 0; j<=8; j++){
			if(game_get_square(gm, i, j)!=EMPTY){
				return false;
			}
		}
	}
	game_delete(gm);
	return true;
}

/**test_game_copy **/
bool test_game_copy(void){
	game g1 = game_default();
	game g2 = game_default();
	bool test = game_equal(g1, g2);
	if(!test){
		return false;
	}
	game_delete(g1);
	game_delete(g2);
	return true;
}

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
		testPassed = test_game_new_empty();
	}else if (strcmp("game_copy", argv[1]) == 0){
 		testPassed = test_game_copy();
	}else {
		fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if (testPassed){
		fprintf(stderr,"Test \"%s\" finished: SUCCESS\n", argv[1]);
		return EXIT_SUCCESS;
	}else{
		fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
		return EXIT_FAILURE;
	}
}
