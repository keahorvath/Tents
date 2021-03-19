#include "game_tools.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "extra_functions.h"
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
static uint *make_array_of_all_trees(cgame g);
static uint game_nb_trees(cgame g);
static uint fill_according_to_trees(game g);

game game_load(char *filename) {
  FILE *f;
  // Open the file
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
      // Determine the type of object(according to the character)
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
      fclose(f);
      return NULL;
    }
    fclose(f);
    return g;
  }
  fclose(f);
  return NULL;
}

void game_save(cgame g, char *filename) {
  // Open a file
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

/**
 * @brief The recursive function that goes with function game_solve and
 *game_nb_solutioos
 * @details This function checks that playing a move in a square is a regular
 * move (see @ref index).
 * @param g the game
 * @param count_solutions true if called by game_nb_solutions, false if called
 *by game_solve
 * @param p_nb_sol_found pointer to the number of solutions found
 * @return the number of solutions found
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint game_solve_rec(game g, bool count_solutions, uint *p_nb_sol_found) {
  if (game_is_over(g)) {
    return 1;
  }
  if (count_solutions == true) {
    game_fill(g);
  }
  printf("game after first fill\n");
  game_print(g);
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
        game_print(g);
        printf("play tent in %u %u \n", i, j);
        game_play_move(g, i, j, TENT);
        nb_moves = game_fill(g);
        game_print(g);
        if (nb_moves == -1) {
          printf("replacing with grass in %u %u \n", i, j);

          game_play_move(g, i, j, GRASS);
          continue;
        }
        nb_sol_before = game_solve_rec(g, count_solutions, p_nb_sol_found);
        if (game_is_over(g)) {
          printf("SOLUTION\n");
          game_print(g);
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
  test_pointer(g);
  uint nb_moves = game_fill(g);
  printf("first fill:\n");
  game_print(g);
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
  test_pointer(g);
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

/**
 * @brief Checks in a more detailed manner if a given move in a square is
 *regular
 * @details This function checks that playing a move in a square is a regular
 * move (see @ref index).
 * @param g the game
 * @param i row index
 * @param j column index
 * @param s the square value
 * @return either REGULAR, LOSING or ILLEGAL depending on the move
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @pre @p s must be either EMPTY, GRASS, TENT or TREE.
 **/
int game_extra_check_move(cgame g, uint i, uint j, square s) {
  if (game_check_move(g, i, j, s) == LOSING) {
    return LOSING;
  } else if (game_check_move(g, i, j, s) == ILLEGAL) {
    return ILLEGAL;
  }
  // First we find the coordinates of the cells around (depends on wrapping)
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
  /* If in the given row (or column) in which the cell is,
  there is the same number of possible placements than the number of tents we
  have to place, then, if the size of the section in which the cell is is odd,
  we can make some deductions:
  - if the cell is in an "odd" position, then it has to contain a tent
  - if the cell is in an "even" position, then it has to contain grass
  for example:
  x  --   x  -- x: 5
  There are 5 possible tent placements and we have to place 5 tents
  So we can make 4 deductions:
  - the first section in even, so we can't deduct anything
  - the second one is odd, so we know that the cells in position 1 and 3 of that
  section have to be tents, and the cell in position 2 has to be grass
  - the third section is even so we can't deduct anything
  - the fourth section is odd so we can place a tent
  The same goes for columns
  */
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
  /*
  If the game isn't diagadj, we can make some more deductions
  We can look at the row above (or below) or the column on the left (or right)

  For example:
  0:x  -    x*x:4
  1:  x- x -x--:1
  We can see that row 0 still has 3 tents to place and there are only 3 possible
  tent placements (since in a section of 4, there can be a maximum of 2 tents
  and in a section of 2 there can only be 1 tent) We can therefore make some
  deductions:
  - in the first section of row 0, there has to be a tent, so either way, the
  cell in row 1 col 1 cannot be a tent (a tent will always see it)
  - in the second section (size 4) of row 0, there has to be 2 tents
  no matter how you place them, they will always see row 1 col 4 and row 1 col 6
  so these cells have to be grass
  */
  if (s == TENT && !game_is_diagadj(g)) {
    if (above_i != game_nb_rows(g)) {
      if (nb_possible_tent_placements_row(g, above_i) ==
          (game_get_expected_nb_tents_row(g, above_i) -
           game_get_current_nb_tents_row(g, above_i))) {
        if (game_get_square(g, above_i, j) == EMPTY) {
          return LOSING;
        }
      }
    }
    if (below_i != game_nb_rows(g)) {
      if (nb_possible_tent_placements_row(g, below_i) ==
          (game_get_expected_nb_tents_row(g, below_i) -
           game_get_current_nb_tents_row(g, below_i))) {
        if (game_get_square(g, below_i, j) == EMPTY) {
          return LOSING;
        }
      }
    }
    if (left_j != game_nb_cols(g)) {
      if (nb_possible_tent_placements_col(g, left_j) ==
          (game_get_expected_nb_tents_col(g, left_j) -
           game_get_current_nb_tents_col(g, left_j))) {
        if (game_get_square(g, i, left_j) == EMPTY) {
          return LOSING;
        }
      }
    }
    if (right_j != game_nb_cols(g)) {
      if (nb_possible_tent_placements_col(g, right_j) ==
          (game_get_expected_nb_tents_col(g, right_j) -
           game_get_current_nb_tents_col(g, right_j))) {
        if (game_get_square(g, i, right_j) == EMPTY) {
          return LOSING;
        }
      }
    }
  }
  // check number 3
  if (s == TENT && !game_is_diagadj(g)) {
    if (above_i != game_nb_rows(g) && left_j != game_nb_cols(g) && right_j != game_nb_cols(g)) {
      if (nb_possible_tent_placements_row(g, above_i) ==
          (game_get_expected_nb_tents_row(g, above_i) -
           game_get_current_nb_tents_row(g, above_i) + 1)) {
        if (game_get_square(g, above_i, left_j) == EMPTY && size_of_section(g, above_i, left_j, false) == 1 && game_get_square(g, above_i, right_j) == EMPTY && size_of_section(g, above_i, right_j, false) == 1){
          printf("above\n");
          return LOSING;
        }
      }
    }
    if (below_i != game_nb_rows(g) && left_j != game_nb_cols(g) && right_j != game_nb_cols(g)) {
      if (nb_possible_tent_placements_row(g, below_i) ==
          (game_get_expected_nb_tents_row(g, below_i) -
           game_get_current_nb_tents_row(g, below_i)+1)) {
        if (game_get_square(g, below_i, left_j) == EMPTY && size_of_section(g, below_i, left_j, false) == 1 && game_get_square(g, below_i, right_j) == EMPTY && size_of_section(g, below_i, right_j, false) == 1){
          printf("below\n");
          return LOSING;
        }
      }
    }
    if (left_j != game_nb_cols(g) && above_i != game_nb_rows(g) && below_i != game_nb_rows(g)) {
      if (nb_possible_tent_placements_col(g, left_j) ==
          (game_get_expected_nb_tents_col(g, left_j) -
           game_get_current_nb_tents_col(g, left_j)+1)) {
        if (game_get_square(g, above_i, left_j) == EMPTY && size_of_section(g, above_i, left_j, true) == 1 && game_get_square(g, below_i, left_j) == EMPTY && size_of_section(g, below_i, left_j, true) == 1){
          printf("left\n");
          return LOSING;
        }
      }
    }
    if (right_j != game_nb_cols(g) && above_i != game_nb_rows(g) && below_i != game_nb_rows(g)) {
      if (nb_possible_tent_placements_col(g, right_j) ==
          (game_get_expected_nb_tents_col(g, right_j) -
           game_get_current_nb_tents_col(g, right_j)+1)) {
        if (game_get_square(g, above_i, right_j) == EMPTY && size_of_section(g, above_i, right_j, true) == 1 && game_get_square(g, below_i, right_j) == EMPTY && size_of_section(g, below_i, right_j, true) == 1){
          printf("right\n");
          return LOSING;
        }
      }
    }
  }

  return REGULAR;
}

/**
 * @brief Fills the game to the maximum
 * @details This function checks each cell: if grass is losing, it places a
 *tent, if tent is losing, it places grass
 * @param g the game
 * @return the total number of moves that have been made
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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
            printf("placing grass in %u %u\n", i, j);
            game_play_move(g, i, j, GRASS);
            nb_moves++;
            total_nb_moves++;
          } else if (grass_move == LOSING && tent_move == REGULAR) {
            printf("placing tent in %u %u\n", i, j);
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
    if (!game_is_over(g)) {
      uint cpt = fill_according_to_trees(g);
      nb_moves += cpt;
      total_nb_moves += cpt;
    }
  }
  printf("end_fill:\n");
  game_print(g);
  return total_nb_moves;
}

uint *make_array_of_all_trees(cgame g) {
  test_pointer(g);
  uint *array = (uint *)malloc(sizeof(uint) * game_nb_trees(g) * 2);
  if (array == NULL) {
    fprintf(stderr, "Not enough memory\n");
    exit(EXIT_FAILURE);
  }
  uint cpt = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TREE) {
        array[cpt] = i;
        array[cpt + 1] = j;
        cpt += 2;
      }
    }
  }
  return array;
}

uint game_nb_trees(cgame g) {
  test_pointer(g);
  uint cpt = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TREE) {
        cpt++;
      }
    }
  }
  return cpt;
}

uint nb_trees_around_cell(cgame g, uint i, uint j) {
  test_pointer(g);
  test_i_value(g, i);
  test_j_value(g, j);
  uint *cells_around = make_array_of_ortho_adjacent_cells(g, i, j);
  uint cell_i, cell_j;
  uint ind = 1;
  uint nb_trees = 0;
  while (cells_around[ind - 1] < game_nb_rows(g)) {
    cell_i = cells_around[ind - 1];
    cell_j = cells_around[ind];
    if (game_get_square(g, cell_i, cell_j) == TREE) {
      nb_trees++;
    }
    ind += 2;
  }
  free(cells_around);
  return nb_trees;
}

uint fill_according_to_trees(game g) {
  uint nb_moves = 0;
  uint cpt = 1;  // counts how many trees were added in that round
  uint *trees = make_array_of_all_trees(g);
  uint *taken_trees = (uint *)malloc(sizeof(uint) * game_nb_trees(g) * 2);
  if (taken_trees == NULL) {
    fprintf(stderr, "Not enough memory\n");
    exit(EXIT_FAILURE);
  }
  uint *taken_tents = (uint *)malloc(sizeof(uint) * game_nb_trees(g) * 2);
  if (taken_tents == NULL) {
    fprintf(stderr, "Not enough memory\n");
    exit(EXIT_FAILURE);
  }
  uint nb_taken = 0;
  while (cpt != 0) {
    cpt = 0;
    bool already_has_a_tent = false;
    for (uint i = 0; i < game_nb_trees(g) * 2; i += 2) {
      if (i % 2 == 1) {
        continue;
      }
      // first check if the tree is already taken
      already_has_a_tent = false;
      // printf("taken trees:\n");
      for (uint nb = 0; nb < nb_taken * 2; nb += 2) {
        // printf("%u %u\n", taken_trees[nb], taken_trees[nb+1]);
        if (trees[i] == taken_trees[nb] &&
            trees[i + 1] == taken_trees[nb + 1]) {
          already_has_a_tent = true;
          break;
        }
      }
      if (already_has_a_tent) {
        continue;
      }
      uint *adj_cells_tree =
          make_array_of_ortho_adjacent_cells(g, trees[i], trees[i + 1]);
      uint ind = 1;
      uint cell_i, cell_j;
      uint nb_tent_placements = 0;
      uint possible_placement_i, possible_placement_j;
      uint nb_tents_avail = 0;
      uint nb_tents_not_avail = 0;
      while (adj_cells_tree[ind - 1] < game_nb_rows(g)) {
        cell_i = adj_cells_tree[ind - 1];
        cell_j = adj_cells_tree[ind];
        if (game_get_square(g, cell_i, cell_j) == TENT) {
          bool is_taken = false;
          for (uint nb = 0; nb < nb_taken * 2; nb += 2) {
            if (cell_i == taken_tents[nb] && cell_j == taken_tents[nb + 1]) {
              is_taken = true;
              nb_tents_not_avail++;
              break;
            }
          }
          if (!is_taken) {
            nb_tent_placements++;
            nb_tents_avail++;
            possible_placement_i = cell_i;
            possible_placement_j = cell_j;
          }
        } else if (game_get_square(g, cell_i, cell_j) == EMPTY) {
          nb_tent_placements++;
          possible_placement_i = cell_i;
          possible_placement_j = cell_j;
        }
        ind += 2;
      }
      if (nb_tent_placements == 1) {
        if (game_get_square(g, possible_placement_i, possible_placement_j) ==
            EMPTY) {
          nb_moves++;
          printf("playing tent: %u %u\n", possible_placement_i, possible_placement_j);
          game_play_move(g, possible_placement_i, possible_placement_j, TENT);
        }
        taken_tents[nb_taken * 2] = possible_placement_i;
        taken_tents[nb_taken * 2 + 1] = possible_placement_j;
        taken_trees[nb_taken * 2] = trees[i];
        taken_trees[nb_taken * 2 + 1] = trees[i + 1];
        nb_taken++;
        cpt++;
      } else if (nb_tents_avail == 1 && nb_tents_not_avail == 0) {
        ind = 1;
        while (adj_cells_tree[ind - 1] < game_nb_rows(g)) {
          cell_i = adj_cells_tree[ind - 1];
          cell_j = adj_cells_tree[ind];
          if (game_get_square(g, cell_i, cell_j) == TENT &&
              nb_trees_around_cell(g, cell_i, cell_j) == 1) {
            uint ind2 = 1;
            taken_tents[nb_taken * 2] = cell_i;
            taken_tents[nb_taken * 2 + 1] = cell_j;
            taken_trees[nb_taken * 2] = trees[i];
            taken_trees[nb_taken * 2 + 1] = trees[i + 1];
            nb_taken++;
            cpt++;
            while (adj_cells_tree[ind2 - 1] < game_nb_rows(g)) {
              uint cell_i2 = adj_cells_tree[ind2 - 1];
              uint cell_j2 = adj_cells_tree[ind2];
              if (game_get_square(g, cell_i2, cell_j2) == EMPTY &&
                  nb_trees_around_cell(g, cell_i2, cell_j2) == 1) {
                nb_moves++;
                // printf("playing grass in %u %u \n", cell_i2, cell_j2);
                game_play_move(g, cell_i2, cell_j2, GRASS);
              }
              ind2 += 2;
            }
            break;
          }
          ind += 2;
        }
      }
      free(adj_cells_tree);
    }
  }
  free(taken_tents);
  free(taken_trees);
  free(trees);
  return nb_moves;
}
/**
 * @brief Gives the size of the section that the cell is in (in a given
 *direction)
 * @details Counts the number of cells that are empty around the cell
 * @param g the game
 * @param i row index
 * @param j column index
 * @param vertical the direction of the section
 * @return the total number of moves that have been made
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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

/**
 * @brief Gives the number of empty cells above the given cell
 * @param g the game
 * @param i row index
 * @param j column index
 * @return the number of empty cells above
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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

/**
 * @brief Gives the number of empty cells below the given cell
 * @param g the game
 * @param i row index
 * @param j column index
 * @return the number of empty cells below
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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

/**
 * @brief Gives the number of empty cells to the left of the given cell
 * @param g the game
 * @param i row index
 * @param j column index
 * @return the number of empty cells to the left
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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

/**
 * @brief Gives the number of empty cells to the right of the given cell
 * @param g the game
 * @param i row index
 * @param j column index
 * @return the number of empty cells to the right
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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

/**
 * @brief Gives the number of possible tent placements in the row
 * @param g the game
 * @param i row index
 * @return the number of possible tent placements in the row
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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

/**
 * @brief Gives the number of possible tent placements in the column
 * @param g the game
 * @param j column index
 * @return the number of possible tent placements in the column
 * @pre @p g must be a valid pointer toward a game structure.
 **/
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
