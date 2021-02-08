#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_ext.h"

game game_load(char *filename) {
  game g = game_new_empty();
  return g;
}

void game_save(cgame g, char *filename) {
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    fprintf(stderr, "file couldn't open!\n");
    exit(EXIT_FAILURE);
  }
  fprintf(f, "%u %u ", game_nb_rows(g), game_nb_cols(g));
  fprintf(f, "%d ", game_is_wrapping(g));
  fprintf(f, "%d\n", game_is_diagadj(g));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    fprintf(f, "%u ", game_get_expected_nb_tents_row(g, i));
  }
  fprintf(f, "\n");
  for (uint i = 0; i < game_nb_cols(g); i++) {
    fprintf(f, "%u ", game_get_expected_nb_tents_col(g, i));
  }
  fprintf(f, "\n");
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        fprintf(f, " ");
      } else if (game_get_square(g, i, j) == TREE) {
        fprintf(f, "x");
      } else if (game_get_square(g, i, j) == TENT) {
        fprintf(f, "*");
      } else if (game_get_square(g, i, j) == GRASS) {
        fprintf(f, "-");
      }
    }
    fprintf(f, "\n");
  }
  fclose(f);
}
