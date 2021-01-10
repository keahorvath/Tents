#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

/*test_game_set_square*/
bool test_game_set_square() {
  game test_game = game_default();
  unsigned int i = 0;
  unsigned int j = 0;
  while ((i < 8) && (j < 8)) {
    game_set_square(test_game, i, j, EMPTY);
    if (game_get_square(test_game, i, j) != EMPTY) {
      return false;
    }
    game_set_square(test_game, i, j, TREE);
    if (game_get_square(test_game, i, j) != TREE) {
      return false;
    }
    game_set_square(test_game, i, j, TENT);
    if (game_get_square(test_game, i, j) != TENT) {
      return false;
    }
    game_set_square(test_game, i, j, GRASS);
    if (game_get_square(test_game, i, j) != GRASS) {
      return false;
    }
    i += 1;
    j += 1;
  }
  game_delete(test_game);
  return true;
}

/*test_game_get_square*/
bool test_game_get_square() {
  game test_game = game_default();
  if ((game_get_square(test_game, 0, 4) != TREE) ||
      (game_get_square(test_game, 0, 0) != EMPTY)) {
    return false;
  }
  game_set_square(test_game, 0, 0, TENT);
  if (game_get_square(test_game, 0, 0) != TENT) {
    return false;
  }
  game_set_square(test_game, 0, 1, GRASS);
  if (game_get_square(test_game, 0, 1) != GRASS) {
    return false;
  }
  game_restart(test_game);  // pour les points :) //
  game_play_move(test_game, 0, 3, TENT);
  if (game_get_square(test_game, 0, 3) != TENT) {
    return false;
  }
  game_delete(test_game);
  return true;
}

/*test_game_equal*/

bool test_game_equal() {
  game g1 = game_default();
  game g2 = game_default();
  bool test = game_equal(g1, g2);
  if (!test) {
    return false;
  }
  game_set_expected_nb_tents_row(g2, 2, 3);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_set_expected_nb_tents_row(g2, 2, 4);
  game_set_expected_nb_tents_col(g2, 2, 3);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_set_expected_nb_tents_col(g2, 2, 1);
  game_set_square(g2, 0, 0, TREE);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_set_square(g2, 0, 0, EMPTY);

  game_set_square(g2, 7, 7, TREE);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_set_square(g2, 7, 7, EMPTY);

  game_set_square(g2, 1, 0, EMPTY);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_set_square(g2, 4, 0, GRASS);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_set_square(g2, 4, 0, EMPTY);

  game_set_square(g2, 4, 0, TENT);
  test = game_equal(g1, g2);
  if (test) {
    return false;
  }
  game_play_move(g1, 4, 4, GRASS);

  // test quand wrapping et diagadj différents
  square squares[] = {TREE, TENT, 0, TENT, TREE, 0, 0, 0, 0};
  uint nb_tents_row[] = {1, 1, 0};
  uint nb_tents_col[] = {1, 1, 0};
  game g3 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, false, true);
  game g4 = game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, true, true);
  game g5 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, false, false);
  game g6 =
      game_new_ext(3, 3, squares, nb_tents_row, nb_tents_col, true, false);
  if (game_equal(g3, g4) || game_equal(g5, g6)) {
    return false;
  }
  // test si nb_rows différents
  square squares2[] = {TREE, TENT, 0, TENT, TREE, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row2[] = {1, 1, 0, 0};
  game g7 =
      game_new_ext(4, 3, squares2, nb_tents_row2, nb_tents_col, false, true);
  if (game_equal(g3, g7)) {
    return false;
  }
  game_delete(g1);
  game_delete(g2);
  game_delete(g3);
  game_delete(g4);
  game_delete(g5);
  game_delete(g6);
  game_delete(g7);
  return true;
}

/*test_game_delete*/

bool test_game_delete() {
  game game_test = game_default();
  game_delete(game_test);
  return true;
}

/*test_game_set_expected_nb_tents_row*/

bool test_game_set_expected_nb_tents_row() {
  game game_test = game_default();
  game_set_expected_nb_tents_row(game_test, 0, 1);
  game_set_expected_nb_tents_row(game_test, 3, 2);
  game_set_expected_nb_tents_row(game_test, 7, 3);
  if ((game_get_expected_nb_tents_row(game_test, 0) != 1) ||
      (game_get_expected_nb_tents_row(game_test, 3) != 2) ||
      (game_get_expected_nb_tents_row(game_test, 7) != 3)) {
    return false;
  }
  game_delete(game_test);
  return true;
}

/*test_game_set_expected_nb_tents_col*/

bool test_game_set_expected_nb_tents_col() {
  game game_test = game_default();
  game_set_expected_nb_tents_col(game_test, 0, 1);
  game_set_expected_nb_tents_col(game_test, 3, 2);
  game_set_expected_nb_tents_col(game_test, 7, 3);
  if ((game_get_expected_nb_tents_col(game_test, 0) != 1) ||
      (game_get_expected_nb_tents_col(game_test, 3) != 2) ||
      (game_get_expected_nb_tents_col(game_test, 7) != 3)) {
    return false;
  }
  game_delete(game_test);
  return true;
}

/**test_game_undo **/
bool test_game_undo(void) {
  game game_test = game_default();
  game game_test2 = game_default();
  game_undo(game_test);
  bool test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }
  game_play_move(game_test2, 0, 0, GRASS);
  game_undo(game_test2);
  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }
  game_play_move(game_test2, 0, 1, TENT);
  game_play_move(game_test, 0, 1, TENT);

  game_play_move(game_test2, 0, 1, GRASS);
  game_undo(game_test2);
  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }
  game_delete(game_test);
  game_delete(game_test2);
  return true;
}

/**test_game_undo **/
bool test_game_redo(void) {
  game game_test = game_default();
  game game_test2 = game_default();
  game_redo(game_test);
  bool test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }

  game_play_move(game_test2, 0, 1, TENT);

  game_play_move(game_test2, 0, 2, GRASS);

  game_play_move(game_test2, 0, 3, GRASS);

  game_undo(game_test2);
  game_undo(game_test2);
  game_undo(game_test2);

  game_redo(game_test2);
  game_play_move(game_test, 0, 1, TENT);
  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }

  game_redo(game_test2);
  game_play_move(game_test, 0, 2, GRASS);
  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }

  game_redo(game_test2);
  game_play_move(game_test, 0, 3, GRASS);
  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }
  game_redo(game_test2);
  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }

  game_undo(game_test2);
  game_undo(game_test2);
  game_undo(game_test2);

  game_delete(game_test);
  game_test = game_default();

  game_redo(game_test2);
  game_play_move(game_test, 0, 1, TENT);

  game_play_move(game_test2, 2, 0, TENT);
  game_play_move(game_test, 2, 0, TENT);
  game_redo(game_test2);

  test = game_equal(game_test, game_test2);
  if (!test) {
    return false;
  }

  game_delete(game_test);
  game_delete(game_test2);
  return true;
}

int main(int argc, char *argv[]) {
  // start test
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool testPassed = false;

  if (strcmp("game_set_square", argv[1]) == 0) {
    testPassed = test_game_set_square();
  } else if (strcmp("game_get_square", argv[1]) == 0) {
    testPassed = test_game_get_square();
  } else if (strcmp("game_equal", argv[1]) == 0) {
    testPassed = test_game_equal();
  } else if (strcmp("game_delete", argv[1]) == 0) {
    testPassed = test_game_delete();
  } else if (strcmp("game_set_expected_nb_tents_row", argv[1]) == 0) {
    testPassed = test_game_set_expected_nb_tents_row();
  } else if (strcmp("game_set_expected_nb_tents_col", argv[1]) == 0) {
    testPassed = test_game_set_expected_nb_tents_col();
  } else if (strcmp("game_undo", argv[1]) == 0) {
    testPassed = test_game_undo();
  } else if (strcmp("game_redo", argv[1]) == 0) {
    testPassed = test_game_redo();
  } else {
    fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  // print test result
  if (testPassed) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}
