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
  FILE *f;
  f = fopen(filename, "w");
  if (f != NULL) {
    fprintf(f, "%d %d ", game_nb_rows(g), game_nb_cols(g));
    fprintf(f, "%d ", game_is_wrapping(g));
    fprintf(f, "%d\n", game_is_diagadj(g));
    for (int i = 0; i < game_nb_rows(g); i++) {
      fprintf(f, "%d ", game_get_expected_nb_tents_rows(g, i));
    }
    fprintf(f, "/n");
    for (int i = 0; i < game_nb_cols(g); i++) {
      fprintf(f, "%d ", game_get_expected_nb_tents_cols(g, i));
    }
    fprintf(f, "/n");
    for (int i = 0; i < game_nb_rows(g); i++) {
      for (int j = 0; game_nb_cols(g); j++) {
        if (game_get_square(g, i, j) == 0) {
          fprintf(f, " ");
        }
        if (game_get_square(g, i, j) == 1) {
          fprintf(f, "x");
        }
        if (game_get_square(g, i, j) == 2) {
          fprintf(f, "*");
        }
        if (game_get_square(g, i, j) == 3) {
          fprintf(f, "-");
        }
        fprintf(f, "\n");
      }
    }
    fclose(f);
  }
}