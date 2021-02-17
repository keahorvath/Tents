#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "queue.h"

bool test_game_play_move(void) {
  game g0 = game_default();
  game g1 = game_default();
  game g2 = game_default();
  game g3 = game_default();
  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g0, i, j) != TREE) {
        game_play_move(g1, i, j, EMPTY);
        game_play_move(g2, i, j, TENT);
        game_play_move(g3, i, j, GRASS);
      }
      if (game_get_square(g0, i, j) == TREE) {
        if (game_get_square(g1, i, j) != TREE ||
            game_get_square(g2, i, j) != TREE ||
            game_get_square(g3, i, j) != TREE) {
          return false;
        }
      } else if (game_get_square(g1, i, j) != EMPTY ||
                 game_get_square(g2, i, j) != TENT ||
                 game_get_square(g3, i, j) != GRASS) {
        return false;
      }
      if (!(i == DEFAULT_SIZE - 1 || j == DEFAULT_SIZE - 1)) {
        for (unsigned int k = i + 1; k < DEFAULT_SIZE; k++) {
          for (unsigned int l = j + 1; l < DEFAULT_SIZE; l++) {
            if (game_get_square(g0, k, l) != game_get_square(g1, k, l) ||
                game_get_square(g0, k, l) != game_get_square(g2, k, l) ||
                game_get_square(g0, k, l) != game_get_square(g3, k, l)) {
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

bool test_game_check_move(void) {
  game g = game_default();

  // test 1 if replacing tree is illegal
  if (game_check_move(g, 0, 4, TENT) != ILLEGAL ||
      game_check_move(g, 0, 4, GRASS) != ILLEGAL ||
      game_check_move(g, 0, 4, EMPTY) != ILLEGAL) {
    return false;
  }

  // test 2 if placing new tree is illegal
  if (game_check_move(g, 1, 1, TREE) != ILLEGAL) {
    return false;
  }

  // test 3/4 if placing n+1 tents in row or column is losing
  if (game_check_move(g, 1, 1, TENT) != LOSING) {
    return false;
  }

  // test 5 if placing tent adjacent to tent is losing
  square square1[] = {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                      0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
                      0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row1[] = {3, 1, 4, 0, 4, 0, 1, 0};
  uint nb_tents_col1[] = {4, 0, 1, 2, 1, 1, 2, 1};
  game ga = game_new(square1, nb_tents_row1, nb_tents_col1);
  game_play_move(g, 0, 6, TENT);
  game_play_move(ga, 0, 6, TENT);
  if (game_check_move(g, 0, 7, TENT) != LOSING ||
      game_check_move(ga, 1, 5, TENT) != LOSING) {
    return false;
  }
  // test 6 if placing tent in row and column with no tents required is losing
  if (game_check_move(g, 7, 1, TENT) != LOSING ||
      game_check_move(g, 3, 7, TENT) != LOSING) {
    return false;
  }
  // test 7 if placing grass and not enough empty squares for tents is losing
  if (game_check_move(g, 0, 0, GRASS) != LOSING) {
    return false;
  }

  // test 8 if surronding tree by grass is losing
  game g1 = game_default();
  game_play_move(g1, 5, 3, GRASS);
  game_play_move(g1, 6, 4, GRASS);
  game_play_move(g1, 5, 5, GRASS);
  if (game_check_move(g1, 4, 4, GRASS) != LOSING) {
    return false;
  }

  // test 9 if placing tent with no tree around is losing
  square squares[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row[] = {0, 0, 1, 0, 0, 0, 0, 0};
  uint nb_tents_col[] = {0, 0, 1, 0, 0, 0, 0, 0};
  game g2 = game_new(squares, nb_tents_row, nb_tents_col);
  if (game_check_move(g2, 2, 2, TENT) != LOSING) {
    return false;
  }
  if (game_check_move(g, 0, 2, TENT) != LOSING) {
    return false;
  }

  // test 10 if placing more tents than trees is losing
  square squares3[] = {0, TREE, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row3[] = {2, 0, 0};
  uint nb_tents_col3[] = {1, 0, 1};
  game g3 =
      game_new_ext(3, 3, squares3, nb_tents_row3, nb_tents_col3, false, false);
  game_play_move(g3, 0, 0, TENT);
  if (game_check_move(g, 0, 2, TENT) != LOSING) {
    return false;
  }

  // test 11 if wrapping works
  square squares4[] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row4[] = {1, 0, 0};
  uint nb_tents_col4[] = {0, 0, 1};
  game g4 =
      game_new_ext(3, 3, squares4, nb_tents_row4, nb_tents_col4, true, false);
  if (game_check_move(g4, 0, 2, TENT) == LOSING) {
    return false;
  }
  square squares5[] = {TREE, TENT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TREE, 0, 0, 0};
  uint nb_tents_row5[] = {1, 0, 0, 1};
  uint nb_tents_col5[] = {0, 2, 0, 0};
  game g5 =
      game_new_ext(4, 4, squares5, nb_tents_row5, nb_tents_col5, true, false);
  if (game_check_move(g5, 3, 1, TENT) != LOSING) {
    return false;
  }
  game g6 =
      game_new_ext(4, 4, squares5, nb_tents_row5, nb_tents_col5, false, false);
  if (game_check_move(g6, 3, 1, TENT) == LOSING) {
    return false;
  }

  // test 12 if diagajd works
  square squares7[] = {TREE, TENT, 0, 0, TREE, 0, 0, 0, 0};
  uint nb_tents_row7[] = {1, 1, 0};
  uint nb_tents_col7[] = {1, 1, 0};
  game g7 =
      game_new_ext(3, 3, squares7, nb_tents_row7, nb_tents_col7, false, true);
  if (game_check_move(g7, 1, 0, TENT) == LOSING) {
    return false;
  }
  game g8 =
      game_new_ext(3, 3, squares7, nb_tents_row7, nb_tents_col7, false, false);
  if (game_check_move(g8, 1, 0, TENT) != LOSING) {
    return false;
  }

  game_delete(g);
  game_delete(ga);
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  game_delete(g6);
  game_delete(g7);
  game_delete(g8);
  return true;
}

bool test_game_is_over(void) {
  game g = game_default_solution();
  // test 1 if the correct default solution returns an error
  if (!game_is_over(g)) {
    return false;
  }
  // test 2 if all tents connected to tree and vice versa
  square squares[] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row[] = {0, 1, 1, 0, 0, 0, 0, 0};
  uint nb_tents_col[] = {1, 0, 0, 0, 0, 1, 0, 0};
  game g2 = game_new(squares, nb_tents_row, nb_tents_col);
  game_set_square(g2, 2, 0, TENT);
  game_set_square(g2, 1, 5, TENT);
  if (game_is_over(g2)) {
    return false;
  }

  // test 3 if tents are adjacent
  square squares3[] = {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row3[] = {2, 0, 1, 0, 0, 0, 0, 0};
  uint nb_tents_col3[] = {0, 0, 0, 1, 1, 0, 1, 0};
  game g3 = game_new(squares3, nb_tents_row3, nb_tents_col3);
  game_set_square(g3, 0, 3, TENT);
  game_set_square(g3, 0, 4, TENT);
  game_set_square(g3, 2, 6, TENT);
  if (game_is_over(g3)) {
    return false;
  }

  // test 4 if not having the right nb of tents in row/column still returns true
  game_set_square(g, 0, 0, GRASS);
  if (game_is_over(g)) {
    return false;
  }
  game_set_square(g, 0, 0, TENT);

  // test 5 if having tent not next to tree still returns true
  square squares4[] = {0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 0,
                       0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row4[] = {0, 1, 1, 0, 0, 0, 0, 0};
  uint nb_tents_col4[] = {1, 0, 1, 0, 0, 0, 0, 0};
  game g4 = game_new(squares4, nb_tents_row4, nb_tents_col4);
  if (game_is_over(g3)) {
    return false;
  }

  // test 6 if nb tents = nb trees
  square squares5[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0,
                       0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row5[] = {0, 1, 0, 0, 0, 0, 0, 0};
  uint nb_tents_col5[] = {0, 0, 1, 0, 0, 0, 0, 0};
  game g5 = game_new(squares5, nb_tents_row5, nb_tents_col5);
  if (game_is_over(g5)) {
    return false;
  }

  game_delete(g);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  return true;
}

bool test_game_fill_grass_row(void) {
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
  if (!game_equal(g, g2)) {
    return false;
  }
  square squares[] = {TREE, TENT, 0, TENT, TREE, 0, 0, 0, 0};
  square squares0[] = {TREE, TENT, 0, TENT, TREE, 0, GRASS, GRASS, 0};
  uint nb_tents_row[] = {1, 1, 0};
  uint nb_tents_col[] = {1, 1, 0};
  game g3 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, false, true);
  game g4 =
      game_new_ext(3, 3, squares0, nb_tents_row, nb_tents_col, false, true);
  game_fill_grass_row(g3, 2);
  game_undo(g3);
  if (!game_equal(g3, g4)) {
    return false;
  }
  game_delete(g);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);

  return true;
}

bool test_game_fill_grass_col(void) {
  game g = game_default();
  game_play_move(g, 2, 0, GRASS);
  game_play_move(g, 4, 0, TENT);
  game g2 = game_default();
  game_play_move(g2, 2, 0, GRASS);
  game_play_move(g2, 4, 0, TENT);
  game_play_move(g2, 0, 0, GRASS);
  game_play_move(g2, 6, 0, GRASS);
  game_fill_grass_col(g, 0);

  if (!game_equal(g, g2)) {
    return false;
  }
  square squares[] = {TREE, TENT, 0, TENT, TREE, 0, 0, 0, 0};
  square squares0[] = {TREE, TENT, GRASS, TENT, TREE, GRASS, 0, 0, 0};
  uint nb_tents_row[] = {1, 1, 0};
  uint nb_tents_col[] = {1, 1, 0};
  game g3 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, false, true);
  game g4 =
      game_new_ext(3, 3, squares0, nb_tents_row, nb_tents_col, false, true);
  game_fill_grass_col(g3, 2);
  game_undo(g3);
  if (!game_equal(g3, g4)) {
    return false;
  }
  game_delete(g);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_game_restart(void) {
  game g1 = game_default();
  game g2 = game_default_solution();
  game_restart(g2);
  for (unsigned int i = 0; i < DEFAULT_SIZE; i++) {
    for (unsigned int j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g1, i, j) != game_get_square(g2, i, j)) {
        return false;
      }
    }
  }
  // test if undo list is empty
  square squares[] = {TREE, TENT, 0, TENT, TREE, 0, 0, 0, 0};
  square squares0[] = {TREE, 0, 0, 0, TREE, 0, 0, 0, 0};
  uint nb_tents_row[] = {1, 1, 0};
  uint nb_tents_col[] = {1, 1, 0};
  game g3 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, false, true);
  game g4 =
      game_new_ext(3, 3, squares0, nb_tents_row, nb_tents_col, false, true);
  game_play_move(g3, 0, 1, EMPTY);
  game_restart(g3);
  game_undo(g3);
  if (!game_equal(g3, g4)) {
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_game_new_ext(void) {
  game gm = game_new_empty_ext(3, 3, false, true);
  game_set_square(gm, 0, 0, TREE);
  game_set_square(gm, 1, 1, TREE);
  game_set_expected_nb_tents_col(gm, 0, 1);
  game_set_expected_nb_tents_col(gm, 1, 1);
  game_set_expected_nb_tents_col(gm, 2, 0);
  game_set_expected_nb_tents_row(gm, 0, 1);
  game_set_expected_nb_tents_row(gm, 1, 1);
  game_set_expected_nb_tents_row(gm, 2, 0);

  square squares[] = {1, 0, 0, 0, 1, 0, 0, 0, 0};
  uint nb_tents_row[] = {1, 1, 0};
  uint nb_tents_col[] = {1, 1, 0};
  game gm2 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, false, true);
  if (!game_equal(gm, gm2)) {
    return false;
  }
  game_delete(gm);
  game_delete(gm2);
  return true;
}

bool test_game_new_empty_ext(void) {
  game gm = game_new_empty_ext(4, 3, true, false);
  for (uint i = 0; i < 4; i++) {
    if (game_get_expected_nb_tents_row(gm, i) != 0) {
      return false;
    }
  }
  for (uint j = 0; j < 3; j++) {
    if (game_get_expected_nb_tents_row(gm, j) != 0) {
      return false;
    }
  }
  for (uint i = 0; i < 4; i++) {
    for (uint j = 0; j < 3; j++) {
      if (game_get_square(gm, i, j) != EMPTY) {
        return false;
      }
    }
  }
  if (game_is_wrapping(gm) != true || game_is_diagadj(gm) != false) {
    return false;
  }
  game_delete(gm);
  return true;
}

bool test_game_load(void) {
  game g1 = game_default_solution();
  game_save(g1, "save_1.tnt");
  game g2 = game_load("save_1.tnt");
  if (!game_equal(g1, g2)) {
    return false;
  }
  square squares[] = {TREE, TENT, EMPTY, GRASS, TENT, TREE, GRASS, EMPTY};
  uint nb_tents_row[] = {1, 1};
  uint nb_tents_col[] = {0, 1, 0, 1};
  game g3 =
      game_new_ext(2, 4, squares, nb_tents_row, nb_tents_col, false, true);
  game_save(g3, "save_2.tnt");
  game g4 = game_load("save_2.tnt");
  if (!game_equal(g3, g4)) {
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  return true;
}

bool test_game_save(void) {
  square squares[] = {TREE, TENT, EMPTY, GRASS, TENT, TREE, GRASS, EMPTY};
  uint rows[] = {1, 1};
  uint cols[] = {0, 1, 0, 1};
  game g = game_new_ext(2, 4, squares, rows, cols, false, true);
  game_save(g, "save_1.tnt");
  FILE* f = fopen("save_1.tnt", "r");
  if (f == NULL) {
    fprintf(stderr, "File couldn't open!\n");
    exit(EXIT_FAILURE);
  }
  unsigned int nb_rows, nb_cols, is_wrapping, is_diagadj;
  int i = fscanf(f, "%u%*c%u%*c%u%*c%u%*c", &nb_rows, &nb_cols, &is_wrapping,
                 &is_diagadj);
  if (nb_rows != game_nb_rows(g) || nb_cols != game_nb_cols(g) ||
      is_wrapping != game_is_wrapping(g) || is_diagadj != game_is_diagadj(g)) {
    return false;
  }
  uint nb_tents_row[nb_rows];
  uint nb_tents_col[nb_cols];
  // load row line
  for (int indice = 0; indice < nb_rows; indice++) {
    i = fscanf(f, "%u%*c", nb_tents_row + indice);
    if (nb_tents_row[indice] != game_get_expected_nb_tents_row(g, indice)) {
      return false;
    }
  }
  // load column line
  for (int indice = 0; indice < nb_cols; indice++) {
    i = fscanf(f, "%u%*c", nb_tents_col + indice);
    if (nb_tents_col[indice] != game_get_expected_nb_tents_col(g, indice)) {
      return false;
    }
  }
  fseek(f, 1, SEEK_CUR);  // skip the character '\n '
  // load the grill of the game
  square square[nb_rows * nb_cols];
  for (uint indice = 0; indice < nb_rows * nb_cols; indice++) {
    char s;
    if ((indice != 0) && (indice != (nb_rows * nb_cols - 1)) &&
        (indice % nb_cols == 0)) {
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
    if (square[indice] !=
        game_get_square(g, (indice) / nb_cols, (indice) % nb_cols)) {
      return false;
    }
  }
  game_delete(g);
  return true;
}

bool test_game_solve(void){
  game g1 = game_default();
  game g2 = game_default_solution();
  if (!game_solve(g1)){
    game_print(g1);
    return false;
  }
  game_print(g1);
  if (!game_equal(g1, g2)){
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  return true;
}

int main(int argc, char* argv[]) {
  printf("=> Start test \"%s\"\n", argv[1]);
  bool testPassed = false;

  if (strcmp("game_play_move", argv[1]) == 0) {
    testPassed = test_game_play_move();
  } else if (strcmp("game_check_move", argv[1]) == 0) {
    testPassed = test_game_check_move();
  } else if (strcmp("game_is_over", argv[1]) == 0) {
    testPassed = test_game_is_over();
  } else if (strcmp("game_fill_grass_row", argv[1]) == 0) {
    testPassed = test_game_fill_grass_row();
  } else if (strcmp("game_fill_grass_col", argv[1]) == 0) {
    testPassed = test_game_fill_grass_col();
  } else if (strcmp("game_restart", argv[1]) == 0) {
    testPassed = test_game_restart();
  } else if (strcmp("game_new_empty_ext", argv[1]) == 0) {
    testPassed = test_game_new_empty_ext();
  } else if (strcmp("game_new_ext", argv[1]) == 0) {
    testPassed = test_game_new_ext();
  } else if (strcmp("game_load", argv[1]) == 0) {
    testPassed = test_game_load();
  } else if (strcmp("game_save", argv[1]) == 0) {
    testPassed = test_game_save();
  }else if (strcmp("game_solve", argv[1]) == 0){
    testPassed = test_game_solve();
  } else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  if (testPassed) {
    printf("Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}