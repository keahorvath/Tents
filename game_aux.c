#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_ext.h"

void game_print(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "function called on null address");
    exit(EXIT_FAILURE);
  }
  printf("   ");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    printf("%u", i);
  }
  printf("   ");
  printf("\n");
  printf("   --------   \n");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    printf("%u |", i);
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        printf(" ");
      }
      if (game_get_square(g, i, j) == TREE) {
        printf("x");
      }
      if (game_get_square(g, i, j) == TENT) {
        printf("*");
      }
      if (game_get_square(g, i, j) == GRASS) {
        printf("-");
      }
    }
    printf("| %u\n", game_get_expected_nb_tents_row(g, i));
  }
  printf("   --------   \n");
  printf("   ");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    printf("%u", game_get_expected_nb_tents_col(g, i));
  }
  printf("   \n");
}

game game_default(void) {
  square square[] = {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                     0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                     0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
  uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
  return game_new(square, nb_tents_row, nb_tents_col);
}

game game_default_solution(void) {
  square square[] = {2, 3, 3, 2, 1, 1, 2, 3, 1, 3, 3, 3, 3, 3, 3, 1,
                     2, 3, 3, 2, 1, 2, 3, 2, 1, 3, 3, 3, 3, 1, 3, 3,
                     2, 1, 2, 3, 2, 3, 2, 3, 1, 3, 3, 3, 1, 3, 1, 3,
                     2, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 3};
  uint nb_tents_row[] = {3, 0, 4, 0, 4, 0, 1, 0};
  uint nb_tents_col[] = {4, 0, 1, 2, 1, 1, 2, 1};
  return game_new(square, nb_tents_row, nb_tents_col);
}
