#include "game_tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_ext.h"
#include "game_aux.h"

static bool game_is_full(cgame g);
static bool game_solve_rec(game g, uint total_nb_moves);

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

bool game_solve_rec(game g, uint total_nb_moves){
  if (game_is_over(g)) {
    return true;
  }
  if (game_is_full(g)) {
    //if the game doesn't have a solution we have to bring it back to its original state
    for (uint i = 0; i < total_nb_moves; i++){
      game_undo(g);
    }
    return false;
  }
  uint nb_of_moves_made = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY &&
          game_check_move(g, i, j, TENT) == LOSING) {
        game_play_move(g, i, j, GRASS);
        nb_of_moves_made++;
        total_nb_moves++;
      } else if (game_get_square(g, i, j) == EMPTY &&
                 game_check_move(g, i, j, GRASS) == LOSING) {
        game_play_move(g, i, j, TENT);
        nb_of_moves_made++;
        total_nb_moves++;
      }
    }
  }
  // if the nb_of_moves_made is 0, it means that any of the empty cells can be a
  // tent
  if (nb_of_moves_made == 0) {
    for (uint i = 0; i < game_nb_rows(g); i++) {
      for (uint j = 0; j < game_nb_cols(g); j++) {
        // choose the first empty cell found and put a tent
        if (game_get_square(g, i, j) == EMPTY) {
          game_play_move(g, i, j, TENT);
          total_nb_moves++;
          return game_solve_rec(g, total_nb_moves);
        }
      }
    }
  }
  return game_solve_rec(g, total_nb_moves);
}

bool game_solve(game g) {
  bool game_is_solved = game_solve_rec(g, 0);
  if (!game_is_solved){
    return false;
  }
  return true;
}

uint game_nb_solutions(game g) { return 0; }

bool game_is_full(cgame g) {
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == EMPTY) {
        return false;
      }
    }
  }
  return true;
}

/*int main(int argc, char *argv[]) {
  char option = argv[1];
  FILE f;
  int ret1 = scanf("%c", &option);
    if (option == '-s'){
      f = open(argv[3], "a");
      f.write(game_solve_rec(argv[2]));
      f.close();

    }
    else if (option == '-c') {
      f = open(argv[3], "a");
      f.write(game_nb_solutions(argv[2]));
      f.close();
    }
}*/