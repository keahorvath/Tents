#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "game.h"



/**
 * @brief Prints a game as text on the standard output stream.
 * @details The different squares EMPTY, TREE, TENT, and GRASS are respectively
 * displayed with the following characters ' ', 'x', '*', '-'.
 * Here is the attempted output for the default game:
@verbatim
   01234567
   --------
0 |    xx  | 3
1 |x      x| 0
2 |    x   | 4
3 |x    x  | 0
4 | x      | 4
5 |x   x x | 0
6 |        | 1
7 |x       | 0
   --------
   40121121
@endverbatim
 * @param g the game
 * @pre @p g must be a valid pointer toward a game structure.
 **/

void game_print(cgame g){
	if (g == NULL){
		fprintf(stderr, "function called on null address");
		exit(EXIT_FAILURE);
	}
	printf("   ");
	for (uint i = 1; i < 8; i++){
		printf("%u", i);
	}
	printf("\n");
	printf("   --------");
	for (uint i = 0; i < 8; i++){
		printf("%u |", i);
		for (uint j = 0; j < 8; j++){
			if (game_get_square(g, i, j) == EMPTY){
				printf(" ");
			}
			if (game_get_square(g, i, j) == TREE){
				printf("x");
			}
			if (game_get_square(g, i, j) == TENT){
				printf("*");
			}
			if (game_get_square(g, i, j) == GRASS){
				printf("-");
			}
		}
		printf("| %u\n", game_get_expected_nb_tents_row(g, i));
	}
	printf("   --------\n");
	printf("   ");
	for (uint i = 0; i < 8; i++){
		printf("%u", game_get_expected_nb_tents_col(g, i));
	}

}

game game_default(void){
	square square[] = {0, 0, 0, 0, 1, 1, 0, 0, 1 ,0 ,0, 0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 , 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
	uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
	uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};

	return game_new(square, nb_tents_row, nb_tents_col);
}

game game_default_solution(void){
	square square[] = {2, 3, 3, 2, 1, 1, 2, 3, 1, 3, 3, 3, 3, 3, 3, 1, 2, 3, 3, 2, 1, 2, 3, 2, 1, 3, 3, 3, 3, 1, 3, 3, 2, 1, 2, 3, 2, 3, 2, 3, 1, 3, 3, 3, 1, 3, 1, 3, 2, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3};
	uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
	uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
	return game_new(square, nb_tents_row, nb_tents_col);
}


