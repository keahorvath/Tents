#include "game.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

// Declaration of the functions that aren't given in the .h files
static uint *make_array_of_all_adjacent_cells(cgame g, uint i, uint j);
static uint *make_array_of_ortho_adjacent_cells(cgame g, uint i, uint j);
static void test_pointer(cgame g);
static void test_i_value(cgame g, uint i);
static void test_j_value(cgame g, uint j);

/**
 * @brief The structure that stores the game state.
 **/
struct game_s {
  uint nb_rows;
  uint nb_cols;
  square *squares;
  uint *nb_tents_row;
  uint *nb_tents_col;
  bool wrapping;
  bool diagadj;
  queue *undo_hist;
  queue *redo_hist;
};

/**
 * @brief The structure pointer that stores the game state.
 **/
typedef struct game_s *game;

/**
 * @brief The structure constant pointer that stores the game state.
 * @details That means that it is not possible to modify the game using this
 * pointer.
 **/
typedef const struct game_s *cgame;

struct move {
  square s;
  uint i;
  uint j;
};
/**
 * @brief The structure that stores the move.
 **/
typedef struct move move;

/**
 * @brief stores a move in a structure allocated dynamically.
 * @param squares an array describing the initial square values (row-major
 * storage)
 * @param i the number of row
 * @param j the number of column
 * @pre @p squares must be an initialized array of default size squared.
 * @pre @p i must be an unsigned integer.
 * @pre @p j must be an unsigned integer.
 * @return the created Move.
 **/
move *create_move(square s, uint i, uint j) {
  move *Move = (move *)malloc(sizeof(move));
  if (Move == NULL) exit(EXIT_FAILURE);
  Move->s = s;
  Move->i = i;
  Move->j = j;
  return Move;
}

/**
 * @brief free the move allocated dynamically.
 * @param queue a pointer to a queue
 * @return nothing.
 **/
void free_Moves(queue *queue) {
  if (queue != NULL) {
    while (!queue_is_empty(queue)) {
      move *move = queue_pop_tail(queue);
      free(move);
      move = NULL;
    }
    queue_clear(queue);
  }
}

/**
 * @brief Creates a new game with default size and initializes it.
 * @param squares an array describing the initial square values (row-major
 * storage)
 * @param nb_tents_row an array with the expected number of tents in each row
 * @param nb_tents_col an array with the expected number of tents in each column
 * @pre @p squares must be an initialized array of default size squared.
 * @pre @p nb_tents_row must be an initialized array of default size.
 * @pre @p nb_tents_col must be an initialized array of default size.
 * @return the created game
 **/
game game_new(square *squares, uint *nb_tents_row, uint *nb_tents_col) {
  // if the space of the game wasn't make then the game call an error//
  if (squares == NULL || nb_tents_row == NULL || nb_tents_col == NULL) {
    fprintf(stderr, "Function called on NULL pointer!\n");
    exit(EXIT_FAILURE);
  }
  game g = game_new_empty();
  // put the square in the game//
  for (uint i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
    g->squares[i] = squares[i];
  }
  // put the corresponding objects to all the square//
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    g->nb_tents_col[i] = nb_tents_col[i];
    g->nb_tents_row[i] = nb_tents_row[i];
  }
  return g;
}

/**
 * @brief Creates a new empty game with defaut size.
 * @details All squares are initialized with empty squares, and the expected
 * numbers of tents for each row/column are set to zero.
 * @return the created game
 **/
game game_new_empty(void) {
  // create a new memory space//
  game g = (game)malloc(sizeof(struct game_s));
  // if the memory space wasn't create then call an error//
  if (g == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  // create a new memory space for each square//
  g->squares = (square *)malloc(sizeof(square) * DEFAULT_SIZE * DEFAULT_SIZE);
  if (g->squares == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  // create a new memory space for each number of tents in columns//
  g->nb_tents_col = (uint *)malloc(sizeof(uint) * DEFAULT_SIZE);
  if (g->nb_tents_col == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  // create a new memory space for each number of rows//
  g->nb_tents_row = (uint *)malloc(sizeof(uint) * DEFAULT_SIZE);
  if (g->nb_tents_row == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->nb_rows = DEFAULT_SIZE;
  g->nb_cols = DEFAULT_SIZE;
  g->wrapping = false;
  g->diagadj = false;
  g->undo_hist = queue_new();
  g->redo_hist = queue_new();
  // pose all the expected tents in the game for each column and each row//
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    game_set_expected_nb_tents_row(g, i, 0);
    game_set_expected_nb_tents_col(g, i, 0);
    for (int j = 0; j < DEFAULT_SIZE; j++) {
      game_set_square(g, i, j, EMPTY);
    }
  }
  return g;
}

/**
 * @brief Duplicates a game.
 * @param g the game to copy
 * @return the copy of the game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
game game_copy(cgame g) {
  // if the game doesn't exist then call an error//
  test_pointer(g);
  game g_copy = game_new_empty_ext(game_nb_rows(g), game_nb_cols(g),
                                   game_is_wrapping(g), game_is_diagadj(g));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    uint nb_r = game_get_expected_nb_tents_row(g, i);
    game_set_expected_nb_tents_row(g_copy, i, nb_r);
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    uint nb_c = game_get_expected_nb_tents_col(g, j);
    game_set_expected_nb_tents_col(g_copy, j, nb_c);
  }
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      game_set_square(g_copy, i, j, s);
    }
  }
  return g_copy;
}

/**
 * @brief Tests if two games are equal.
 * @param g1 the first game
 * @param g2 the second game
 * @return true if the two games are equal, false otherwise
 * @pre @p g1 must be a valid pointer toward a game structure.
 * @pre @p g2 must be a valid pointer toward a game structure.
 **/
bool game_equal(cgame g1, cgame g2) {
  // if the two games aren' create then call an error//
  test_pointer(g1);
  test_pointer(g2);
  // if the columns and the rows of g1 are differents of g2 then its false//
  if (game_nb_cols(g1) != game_nb_cols(g2) ||
      game_nb_rows(g1) != game_nb_rows(g2)) {
    return false;
  }
  // if the diagonals of g1 are differents of g2 then its false//
  if (game_is_diagadj(g1) != game_is_diagadj(g2) ||
      game_is_wrapping(g1) != game_is_wrapping(g2)) {
    return false;
  }
  // for a specific row, if g1 is different of g2 then its false//
  for (uint i = 0; i < game_nb_rows(g1); i++) {
    if (g1->nb_tents_row[i] != g2->nb_tents_row[i]) {
      return false;
    }
  }
  // for a specific column, if g1 is different of g2 then its false//
  for (uint j = 0; j < game_nb_cols(g1); j++) {
    if (g1->nb_tents_col[j] != g2->nb_tents_col[j]) {
      return false;
    }
  }
  // for all the game if a square of g1 is different of a square of g2 then its
  // false//
  for (uint i = 0; i < game_nb_rows(g1) * game_nb_cols(g2); i++) {
    if (g1->squares[i] != g2->squares[i]) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Deletes the game and frees the allocated memory.
 * @param g the game to delete
 * @pre @p g must be a valid pointer toward a game structure.
 **/
void game_delete(game g) {
  if (g != NULL) {
    free(g->nb_tents_col);
    free(g->nb_tents_row);
    free(g->squares);
    free_Moves(g->undo_hist);
    free(g->undo_hist);
    free_Moves(g->redo_hist);
    free(g->redo_hist);
  }
  free(g);
}

/**
 * @brief Sets the value of a given square.
 * @details This function is useful for initializing the squares of an empty
 * game.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param s the square value
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @pre @p s must be either EMPTY, GRASS, TENT or TREE.
 **/
void game_set_square(game g, uint i, uint j, square s) {
  test_pointer(g);
  if (s != EMPTY && s != GRASS && s != TENT && s != TREE) {
    fprintf(stderr, "You can't place this kind of square\n");
    exit(EXIT_FAILURE);
  }
  test_i_value(g, i);
  test_j_value(g, j);
  g->squares[j + i * game_nb_cols(g)] = s;
}

/**
 * @brief Gets the value of a given square.
 * @param g the game
 * @param i row index
 * @param j column index
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @return the square value
 **/
square game_get_square(cgame g, uint i, uint j) {
  test_pointer(g);
  test_i_value(g, i);
  test_j_value(g, j);
  return g->squares[j + i * game_nb_cols(g)];
}

/**
 * @brief Sets the expected number of tents in a given row.
 * @param g the game
 * @param i row index
 * @param nb_tents the expected number of tents on this row
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 **/
void game_set_expected_nb_tents_row(game g, uint i, uint nb_tents) {
  test_pointer(g);
  test_i_value(g, i);
  g->nb_tents_row[i] = nb_tents;
}

/**
 * @brief Sets the expected number of tents in a given column.
 * @param g the game
 * @param j column index
 * @param nb_tents the expected number of tents on this column
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p j < game height
 **/
void game_set_expected_nb_tents_col(game g, uint j, uint nb_tents) {
  test_pointer(g);
  test_j_value(g, j);
  g->nb_tents_col[j] = nb_tents;
}

/**
 * @brief Gets the expected number of tents in a given row.
 * @param g the game
 * @param i row index
 * @return the expected number of tents on this row
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 **/
uint game_get_expected_nb_tents_row(cgame g, uint i) {
  test_pointer(g);
  test_i_value(g, i);
  return g->nb_tents_row[i];
}

/**
 * @brief Gets the expected number of tents in a given column.
 * @param g the game
 * @param j column index
 * @return the expected number of tents on this column
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p j < game height
 **/
uint game_get_expected_nb_tents_col(cgame g, uint j) {
  test_pointer(g);
  test_j_value(g, j);
  return g->nb_tents_col[j];
}

/**
 * @brief Gets the expected number of tents on the whole grid.
 * @param g the game
 * @return the expected number of tents on this game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint game_get_expected_nb_tents_all(cgame g) {
  test_pointer(g);
  // set counter for the amount of tents.
  uint tents = 0;
  // checking array for expected number of tents for each row.
  // it could also be done by checking the array of expected number of tents for
  // each column instead.
  for (uint i = 0; i < game_nb_rows(g); i++) {
    tents += g->nb_tents_row[i];
  }
  return tents;
}

/**
 * @brief Gets the current number of tents in a given row.
 * @param g the game
 * @param i row index
 * @return the current number of tents on this row
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 **/
uint game_get_current_nb_tents_row(cgame g, uint i) {
  test_pointer(g);
  test_i_value(g, i);
  // setting a counter for the amount of tents.
  uint tents = 0;
  // checking each square of the row.
  for (int j = 0; j < game_nb_cols(g); j++) {
    // if the square is a tent then increase the counter by 1, otherwise the
    // counter is unchanged.
    if (game_get_square(g, i, j) == TENT) {
      tents++;
    }
  }
  return tents;
}

/**
 * @brief Gets the current number of tents in a given column.
 * @param g the game
 * @param j column index
 * @return the expected number of tents on this column
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p j < game height
 **/
uint game_get_current_nb_tents_col(cgame g, uint j) {
  test_pointer(g);
  test_j_value(g, j);
  // setting a counter for the amount of tents.
  uint tents = 0;
  // checking each square of the row.
  for (int i = 0; i < game_nb_rows(g); i++) {
    // if the square is a tent then increase the counter by 1, otherwise the
    // counter is unchanged.
    if (game_get_square(g, i, j) == TENT) {
      tents++;
    }
  }
  return tents;
}

/**
 * @brief Gets the current number of tents on the whole grid.
 * @param g the game
 * @return the current number of tents on this game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint game_get_current_nb_tents_all(cgame g) {
  test_pointer(g);
  // setting a counter for the amount of tents.
  uint tents = 0;
  // checking each arrow to retrieve the amount of tents for all of them.
  // it could also be done by checking the columns instead.
  for (int i = 0; i < game_nb_rows(g); i++) {
    tents += game_get_current_nb_tents_row(g, i);
  }
  return tents;
}

/**
 * @brief Plays a move in a given square.
 * @details This function allows to play both a regular or a losing move, but
 * not an illegal move (see @ref index).
 * Before playing, it stores the type of square  in row @param i and column 
 * @param j in the history of the game : @param undo_hist.
 * Once played it delete the moves in the futur queue  @param redo_hist.
 * @param g the game
 * @param i row index
 * @param j column index
 * @param s the square value
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 * @pre @p s must be either EMPTY, GRASS or TENT (but not TREE).
 * @pre The game square at position (i,j) must not be TREE.
 **/
void game_play_move(game g, uint i, uint j, square s) {
  test_pointer(g);
  test_i_value(g, i);
  test_j_value(g, j);
  if (s != EMPTY && s != GRASS && s != TENT) {
    fprintf(stderr, "You can't place this kind of square\n");
    exit(EXIT_FAILURE);
  }
  if (game_get_square(g, i, j) == TREE) {
    fprintf(stderr, "You can't replace a tree with something else\n");
    exit(EXIT_FAILURE);
  }
  move *move = create_move(game_get_square(g, i, j), i, j);
  queue_push_head(g->undo_hist, move);

  game_set_square(g, i, j, s);
  free_Moves(g->redo_hist);
}

/**
 * @brief Checks if a given move in a square is regular
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
int game_check_move(cgame g, uint i, uint j, square s) {
  test_pointer(g);
  test_i_value(g, i);
  test_j_value(g, j);
  if (s != EMPTY && s != GRASS && s != TENT && s != TREE) {
    fprintf(stderr, "You can't place this kind of square\n");
    exit(EXIT_FAILURE);
  }
  // placing or replacing TREE is illegal
  if ((s == TREE && game_get_square(g, i, j) != TREE) ||
      (s != TREE && game_get_square(g, i, j) == TREE)) {
    return ILLEGAL;
  }
  // placing an empty space is regular
  if (s == EMPTY) {
    return REGULAR;
  }

  uint *all_adj_cells = make_array_of_all_adjacent_cells(g, i, j);
  uint *ortho_adj_cells = make_array_of_ortho_adjacent_cells(g, i, j);

  if (s == TENT) {
    // placing n+1 tents in column or row is losing
    if (game_get_current_nb_tents_col(g, j) +
            (game_get_square(g, i, j) != TENT) >
        game_get_expected_nb_tents_col(g, j)) {
      free(all_adj_cells);
      free(ortho_adj_cells);
      return LOSING;
    }
    if (game_get_current_nb_tents_row(g, i) +
            (game_get_square(g, i, j) != TENT) >
        game_get_expected_nb_tents_row(g, i)) {
      free(all_adj_cells);
      free(ortho_adj_cells);
      return LOSING;
    }

    // placing tent adjacent to another tent is losing
    uint *adj_cells;

    if (game_is_diagadj(g)) {
      adj_cells = ortho_adj_cells;
    } else {
      adj_cells = all_adj_cells;
    }
    uint ind = 1;
    uint cell_i, cell_j;
    // We look if one of the adjacent cells contains a tent
    while (adj_cells[ind - 1] < game_nb_rows(g)) {
      cell_i = adj_cells[ind - 1];
      cell_j = adj_cells[ind];
      if (game_get_square(g, cell_i, cell_j) == TENT) {
        free(all_adj_cells);
        free(ortho_adj_cells);
        return LOSING;
      }
      ind += 2;
    }

    // placing tent with no tree around is losing
    ind = 1;
    uint nb_trees_around = 0;
    while (ortho_adj_cells[ind - 1] < game_nb_rows(g)) {
      cell_i = ortho_adj_cells[ind - 1];
      cell_j = ortho_adj_cells[ind];
      if (game_get_square(g, cell_i, cell_j) == TREE) {
        nb_trees_around++;
      }
      ind += 2;
    }
    if (nb_trees_around == 0) {
      free(all_adj_cells);
      free(ortho_adj_cells);
      return LOSING;
    }

    // placing more tents than trees is losing
    if (game_get_current_nb_tents_all(g) + (game_get_square(g, i, j) != TENT) >
        game_get_expected_nb_tents_all(g)) {
      free(all_adj_cells);
      free(ortho_adj_cells);
      return LOSING;
    }
  }

  if (s == GRASS) {
    // placing grass and not enough empty spaces for tents is losing
    uint nb_empty_row = 0;
    uint nb_empty_col = 0;
    for (uint b = 0; b < game_nb_cols(g); b++) {
      if (game_get_square(g, i, b) == EMPTY) {
        nb_empty_row++;
      }
    }
    for (uint a = 0; a < game_nb_rows(g); a++) {
      if (game_get_square(g, a, j) == EMPTY) {
        nb_empty_col++;
      }
    }
    if (game_get_square(g, i, j) == EMPTY) {
      nb_empty_col--;
      nb_empty_row--;
    }
    uint nb_tents_to_place_col = game_get_expected_nb_tents_col(g, j) -
                                 game_get_current_nb_tents_col(g, j);
    uint nb_tents_to_place_row = game_get_expected_nb_tents_row(g, i) -
                                 game_get_current_nb_tents_row(g, i);
    if (game_get_square(g, i, j) == TENT) {
      nb_tents_to_place_col++;
      nb_tents_to_place_row++;
    }
    if (nb_tents_to_place_row > nb_empty_row ||
        nb_tents_to_place_col > nb_empty_col) {
      free(all_adj_cells);
      free(ortho_adj_cells);
      return LOSING;
    }

    // surrounding tree by grass is losing
    uint ind = 1;
    uint cell_i, cell_j;
    while (ortho_adj_cells[ind - 1] < game_nb_rows(g)) {
      cell_i = ortho_adj_cells[ind - 1];
      cell_j = ortho_adj_cells[ind];
      // If one of the cells adj to (i, j) is a tree,
      if (game_get_square(g, cell_i, cell_j) == TREE) {
        // Then we look if all of the other cells adj to the tree are grass
        uint *cells_adj_tree =
            make_array_of_ortho_adjacent_cells(g, cell_i, cell_j);
        uint ind2 = 1;
        uint nb_grass_around_tree = 0;
        if (game_get_square(g, i, j) != GRASS) {
          nb_grass_around_tree++;
        }
        while (cells_adj_tree[ind2 - 1] < game_nb_rows(g)) {
          if (game_get_square(g, cells_adj_tree[ind2 - 1],
                              cells_adj_tree[ind2]) == GRASS) {
            nb_grass_around_tree++;
          }
          ind2 += 2;
        }
        uint nb_space_around_tree = (ind2 - 1) / 2;
        free(cells_adj_tree);
        if (nb_grass_around_tree == nb_space_around_tree) {
          free(all_adj_cells);
          free(ortho_adj_cells);
          return LOSING;
        }
      }
      ind += 2;
    }
  }
  free(all_adj_cells);
  free(ortho_adj_cells);
  return REGULAR;
}

/**
 * @brief Creates an array containing all of the given cell's adjacent cells
 *(including diagonals)
 * @param g the game
 * @param i row index
 * @param j column index
 * @return the array containing the width and height of the cells
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 **/
uint *make_array_of_all_adjacent_cells(cgame g, uint i, uint j) {
  test_pointer(g);
  test_i_value(g, i);
  test_j_value(g, j);
  uint adjacents[] = {
      i, j - 1, i - 1, j - 1, i - 1, j, i - 1, j + 1,
      i, j + 1, i + 1, j + 1, i + 1, j, i + 1, j - 1};  // contains the 8
                                                        // adjacent positions
  uint *array = (uint *)malloc(
      sizeof(uint) * 17);  // each cell can't have more than 8 adjacent cells
  if (array == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  uint counter = 0;  // counts the current index in array
  uint current_i, current_j;
  for (uint index = 0; index < 15; index += 2) {
    current_i = adjacents[index];
    current_j = adjacents[index + 1];
    // if the coordinates aren't out of the grid, then we know they're valid
    if (current_i < game_nb_rows(g) && current_j < game_nb_cols(g)) {
      array[counter] = current_i;
      array[counter + 1] = current_j;
      counter += 2;
    }
    // but if they are, there is an adjacent cell only if the game is wrapping
    else if (game_is_wrapping(g)) {
      if (current_i == game_nb_rows(g)) {
        array[counter] = 0;
      } else if (current_i > game_nb_rows(g)) {
        array[counter] = game_nb_rows(g) - 1;
      } else {
        array[counter] = current_i;
      }
      if (current_j == game_nb_cols(g)) {
        array[counter + 1] = 0;
      } else if (current_j > game_nb_cols(g)) {
        array[counter + 1] = game_nb_cols(g) - 1;
      } else {
        array[counter + 1] = current_j;
      }
      counter += 2;
    }
  }
  array[counter] =
      1000;  // can never be reached and therefore indicates the end of array
  return array;
}

/**
 * @brief Creates an array containing all of the given cell's orthogonally
 *adjacent cells
 * @param g the game
 * @param i row index
 * @param j column index
 * @return the array containing the width and height of the cells
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 * @pre @p j < game height
 **/
uint *make_array_of_ortho_adjacent_cells(cgame g, uint i, uint j) {
  test_pointer(g);
  test_i_value(g, i);
  test_j_value(g, j);
  uint adjacents[] = {
      i, j - 1, i - 1, j,
      i, j + 1, i + 1, j};  // contains the 4 orthogonally adjacent positions
  uint *array = (uint *)malloc(
      sizeof(uint) * 9);  // each cell can't have more than 4 adjacent cells
  if (array == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  uint counter = 0;  // counts the current index in array
  uint current_i, current_j;
  for (uint index = 0; index < 7; index += 2) {
    current_i = adjacents[index];
    current_j = adjacents[index + 1];
    // if the coordinates aren't out of the grid, then we know they're valid
    if (current_i < game_nb_rows(g) && current_j < game_nb_cols(g)) {
      array[counter] = current_i;
      array[counter + 1] = current_j;
      counter += 2;
    }
    // but if they are, there is an adjacent cell only if the game is wrapping
    else if (game_is_wrapping(g)) {
      if (current_i == game_nb_rows(g)) {
        array[counter] = 0;
      } else if (current_i > game_nb_rows(g)) {
        array[counter] = game_nb_rows(g) - 1;
      } else {
        array[counter] = current_i;
      }
      if (current_j == game_nb_cols(g)) {
        array[counter + 1] = 0;
      } else if (current_j > game_nb_cols(g)) {
        array[counter + 1] = game_nb_cols(g) - 1;
      } else {
        array[counter + 1] = current_j;
      }
      counter += 2;
    }
  }
  array[counter] =
      1000;  // can never be reached and therefore indicates the end of array
  return array;
}
/**
 * @brief Checks if the game is won.
 * @param g the game
 * @details This function checks that all the game rules (decribed on @ref
 * index) are satisfied, and more specifically that all the tents are correctly
 * placed. The remaining squares are either empty square or grass square.
 * @return true if the game ended successfully, false otherwise
 * @pre @p g must be a valid pointer toward a game structure.
 **/
bool game_is_over(cgame g) {
  test_pointer(g);
  // We look if there is the correct nb of tents on each row
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_current_nb_tents_row(g, i) !=
        game_get_expected_nb_tents_row(g, i)) {
      return false;
    }
  }
  // We look if there is the correct nb of tents on each col
  for (uint j = 0; j < game_nb_cols(g); j++) {
    if (game_get_current_nb_tents_col(g, j) !=
        game_get_expected_nb_tents_col(g, j)) {
      return false;
    }
  }
  // We look if each square is in a regular position
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) != TREE) {
        if (game_check_move(g, i, j, game_get_square(g, i, j)) != REGULAR) {
          return false;
        }
      }
    }
  }
  // We finally see if the nb tents = nb trees
  uint nb_trees = 0;
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TREE) {
        nb_trees++;
      }
    }
  }
  if (game_get_current_nb_tents_all(g) != nb_trees) {
    return false;
  }
  return true;
}

/**
 * @brief Fills a row with grass.
 * @details This function only puts grass on empty squares, without modifying
 * the other squares.
 * @param g the game
 * @param i row index
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p i < game width
 */
void game_fill_grass_row(game g, uint i) {
  test_pointer(g);
  test_i_value(g, i);
  for (uint j = 0; j < game_nb_cols(g); j++) {
    if (game_get_square(g, i, j) == EMPTY) {
      move *move = create_move(game_get_square(g, i, j), i, j);
      queue_push_head(g->undo_hist,
                      move);  // We add the move to the undo history
      game_set_square(g, i, j,
                      GRASS);  // fill all the empty squares in row i with grass
    }
  }
  free_Moves(g->redo_hist);
}

/**
 * @brief Fills a column with grass.
 * @details This function only puts grass on empty squares, without modifying
 * the other squares.
 * @param g the game
 * @param j column index
 * @pre @p g must be a valid pointer toward a game structure.
 * @pre @p j < game height
 */
void game_fill_grass_col(game g, uint j) {
  test_pointer(g);
  test_j_value(g, j);
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_square(g, i, j) == EMPTY) {
      move *move = create_move(game_get_square(g, i, j), i, j);
      queue_push_head(g->undo_hist,
                      move);  // We add the move to the undo history
      game_set_square(
          g, i, j, GRASS);  // fill all the empty squares in column j with grass
    }
  }
}

/**
 * @brief Restarts a game.
 * @details All the game is reset to its initial state. In particular, all the
 * squares except trees are reset to empty state.
 * @param g the game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
void game_restart(game g) {
  test_pointer(g);
  // We set all of the TENT and GRASS squares to empty
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TENT ||
          game_get_square(g, i, j) == GRASS) {
        game_set_square(g, i, j, EMPTY);
      }
    }
  }
  // We clear the histories
  free_Moves(g->redo_hist);
  free_Moves(g->undo_hist);
}

game game_new_ext(uint nb_rows, uint nb_cols, square *squares,
                  uint *nb_tents_row, uint *nb_tents_col, bool wrapping,
                  bool diagadj) {
  if (squares == NULL || nb_tents_row == NULL || nb_tents_col == NULL) {
    fprintf(stderr, "Function called on NULL pointer!\n");
    exit(EXIT_FAILURE);
  }
  // We first create an empty game with the right parameters
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, diagadj);
  // and then we add all the given information
  for (uint i = 0; i < nb_rows * nb_cols; i++) {
    g->squares[i] = squares[i];
  }
  for (uint i = 0; i < nb_rows; i++) {
    g->nb_tents_row[i] = nb_tents_row[i];
  }
  for (uint j = 0; j < nb_cols; j++) {
    g->nb_tents_col[j] = nb_tents_col[j];
  }
  return g;
}

/**
 * @brief Creates a new empty game with extended options.
 * @details All squares are initialized with empty squares, and the expected
 * numbers of tents for each row/column are set to zero.
 * @param nb_rows number of rows in game
 * @param nb_cols number of columns in game
 * @param wrapping wrapping option
 * @param diagadj diagadj option
 * @return the created game
 **/
game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping,
                        bool diagadj) {
  // We first allocate the memory for all of the arrays in the game structure
  game g = (game)malloc(sizeof(struct game_s));
  if (g == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->squares = (square *)malloc(sizeof(square) * nb_rows * nb_cols);
  if (g->squares == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->nb_tents_row = (uint *)malloc(sizeof(uint) * nb_rows);
  if (g->nb_tents_row == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->nb_tents_col = (uint *)malloc(sizeof(uint) * nb_cols);
  if (g->nb_tents_col == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  // We then give the right values to the "simple" parameters
  g->nb_rows = nb_rows;
  g->nb_cols = nb_cols;
  g->wrapping = wrapping;
  g->diagadj = diagadj;
  g->undo_hist = queue_new();
  g->redo_hist = queue_new();

  if ((g->undo_hist == NULL) || (g->redo_hist == NULL)) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  // and finally we set, for each row and column, the right nb of tents, and the
  // right value of squares
  for (uint i = 0; i < nb_rows; i++) {
    game_set_expected_nb_tents_row(g, i, 0);
  }
  for (uint j = 0; j < nb_cols; j++) {
    game_set_expected_nb_tents_col(g, j, 0);
  }
  for (uint i = 0; i < nb_rows; i++) {
    for (uint j = 0; j < nb_cols; j++) {
      game_set_square(g, i, j, EMPTY);
    }
  }
  return g;
}

/**
 * @brief Gets the number of rows (or height).
 * @param g the game
 * @return the number of rows on this game
 * @pre @p g is a valid pointer toward a cgame structure
 **/
uint game_nb_rows(cgame g) {
  test_pointer(g);
  return g->nb_rows;
}

/**
 * @brief Gets the number of columns (or width).
 * @param g the game
 * @return the the number of columns on this game
 * @pre @p g is a valid pointer toward a cgame structure
 **/
uint game_nb_cols(cgame g) {
  test_pointer(g);
  return g->nb_cols;
}

/**
 * @brief Checks if the game has the wrapping option
 * @return true, if wrapping, false otherwise
 * @pre @p g is a valid pointer toward a cgame structure
 **/
bool game_is_wrapping(cgame g) {
  test_pointer(g);
  return g->wrapping;
}

/**
 * @brief Checks if the game has the diagadj option
 * @return true, if diagadj, false otherwise
 * @pre @p g is a valid pointer toward a cgame structure
 **/
bool game_is_diagadj(cgame g) {
  test_pointer(g);
  return g->diagadj;
}

/**
 * @brief Undoes the last move.
 * @details Searches in the @ref redo_hist queue the last move and play it.
 * If no moves have been played, this function does nothing.
 * add that last move to @ref redo_hist.
 * The @ref game_restart function clears the history.
 * @param g the game
 * @pre @p g is a valid pointer toward a cgame structure
 */
void game_undo(game g) {
  if ((g == NULL) || ((g->undo_hist) == NULL) || (g->redo_hist == NULL))
    exit(EXIT_FAILURE);

  if (!queue_is_empty(g->undo_hist)) {
    move *UndoMove = queue_pop_head(g->undo_hist);
    move *RedoMove = create_move(game_get_square(g, UndoMove->i, UndoMove->j),
                                 UndoMove->i, UndoMove->j);

    queue_push_head(g->redo_hist, RedoMove);
    game_set_square(g, UndoMove->i, UndoMove->j, UndoMove->s);
    free(UndoMove);
    UndoMove = NULL;
  }
  return;
}

/**
 * @brief Redoes the last move.
 * @details Searches in the history the last cancelled move (by calling @ref
 * game_undo), and replays it. If there are no more moves to be replayed, this
 * function does nothing. 
 * add the move created by the old square to @ref redo_hist.
 * @param g the game
 * @pre @p g is a valid pointer toward a cgame structure
 */
void game_redo(game g) {
  if ((g == NULL) || ((g->redo_hist) == NULL) || (g->undo_hist == NULL))
    exit(EXIT_FAILURE);

  if (!queue_is_empty(g->redo_hist)) {
    move *RedoMove = queue_pop_head(g->redo_hist);
    move *UndoMove = create_move(game_get_square(g, RedoMove->i, RedoMove->j),
                                 RedoMove->i, RedoMove->j);

    queue_push_head(g->undo_hist, UndoMove);
    game_set_square(g, RedoMove->i, RedoMove->j, RedoMove->s);
    free(RedoMove);
    RedoMove = NULL;
  }
  return;
}

/**
 * @brief Checks if the given game is NULL and exits the program if it is
 * @param g the game
 **/
void test_pointer(cgame g) {
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Checks if the given i value is bigger than the nb rows in game
 * and exits program if it is
 * @param g the game
 * @param i row index
 **/
void test_i_value(cgame g, uint i) {
  if (i >= game_nb_rows(g)) {
    fprintf(stderr, "row number is invalid\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Checks if the given j value is bigger than the nb columns in game
 * and exits program if it is
 * @param g the game
 * @param j column index
 **/
void test_j_value(cgame g, uint j) {
  if (j >= game_nb_cols(g)) {
    fprintf(stderr, "column number is invalid\n");
    exit(EXIT_FAILURE);
  }
}