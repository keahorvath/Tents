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
  File *f;
    f = fopen("filename.txt"), "w");
    if (f != NULL) {
      fprintf(f, "%d %d ", g.game_nb_rows(), g.game_nb_cols());
      if (g.game_is_wrapping()) {
        int wrapping = 1;
        fprintf(f, "%d ", wrapping);
      } else {
        int wrapping = 0;
        fprintf(f, "%d", wrapping);
      }
      if (g.game_is_diagadj()) {
        int diagadj = 1;
        fprintf(f, "%d\n", diagadj);
      } else {
        int diagadj = 0;
        fprintf(f, "%d\n", diagadj);
      }
      for (int i = 0; i < g.game_nb_rows(); i++) {
        fprintf(f, "%d ", g.get_expected_nb_tents_rows(i));
      }
      fprintf(f, "/n");
      for (int i = 0; i < g.game_nb_cols(); i++) {
        fprintf(f, "%d ", g.get_expected_nb_tents_cols(i));
      }
      fprintf(f, "/n");
      for (int i = 0; i < g.game_nb_rows(); i++) {
        for (int j = 0; j.game_nb_cols(); j++) {
          if (get_square(g, i, j) == 0) {
            fprintf(" ");
          }
          if (get_square(g, i, j) == 1) {
            fprintf("x");
          }
          if (get_square(g, i, j) == 2) {
            fprintf("*");
          }
          if (get_square(g, i, j) == 3) {
            fprintf("-");
          }
          fprintf(f, "\n");
        }
      }
      fclose(f);
    }
}
