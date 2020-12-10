#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"

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
    printf("test 1\n");
    return false;
  }

  // test 2 if placing new tree is illegal
  if (game_check_move(g, 1, 1, TREE) != ILLEGAL) {
    printf("test 2\n");

    return false;
  }

  // test 3/4 if placing n+1 tents in row or column is losing
  printf("check : %d\n", game_check_move(g, 1, 1, TENT));
  if (game_check_move(g, 1, 1, TENT) != LOSING) {
    printf("test 3/4\n");
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
    printf("test 5\n");

    return false;
  }

  // test 6 if placing tent in row and column with no tents required is losing
  if (game_check_move(g, 7, 1, TENT) != LOSING ||
      game_check_move(g, 3, 7, TENT) != LOSING) {
    printf("test 6\n");

    return false;
  }

  // test 7 if placing grass and not enough empty squares for tents is losing
  if (game_check_move(g, 0, 0, GRASS) != LOSING) {
    printf("test 7\n");

    return false;
  }

  // test 8 if surronding tree by grass is losing
  game g1 = game_default();
  game_play_move(g1, 5, 3, GRASS);
  game_play_move(g1, 6, 4, GRASS);
  game_play_move(g1, 5, 5, GRASS);
  if (game_check_move(g1, 4, 4, GRASS) != LOSING) {
    printf("test 8\n");

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
    printf("test 9\n");

    return false;
  }
  if (game_check_move(g, 0, 2, TENT) != LOSING) {
    return false;
  }

  game_delete(g);
  game_delete(g1);
  game_delete(g2);
  return true;
}

bool test_game_is_over(void) {
  game g = game_default_solution();
  // test 1 if the correct default solution returns an error
  if (!game_is_over(g)) {
    printf("over test 1\n");

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
  game_play_move(g2, 2, 0, TENT);
  game_play_move(g2, 1, 5, TENT);
  if (game_is_over(g2)) {
    printf("over test 2\n");
    return false;
  }

  // test 3 if tents are adjacent
  square squares3[] = {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint nb_tents_row3[] = {2, 0, 1, 0, 0, 0, 0, 0};
  uint nb_tents_col3[] = {0, 0, 0, 1, 1, 0, 1, 0};
  game g4 = game_new(squares3, nb_tents_row3, nb_tents_col3);
  game_play_move(g4, 0, 3, TENT);
  game_play_move(g4, 0, 4, TENT);
  game_play_move(g4, 2, 6, TENT);
  if (game_is_over(g4)) {
    printf("over test 3\n");
    return false;
  }

  // test 4 if not having the right nb of tents in row/column still returns true
  game_play_move(g, 0, 0, GRASS);
  if (game_is_over(g)) {
    printf("over test 4\n");
    return false;
  }

  game_set_square(g, 7, 0, TREE);

  // test 5 if placing tent not next to tree still returns true
  game_play_move(g, 6, 0, GRASS);
  game_play_move(g, 6, 5, TENT);
  if (game_is_over(g)) {
    printf("over test 5\n");
    return false;
  }
  game_delete(g);
  game_delete(g2);
  game_delete(g4);
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
  game_delete(g);
  game_delete(g2);
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
  game_delete(g);
  game_delete(g2);
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
  game_delete(g1);
  game_delete(g2);
  return true;
}

int main(int argc, char *argv[]) {
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