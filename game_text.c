#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

int main(void) {
  game current_game = game_default();
  char c;
  uint i, j;
  while (!game_is_over(current_game)) {
    game_print(current_game);
    printf("Enter type of square, row number and column number:\n");
    int ret1 = scanf("%c", &c);
    if (c == 'h') {
      printf("Press r to restart the game and q to quit\n");
    } else if (c == 'r') {
      printf("You restarted the game\n");
      game_restart(current_game);
      game_print(current_game);
      printf("Enter type of square, row number and column number:\n");
    } else if (c == 'q') {
      printf("shame\n");
      return EXIT_SUCCESS;
    } else if (c == 'z') {
      game_undo(current_game);
      game_print(current_game);
    } else if (c == 'y') {
      game_redo(current_game);
      game_print(current_game);
    } else if (ret1 == EOF) {
      printf("End of file\n");
      exit(EXIT_SUCCESS);
    }
    int ret2 = scanf("%u %u", &i, &j);
    if (ret1 == 1 && ret2 == 2 && (c == 't' || c == 'g' || c == 'e') && i < 8 &&
        j < 8) {
      if (game_get_square(current_game, i, j) != TREE) {
        square s;
        if (c == 't') {
          s = TENT;
        }
        if (c == 'g') {
          s = GRASS;
        }
        if (c == 'e') {
          s = EMPTY;
        }
        printf("You put %c in the (%u, %u) position\n", c, i, j);
        game_play_move(current_game, i, j, s);
      }
    } else if (ret2 == EOF) {
      printf("End of file\n");
      exit(EXIT_SUCCESS);
    }
  }
  game_print(current_game);
  printf("congratulation\n");
  return EXIT_SUCCESS;
}
