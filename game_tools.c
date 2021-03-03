#include "game_tools.h"
#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

static bool game_solve_rec(game g);
static int game_fill(game g);
static uint game_nb_solutions_rec(game g, uint nb_solutions);
game game_load(char *filename) {
  FILE *f;
  f = fopen(filename, "r");
  if (f != NULL) {
    unsigned int nb_rows, nb_cols, is_wrapping, is_diagadj;
    int i = fscanf(f, "%u%*c%u%*c%u%*c%u%*c", &nb_rows, &nb_cols, &is_wrapping,
                   &is_diagadj);
    unsigned int nb_tents_row[nb_rows];
    unsigned int nb_tents_col[nb_cols];
    // load row line
    for (int indice = 0; indice < nb_rows; indice++) {
      i = fscanf(f, "%u%*c", nb_tents_row + indice);
    }
    // load column line
    for (int indice = 0; indice < nb_cols; indice++) {
      i = fscanf(f, "%u%*c", nb_tents_col + indice);
    }
    fseek(f, 1, SEEK_CUR);  // skip the character '\n '
    // load the grill of the game
    square square[nb_rows * nb_cols];
    for (int indice = 0; indice < nb_rows * nb_cols; indice++) {
      char s;
      if ((indice != 0) && (indice != (nb_rows * nb_cols - 1)) &&
          (indice % (nb_cols) == 0)) {
        fseek(f, 1, SEEK_CUR);  // skip the character '\n '
      }
      i = fscanf(f, "%c", &s);
      if ((i) && (s == ' ')) {
        square[indice] = EMPTY;
      }
      if ((i) && (s == 'x')) {
        square[indice] = TREE;
      }
      if ((i) && (s == '*')) {
        square[indice] = TENT;
      }
      if ((i) && (s == '-')) {
        square[indice] = GRASS;
      }
    }
    game g = game_new_ext(nb_rows, nb_cols, square, nb_tents_row, nb_tents_col,
                          is_wrapping, is_diagadj);
    if (g == NULL) {
      return NULL;
    }
    return g;
  }
  return NULL;
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

bool game_solve_rec(game g) {
  if (game_is_over(g)) {
    return true;
  }
  uint nb_moves;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        game_play_move(g, i, j, TENT);
        nb_moves = game_fill(g);
        if (nb_moves == -1) {
          game_play_move(g, i, j, GRASS);
          continue;
        }
        game_solve_rec(g);
        if (game_is_over(g)) {
          return true;
        }
        while (game_get_square(g, i, j) != EMPTY) {
          game_undo(g);
        }
      }
    }
  }
  return false;
}

bool game_solve(game g) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  uint nb_moves = game_fill(g);
  bool game_is_solved = game_solve_rec(g);
  if (!game_is_solved) {
    for (uint i = 0; i < nb_moves; i++) {
      game_undo(g);
    }
    return false;
  }
  return true;
}

int game_fill(game g) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  uint nb_moves = 1;
  uint total_nb_moves = 0;
  while (nb_moves != 0) {
    nb_moves = 0;
    for (uint i = 0; i < game_nb_rows(g); i++) {
      for (uint j = 0; j < game_nb_cols(g); j++) {
        if (game_get_square(g, i, j) == EMPTY &&
            game_check_move(g, i, j, TENT) == LOSING &&
            game_check_move(g, i, j, GRASS) == REGULAR) {
          game_play_move(g, i, j, GRASS);
          nb_moves++;
          total_nb_moves++;
        } else if (game_get_square(g, i, j) == EMPTY &&
                   game_check_move(g, i, j, GRASS) == LOSING &&
                   game_check_move(g, i, j, TENT) == REGULAR) {
          game_play_move(g, i, j, TENT);
          nb_moves++;
          total_nb_moves++;
        } else if (game_get_square(g, i, j) == EMPTY &&
                   game_check_move(g, i, j, TENT) == LOSING &&
                   game_check_move(g, i, j, GRASS) == LOSING) {
          for (uint nb = 0; nb < total_nb_moves + 1; nb++) {
            game_undo(g);
          }
          return -1;
        }
      }
    }
  }
  return total_nb_moves;
}

uint game_nb_solutions_rec(game g, uint nb_solutions) {
  uint cpt = 0;
  uint nb_moves;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        game_play_move(g, i, j, TENT);
        nb_moves = game_fill(g);
        if (nb_moves == -1) {
          game_play_move(g, i, j, GRASS);
          continue;
        }
        game_print(g);
        game_nb_solutions_rec(g, nb_solutions);
        if (game_is_over(g)) {
          printf("One solution\n");
          cpt++;
        }
        while (game_get_square(g, i, j) != EMPTY) {
          game_undo(g);
        }
      }
    }
  }
  return 1;
}

uint game_nb_solutions(game g) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  uint nb_moves = game_fill(g);
  return 1;
  // return game_nb_solutions_rec(g, 0);
}
