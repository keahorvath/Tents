#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/*Tests*/

// Test game_get_expected_nb_tents_row
bool test_game_get_expected_nb_tents_row(int k) {
  if ((k < 0) | (k > 7)) {
    fprintf(stderr, "Error : this row doesn't exist\n");
    return false;
  }
  game g = game_default_solution();
  if (g == NULL) {
    fprintf(stderr, "Error : invalid game\n");
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 0) != 3) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 1) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 2) != 4) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 3) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 4) != 4) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 5) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 6) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_row(g, 7) != 0) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_get_expected_nb_tents_col
bool test_game_get_expected_nb_tents_col(int k) {
  if ((k < 0) | (k > 7)) {
    fprintf(stderr, "Error: this column doesn't exist\n");
    return false;
  }
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 0) != 4) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 1) != 0) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 2) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 3) != 2) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 4) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 5) != 1) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 6) != 2) {
    return false;
  }
  if (game_get_expected_nb_tents_col(g, 7) != 1) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_get_expected_nb_tents_all
bool test_game_get_expected_nb_tents_all(void) {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "Error : invalid game\n");
    return false;
  }
  if (game_get_expected_nb_tents_all(g) != 12) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_get_current_nb_tents_row
bool test_game_get_current_nb_tents_row(int k) {
  if ((k < 0) | (k > 7)) {
    fprintf(stderr, "Error: this row doesn't exist\n");
    return false;
  }
  game g = game_default_solution();
  if (g == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_get_current_nb_tents_row(g, 0) != 3) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 1) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 2) != 4) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 3) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 4) != 4) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 5) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 6) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_row(g, 7) != 0) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_get_current_nb_tents_col
bool test_game_get_current_nb_tents_col(int k) {
  if ((k < 0) | (k > 7)) {
    fprintf(stderr, "Error: this column doesn't exist\n");
    return false;
  }
  game g = game_default_solution();
  if (g == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_get_current_nb_tents_col(g, 0) != 4) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 1) != 0) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 2) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 3) != 2) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 4) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 5) != 1) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 6) != 2) {
    return false;
  }
  if (game_get_current_nb_tents_col(g, 7) != 1) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_get_current_nb_tents_all
bool test_game_get_current_nb_tents_all(void) {
  game g = game_default();
  if (g == NULL) {
    fprintf(stderr, "Error : invalid game\n");
    return false;
  }
  int tents = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      square a = game_get_square(g, i, j);
      if (a == 2) {
        tents++;
      }
    }
  }
  if (tents != game_get_current_nb_tents_all(g)) {
    fprintf(stderr, "Error: unexpected amount of tents\n");
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_nb_rows
bool test_game_nb_rows() {
  game g = game_new_empty_ext(4, 5, true, true);
  if (g == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_nb_rows(g) != 4) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_nb_cols
bool test_game_nb_cols() {
  game g = game_new_empty_ext(4, 5, true, true);
  if (g == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (game_nb_cols(g) != 5) {
    return false;
  }
  game_delete(g);
  return true;
};

// Test game_is_wrapping
bool test_game_is_wrapping() {
  game g1 = game_new_empty_ext(4, 5, true, true);
  game g2 = game_new_empty_ext(4, 5, false, false);
  if (g1 == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (g2 == NULL) {
    fprintf(stderr, "Error :invalid game\n");
    return false;
  }
  if (!game_is_wrapping(g1)) {
    return false;
  }
  if (game_is_wrapping(g2)) {
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  return true;
};

// Test game_is_diagadj
bool test_game_is_diagadj() {
  game g1 = game_new_empty_ext(4, 5, true, true);
  game g2 = game_new_empty_ext(4, 5, false, false);
  if (g1 == NULL) {
    fprintf(stderr, "Error: invalid game\n");
    return false;
  }
  if (g2 == NULL) {
    fprintf(stderr, "Error :invalid game\n");
    return false;
  }
  if (!game_is_diagadj(g1)) {
    return false;
  }
  if (game_is_diagadj(g2)) {
    return false;
  }
  game_delete(g1);
  game_delete(g2);
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