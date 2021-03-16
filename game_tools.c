#include "game_tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

static uint game_solve_rec(game g, bool count_solution, uint *p_nb_sol);
static int game_fill(game g);
static int game_extra_check_move(cgame g, uint i, uint j, square s);
static uint nb_empty_cells_to_the_left(cgame g, uint i, uint j);
static uint nb_empty_cells_to_the_right(cgame g, uint i, uint j);
static uint nb_empty_cells_above(cgame g, uint i, uint j);
static uint nb_empty_cells_below(cgame g, uint i, uint j);
static uint size_of_section(cgame g, uint i, uint j, bool vertical);
static uint nb_possible_tent_placements_row(cgame g, uint i);
static uint nb_possible_tent_placements_col(cgame g, uint j);

game game_load(char *filename) {
  FILE *f;
  // Open the file and you can modifed the file because of the "r"
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
      // Determine the type of object in function of the character
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
  // Open a file you can only write
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

uint game_solve_rec(game g, bool count_solutions, uint *p_nb_sol_found) {
  if (game_is_over(g)) {
    return 1;
  }
  if (count_solutions == true) {
    game_fill(g);
  }
  uint nb_moves;
  bool stop = false;
  uint nb_sol_before = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (stop == true) {
      break;
    }
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (stop == true) {
        break;
      }
      if (game_get_square(g, i, j) == EMPTY) {
        game_play_move(g, i, j, TENT);
        nb_moves = game_fill(g);
        if (nb_moves == -1) {
          game_play_move(g, i, j, GRASS);
          continue;
        }
        nb_sol_before = game_solve_rec(g, count_solutions, p_nb_sol_found);
        if (game_is_over(g)) {
          *p_nb_sol_found += 1;
          if (!count_solutions) {
            return true;
          }
        }
        uint test = 0;
        while (game_get_square(g, i, j) != EMPTY) {
          test += 1;
          game_undo(g);
        }
        if (count_solutions) {
          if (game_extra_check_move(g, i, j, GRASS) == REGULAR) {
            game_play_move(g, i, j, GRASS);
          } else {
            stop = true;
          }
        }
      }
    }
  }
  return *p_nb_sol_found + nb_sol_before;
}

bool game_solve(game g) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  // All the moves correspond to fill all the game g
  uint nb_moves = game_fill(g);
  if (nb_moves == -1) {
    return false;
  }
  if (game_is_over(g)) {
    return true;
  }
  uint nb_solution_found = 0;
  uint nb_sols = game_solve_rec(g, false, &nb_solution_found);
  if (nb_sols == 0) {
    for (uint i = 0; i < nb_moves; i++) {
      game_undo(g);
    }
    return false;
  }
  return true;
}

uint game_nb_solutions(game g) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  uint nb_moves = game_fill(g);
  if (nb_moves == -1) {
    return 0;
  }
  if (game_is_over(g)) {
    return 1;
  }
  uint nb_solution_found = 0;
  uint game_is_solved = game_solve_rec(g, true, &nb_solution_found);

  if (game_is_solved == 0) {
    for (uint i = 0; i < nb_moves; i++) {
      game_undo(g);
    }
    return 0;
  }
  return nb_solution_found;
}

int game_extra_check_move(cgame g, uint i, uint j, square s) {
  // Name extra check moves
  if (game_check_move(g, i, j, s) == LOSING) {
    return LOSING;
  } else if (game_check_move(g, i, j, s) == ILLEGAL) {
    return ILLEGAL;
  }
  uint above_i = game_nb_rows(g);
  uint below_i = game_nb_rows(g);
  uint left_j = game_nb_cols(g);
  uint right_j = game_nb_cols(g);
  if (i != 0) {
    above_i = i - 1;
  } else if (i == 0 && game_is_wrapping(g)) {
    above_i = game_nb_rows(g) - 1;
  }
  if (i != game_nb_rows(g) - 1) {
    below_i = i + 1;
  } else if ((i == game_nb_rows(g) - 1) && game_is_wrapping(g)) {
    below_i = 0;
  }
  if (j != 0) {
    left_j = j - 1;
  } else if (j == 0 && game_is_wrapping(g)) {
    left_j = game_nb_rows(g) - 1;
  }
  if (j != game_nb_rows(g) - 1) {
    right_j = j + 1;
  } else if ((j == game_nb_rows(g) - 1) && game_is_wrapping(g)) {
    right_j = 0;
  }
  uint sec_size_hor = size_of_section(g, i, j, false);
  uint sec_size_vert = size_of_section(g, i, j, true);
  if (nb_possible_tent_placements_row(g, i) ==
      (game_get_expected_nb_tents_row(g, i) -
       game_get_current_nb_tents_row(g, i))) {
    if (sec_size_hor % 2 == 1 && sec_size_hor != game_nb_cols(g)) {
      if (nb_empty_cells_to_the_left(g, i, j) % 2 == 1) {
        if (s == TENT) {
          return LOSING;
        }
      } else {
        if (s == GRASS) {
          return LOSING;
        }
      }
    }
  } else if (nb_possible_tent_placements_col(g, j) ==
             (game_get_expected_nb_tents_col(g, j) -
              game_get_current_nb_tents_col(g, j))) {
    if (sec_size_vert % 2 == 1 && sec_size_vert != game_nb_rows(g)) {
      if (nb_empty_cells_above(g, i, j) % 2 == 1) {
        if (s == TENT) {
          return LOSING;
        }
      } else {
        if (s == GRASS) {
          return LOSING;
        }
      }
    }
  }
  if (!game_is_diagadj(g)) {
    if (above_i != game_nb_rows(g)) {
      if (nb_possible_tent_placements_row(g, above_i) ==
          (game_get_expected_nb_tents_row(g, above_i) -
           game_get_current_nb_tents_row(g, above_i))) {
        if (game_get_square(g, above_i, j) == EMPTY) {
          if (s == TENT) {
            return LOSING;
          }
        }
      }
    } else if (below_i != game_nb_rows(g)) {
      if (nb_possible_tent_placements_row(g, below_i) ==
          (game_get_expected_nb_tents_row(g, below_i) -
           game_get_current_nb_tents_row(g, below_i))) {
        if (game_get_square(g, below_i, j) == EMPTY) {
          if (s == TENT) {
            return LOSING;
          }
        }
      }
    } else if (left_j != game_nb_cols(g)) {
      if (nb_possible_tent_placements_col(g, left_j) ==
          (game_get_expected_nb_tents_col(g, left_j) -
           game_get_current_nb_tents_row(g, left_j))) {
        if (game_get_square(g, i, left_j) == EMPTY) {
          if (s == TENT) {
            return LOSING;
          }
        }
      }
    } else if (right_j != game_nb_cols(g)) {
      if (nb_possible_tent_placements_col(g, right_j) ==
          (game_get_expected_nb_tents_col(g, right_j) -
           game_get_current_nb_tents_row(g, right_j))) {
        if (game_get_square(g, i, right_j) == EMPTY) {
          if (s == TENT) {
            return LOSING;
          }
        }
      }
    }
  }
  return REGULAR;
}

int game_fill(game g) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  int nb_moves = 1;
  int total_nb_moves = 0;
  while (nb_moves != 0) {
    nb_moves = 0;
    for (uint i = 0; i < game_nb_rows(g); i++) {
      for (uint j = 0; j < game_nb_cols(g); j++) {
        if (game_get_square(g, i, j) == EMPTY) {
          int tent_move = game_extra_check_move(g, i, j, TENT);
          int grass_move = game_extra_check_move(g, i, j, GRASS);
          if (tent_move == LOSING && grass_move == REGULAR) {
            game_play_move(g, i, j, GRASS);
            nb_moves++;
            total_nb_moves++;
          } else if (grass_move == LOSING && tent_move == REGULAR) {
            game_play_move(g, i, j, TENT);
            nb_moves++;
            total_nb_moves++;
          } else if (tent_move == LOSING && grass_move == LOSING) {
            for (int nb = 0; nb < total_nb_moves + 1; nb++) {
              game_undo(g);
            }
            return -1;
          }
        }
      }
    }
  }
  return total_nb_moves;
}

uint size_of_section(cgame g, uint i, uint j, bool vertical) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (vertical) {
    if ((i == 0 && (nb_empty_cells_above(g, i, j) == game_nb_rows(g) - 1)) ||
        (i == game_nb_rows(g) - 1 &&
         nb_empty_cells_below(g, i, j) == game_nb_rows(g) - 1)) {
      return game_nb_rows(g);
    }
    return 1 + nb_empty_cells_above(g, i, j) + nb_empty_cells_below(g, i, j);
  }
  if ((j == 0 && nb_empty_cells_to_the_left(g, i, j) == game_nb_cols(g) - 1) ||
      (j == game_nb_cols(g) - 1 &&
       nb_empty_cells_to_the_right(g, i, j) == game_nb_cols(g) - 1)) {
    return game_nb_cols(g);
  }
  return 1 + nb_empty_cells_to_the_right(g, i, j) +
         nb_empty_cells_to_the_left(g, i, j);
}

uint nb_empty_cells_above(cgame g, uint i, uint j) {
  uint nb = 0;
  uint current_i = i;
  uint cpt = 0;
  while (current_i != game_nb_rows(g) &&
         game_get_square(g, current_i, j) == EMPTY && cpt != game_nb_rows(g)) {
    nb++;
    if (current_i != 0) {
      current_i = current_i - 1;
    } else if (game_is_wrapping(g)) {
      current_i = game_nb_rows(g) - 1;
    } else {
      current_i = game_nb_rows(g);
    }
    cpt++;
  }
  if (game_get_square(g, i, j) == EMPTY) {
    return nb - 1;
  }
  return nb;
}

uint nb_empty_cells_below(cgame g, uint i, uint j) {
  uint nb = 0;
  uint current_i = i;
  uint cpt = 0;
  while (current_i != game_nb_rows(g) &&
         game_get_square(g, current_i, j) == EMPTY && cpt != game_nb_rows(g)) {
    nb++;
    if (current_i != game_nb_rows(g) - 1) {
      current_i = current_i + 1;
    } else if (game_is_wrapping(g)) {
      current_i = 0;
    } else {
      current_i = game_nb_rows(g);
    }
    cpt++;
  }
  if (game_get_square(g, i, j) == EMPTY) {
    return nb - 1;
  }
  return nb;
}

uint nb_empty_cells_to_the_right(cgame g, uint i, uint j) {
  uint cpt = 0;
  uint nb = 0;
  uint current_j = j;
  while (current_j != game_nb_cols(g) &&
         game_get_square(g, i, current_j) == EMPTY && cpt != game_nb_cols(g)) {
    nb++;
    if (current_j != game_nb_cols(g) - 1) {
      current_j = current_j + 1;
    } else if (game_is_wrapping(g)) {
      current_j = 0;
    } else {
      current_j = game_nb_cols(g);
    }
  }
  if (game_get_square(g, i, j) == EMPTY) {
    return nb - 1;
  }
  return nb;
}
uint nb_empty_cells_to_the_left(cgame g, uint i, uint j) {
  uint cpt = 0;
  uint nb = 0;
  uint current_j = j;
  while (current_j != game_nb_cols(g) &&
         game_get_square(g, i, current_j) == EMPTY && cpt != game_nb_cols(g)) {
    nb++;
    if (current_j != 0) {
      current_j = current_j - 1;
    } else if (game_is_wrapping(g)) {
      current_j = game_nb_cols(g) - 1;
    } else {
      current_j = game_nb_cols(g);
    }
    cpt++;
  }
  if (game_get_square(g, i, j) == EMPTY) {
    return nb - 1;
  }
  return nb;
}

uint nb_possible_tent_placements_row(cgame g, uint i) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  uint nb = 0;
  bool is_prev_tent = false;
  for (uint j = 0; j < game_nb_cols(g); j++) {
    if (game_get_square(g, i, j) == EMPTY && !is_prev_tent) {
      nb++;
      is_prev_tent = true;
    } else {
      is_prev_tent = false;
    }
  }
  return nb;
}

uint nb_possible_tent_placements_col(cgame g, uint j) {
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  uint nb = 0;
  bool is_prev_tent = false;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_square(g, i, j) == EMPTY && !is_prev_tent) {
      nb++;
      is_prev_tent = true;
    } else {
      is_prev_tent = false;
    }
  }
  return nb;
}
