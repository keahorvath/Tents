#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/*Tests*/

// Test game_get_expected_nb_tents_row
bool test_game_get_expected_nb_tents_row(int row) {
  if ((row < 0) | (row > 7)) {
    fprintf(stderr, "Error : this row doesn't exist\n");
    return false;
  }
  game game = game_default_solution();
  if (game == NULL) {
    fprintf(stderr, "Error : invalid game\n");
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 0) != 3) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 1) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 2) != 4) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 3) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 4) != 4) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 5) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 6) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_row(game, 7) != 0) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_get_expected_nb_tents_col
bool test_game_get_expected_nb_tents_col(int col) {
  if ((col < 0) | (col > 7)) {
    fprintf(stderr, "Error: this column doesn't exist\n");
    return false;
  }
  game game = game_default();
  if (game == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 0) != 4) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 1) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 2) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 3) != 2) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 4) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 5) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 6) != 2) {
    return false;
  }
  if (game_get_expected_nb_tents_col(game, 7) != 1) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_get_expected_nb_tents_all
bool test_game_get_expected_nb_tents_all(void) {
  game game = game_default();
  if (game == NULL) {
    fprintf(stderr, "Error : invalid game\n");
    return false;
  }
  if (game_get_expected_nb_tents_all(game) != 12) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_get_current_nb_tents_row
bool test_game_get_current_nb_tents_row(int row) {
  if ((row < 0) | (row > 7)) {
    fprintf(stderr, "Error: this row doesn't exist\n");
    return false;
  }
  game game = game_default_solution();
  if (game == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_get_current_nb_tents_row(game, 0) != 3) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 1) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 2) != 4) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 3) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 4) != 4) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 5) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 6) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_row(game, 7) != 0) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_get_current_nb_tents_col
bool test_game_get_current_nb_tents_col(int col) {
  if ((col < 0) | (col > 7)) {
    fprintf(stderr, "Error: this column doesn't exist\n");
    return false;
  }
  game game = game_default_solution();
  if (game == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_get_current_nb_tents_col(game, 0) != 4) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 1) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 2) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 3) != 2) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 4) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 5) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 6) != 2) {
    return false;
  }
  if (game_get_current_nb_tents_col(game, 7) != 1) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_get_current_nb_tents_all
bool test_game_get_current_nb_tents_all(void) {
  game game = game_default();
  if (game == NULL) {
    fprintf(stderr, "Error : invalid game\n");
    return false;
  }
  int tents = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      square s = game_get_square(game, i, j);
      if (s == 2) {
        tents++;
      }
    }
  }
  if (tents != game_get_current_nb_tents_all(game)) {
    fprintf(stderr, "Error: unexpected amount of tents\n");
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_nb_rows
bool test_game_nb_rows() {
  game game = game_new_empty_ext(4, 5, true, true);
  if (game == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_nb_rows(game) != 4) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_nb_cols
bool test_game_nb_cols() {
  game game = game_new_empty_ext(4, 5, true, true);
  if (game == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_nb_cols(game) != 5) {
    return false;
  }
  game_delete(game);
  return true;
};

// Test game_is_wrapping
bool test_game_is_wrapping() {
  game game1 = game_new_empty_ext(4, 5, true, true);
  game game2 = game_new_empty_ext(4, 5, false, false);
  if (game1 == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game2 == NULL) {
    fprintf(stderr, "Error :invalid game\n");
    return false;
  }
  if (!game_is_wrapping(game1)) {
    return false;
  }
  if (game_is_wrapping(game2)) {
    return false;
  }
  game_delete(game1);
  game_delete(game2);
  return true;
};

// Test game_is_diagadj
bool test_game_is_diagadj() {
  game game1 = game_new_empty_ext(4, 5, true, true);
  game game2 = game_new_empty_ext(4, 5, false, false);
  if (game1 == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game2 == NULL) {
    fprintf(stderr, "Error :invalid game\n");
    return false;
  }
  if (!game_is_diagadj(game1)) {
    return false;
  }
  if (game_is_diagadj(game2)) {
    return false;
  }
  game_delete(game1);
  game_delete(game2);
  return true;
};

// Main routine
int main(int argc, char *argv[]) {
  // start tests
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool ok = false;
  if (strcmp("game_get_expected_nb_tents_row", argv[1]) == 0) {
    ok = test_game_get_expected_nb_tents_row(0);
  } else if (strcmp("game_get_expected_nb_tents_col", argv[1]) == 0) {
    ok = test_game_get_expected_nb_tents_col(0);
  } else if (strcmp("game_get_expected_nb_tents_all", argv[1]) == 0) {
    ok = test_game_get_expected_nb_tents_all();
  } else if (strcmp("game_get_current_nb_tents_row", argv[1]) == 0) {
    ok = test_game_get_current_nb_tents_row(0);
  } else if (strcmp("game_get_current_nb_tents_col", argv[1]) == 0) {
    ok = test_game_get_current_nb_tents_col(0);
  } else if (strcmp("game_get_current_nb_tents_all", argv[1]) == 0) {
    ok = test_game_get_current_nb_tents_all();
  } else if (strcmp("game_nb_rows", argv[1]) == 0) {
    ok = test_game_nb_rows();
  } else if (strcmp("game_nb_cols", argv[1]) == 0) {
    ok = test_game_nb_cols();
  } else if (strcmp("game_is_wrapping", argv[1]) == 0) {
    ok = test_game_is_wrapping();
  } else if (strcmp("game_is_diagadj", argv[1]) == 0) {
    ok = test_game_is_diagadj();
  } else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  // print test result
  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}