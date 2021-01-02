#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

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

struct move{
  square s;
  uint i;
  uint j;
};
/**
 * @brief The structure that stores the move.
 **/
typedef struct move move;

move * create_move(square s, uint i, uint j){
  move * move = malloc(sizeof(move));
  if (move == NULL) exit(EXIT_FAILURE);
  move->s = s;
  move->i = i;
  move->j = j;
  return move;
}

void free_Moves(queue * queue){
  if (queue !=NULL){
    while(!queue_is_empty(queue)){
      move * move = queue_pop_tail(queue);
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
  if (squares == NULL || nb_tents_row == NULL || nb_tents_col == NULL) {
    fprintf(stderr, "Function called on NULL pointer!\n");
    exit(EXIT_FAILURE);
  }
  game g = game_new_empty();
  for (uint i = 0; i < DEFAULT_SIZE * DEFAULT_SIZE; i++) {
    g->squares[i] = squares[i];
  }
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
  game g = (game)malloc(sizeof(struct game_s));
  if (g == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->squares = (square *)malloc(sizeof(square) * DEFAULT_SIZE * DEFAULT_SIZE);
  if (g->squares == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->nb_tents_col = (uint *)malloc(sizeof(uint) * DEFAULT_SIZE);
  if (g->nb_tents_col == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
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
  if (g == NULL) {
    fprintf(stderr, "Function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  game g_copy = game_new_empty_ext(game_nb_rows(g), game_nb_cols(g), game_is_wrapping(g), game_is_diagadj(g));
  for (uint i = 0; i < game_nb_rows(g); i++) {
    uint nb_r = game_get_expected_nb_tents_row(g, i);
    game_set_expected_nb_tents_row(g_copy, i, nb_r);
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    uint nb_c = game_get_expected_nb_tents_col(g, j);
    game_set_expected_nb_tents_col(g_copy, j, nb_c);
  }
  for (uint i = 0; i < game_nb_rows(g); i++){
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
  if (g1 == NULL || g2 == NULL) {
    fprintf(stderr, "Function call on NULL pointer\n");
  }
  if (game_nb_cols(g1) != game_nb_cols(g2) || game_nb_rows(g1) != game_nb_rows(g2)){
    return false;
  }
  if (game_is_diagadj(g1) != game_is_diagadj(g2) || game_is_wrapping(g1) != game_is_wrapping(g2)){
    return false;
  }
  for (uint i = 0; i < game_nb_rows(g1); i++) {
    if (g1->nb_tents_row[i] != g2->nb_tents_row[i]) {
      return false;
    }
  }
  for (uint j = 0; j < game_nb_cols(g1); j++) {
    if (g1->nb_tents_col[j] != g2->nb_tents_col[j]) {
      return false;
    }
  }
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
  if ((g == NULL) || (g->squares == NULL)) {
    printf("game doesn't exist");
    exit(EXIT_FAILURE);
  }
  if (s != EMPTY && s != GRASS && s != TENT && s != TREE) {
    fprintf(stderr, "You can't place this kind of square\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    printf("here\n");
    fprintf(stderr, "row or column number is invalid\n");
    exit(EXIT_FAILURE);
  }
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
  if ((g == NULL) || (g->squares == NULL)) {
    fprintf(stderr, "not enough memory\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "row or column number is invalid\n");
    exit(EXIT_FAILURE);
  }
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
  if ((g == NULL) || (g->nb_tents_row == NULL)) {
    fprintf(stderr, "not enough memory\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g)) {
    fprintf(stderr, "row number is invalid\n");
    exit(EXIT_FAILURE);
  }
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
  if ((g == NULL) || (g->nb_tents_col == NULL)) {
    printf("not enough memory");
    exit(EXIT_FAILURE);
  }
  if (j >= game_nb_cols(g)) {
    fprintf(stderr, "invalid column number\n");
    exit(EXIT_FAILURE);
  }
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
  if (g == NULL || g->nb_tents_row == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g)) {
    fprintf(stderr, "invalid row number\n");
    exit(EXIT_FAILURE);
  }
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
  if (g == NULL || g->nb_tents_col == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (j >= game_nb_cols(g)) {
    fprintf(stderr, "invalid column number\n");
    exit(EXIT_FAILURE);
  }
  return g->nb_tents_col[j];
}

/**
 * @brief Gets the expected number of tents on the whole grid.
 * @param g the game
 * @return the expected number of tents on this game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint game_get_expected_nb_tents_all(cgame g) {
  if (g == NULL || g->nb_tents_row == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  // set counter for the amount of tents.
  uint tents = 0;
  // checking array for expected number of tents for each row.
  // it could also be done by checking the array of expected number of tents for
  // each column instead.
  for (int i = 0; i < DEFAULT_SIZE; i++) {
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g)) {
    fprintf(stderr, "invalid row number\n");
    exit(EXIT_FAILURE);
  }
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (j >= game_nb_cols(g)) {
    fprintf(stderr, "invalid column number\n");
    exit(EXIT_FAILURE);
  }
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "row or column number is invalid\n");
    exit(EXIT_FAILURE);
  }
  if (s != EMPTY && s != GRASS && s != TENT) {
    fprintf(stderr, "You can't place this kind of square\n");
    exit(EXIT_FAILURE);
  }
  if (game_get_square(g, i, j) == TREE) {
    fprintf(stderr, "You can't replace a tree with something else\n");
    exit(EXIT_FAILURE);
  }
  move * move = create_move(game_get_square(g,i,j), i,j);
  queue_push_head(g->undo_hist,move);

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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    fprintf(stderr, "row or column number is invalid\n");
    exit(EXIT_FAILURE);
  }
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
  uint i_over, i_under, j_left, j_right; //used when game is wrapping

  if (s == TENT) {
    // placing n+1 tents in column or row is losing
    if (game_get_current_nb_tents_col(g, j) >=
            game_get_expected_nb_tents_col(g, j) &&
        game_get_square(g, i, j) != TENT) {
      return LOSING;
    }
    if (game_get_current_nb_tents_row(g, i) >=
            game_get_expected_nb_tents_row(g, i) &&
        game_get_square(g, i, j) != TENT) {
      return LOSING;
    }
    if (game_get_current_nb_tents_row(g, i) >
            game_get_expected_nb_tents_row(g, i) &&
        game_get_square(g, i, j) == TENT) {
      return LOSING;
    }
    if (game_get_current_nb_tents_col(g, j) >
            game_get_expected_nb_tents_col(g, j) &&
        game_get_square(g, i, j) == TENT) {
      return LOSING;
    }
    // placing tent adjacent to another tent is losing (in case of wrapping = false game)
    if (!game_is_wrapping(g)){
      if (i > 0 && game_get_square(g, i - 1, j) == TENT) {
        return LOSING;
      }
      if (j > 0 && game_get_square(g, i, j - 1) == TENT) {
        return LOSING;
      }
      if (j < game_nb_cols(g) - 1 && game_get_square(g, i, j + 1) == TENT) {
        return LOSING;
      }
      if (i < game_nb_rows(g) - 1 && game_get_square(g, i + 1, j) == TENT) {
        return LOSING;
      }
      if (!game_is_diagadj(g)){
        if (i < game_nb_rows(g) - 1 && j < game_nb_cols(g) - 1 &&
            game_get_square(g, i + 1, j + 1) == TENT) {
          return LOSING;
        }
        if (i > 0 && j > 0 && game_get_square(g, i - 1, j - 1) == TENT) {
          return LOSING;
        }
        if (i < game_nb_rows(g) - 1 && j > 0 &&
            game_get_square(g, i + 1, j - 1) == TENT) {
          return LOSING;
        }
        if (i > 0 && j < game_nb_cols(g) - 1 &&
            game_get_square(g, i - 1, j + 1) == TENT) {
          return LOSING;
        }
      }
    }
    // placing tent adjacent to another tent is losing (in case of wrapping = true game)
    if (game_is_wrapping(g)){
      if (i == 0){
        i_over = game_nb_rows(g) - 1;
        i_under = i + 1;
      }else if (i == game_nb_rows(g) - 1){
        i_over = i-1;
        i_under = 0;
      }else{
        i_over = i - 1;
        i_under = i + 1;
      }
      if (j == 0){
        j_left = game_nb_cols(g) - 1;
        j_right = j + 1;
      }else if (j == game_nb_cols(g) - 1){
        j_left = j-1;
        j_right = 0;
      }else{
        j_left = j - 1;
        j_right = j + 1;
      }
      if (game_get_square(g, i_over, j) == TENT || game_get_square(g, i, j_right) == TENT || game_get_square(g, i_under, j) == TENT || game_get_square(g, i, j_left) == TENT){
        return LOSING;
      }
      if (!game_is_diagadj(g)){
        if (game_get_square(g, i_over, j_left) == TENT || game_get_square(g, i_over, j_right) == TENT || game_get_square(g, i_under, j_left) == TENT || game_get_square(g, i_under, j_right) == TENT){
          return LOSING;
        }
      }
    }
    // placing tent with no tree around is losing
    if (!game_is_wrapping(g)){
      uint nb_trees_around = 0;
      if (i > 0 && game_get_square(g, i - 1, j) == TREE) {
        nb_trees_around++;
      }
      if (j > 0 && game_get_square(g, i, j - 1) == TREE) {
        nb_trees_around++;
      }
      if (i < game_nb_rows(g) - 1 && game_get_square(g, i + 1, j) == TREE) {
        nb_trees_around++;
      }
      if (j < game_nb_cols(g) - 1 && game_get_square(g, i, j + 1) == TREE) {
        nb_trees_around++;
      }
      if (nb_trees_around == 0) {
        return LOSING;
      }
    }
    if (game_is_wrapping(g)){
      if (game_get_square(g, i_over, j) != TREE && game_get_square(g, i, j_right) != TREE && game_get_square(g, i_under, j) != TREE && game_get_square(g, i, j_left) != TREE){
        return LOSING;
      }
    }

    // placing more tents than trees is losing
    if (game_get_square(g, i, j) == TENT &&
        game_get_current_nb_tents_all(g) > game_get_expected_nb_tents_all(g)) {
      return LOSING;
    }
    if (game_get_square(g, i, j) != TENT &&
        game_get_current_nb_tents_all(g) + 1 >
            game_get_expected_nb_tents_all(g)) {
      return LOSING;
    }
    // having less tents than trees when board is full is losing
    uint nb_empty = 0;
    for (uint a = 0; a < game_nb_rows(g); a++) {
      for (uint b = 0; b < game_nb_cols(g); b++) {
        if (game_get_square(g, a, b) == EMPTY) {
          nb_empty++;
        }
      }
    }
    if (game_get_square(g, i, j) == EMPTY && nb_empty == 1 &&
        game_get_current_nb_tents_all(g) + 1 <
            game_get_expected_nb_tents_all(g)) {
      return LOSING;
    }
    if (game_get_square(g, i, j) == TENT && nb_empty == 0 &&
        game_get_current_nb_tents_all(g) < game_get_expected_nb_tents_all(g)) {
      return LOSING;
    }
    if (game_get_square(g, i, j) == GRASS && nb_empty == 0 &&
        game_get_current_nb_tents_all(g) + 1 <
            game_get_expected_nb_tents_all(g)) {
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
      return LOSING;
    }

    // surrounding tree by grass is losing (in case of wrapping = false)
    if (!game_is_wrapping(g)){
      if (i < game_nb_rows(g) - 1 && game_get_square(g, i + 1, j) == TREE) {
        uint i_tree = i + 1;
        uint j_tree = j;
        uint nb_grass_around_tree = 0;
        uint nb_space_around = 0;
        if (i_tree < game_nb_rows(g) - 1) {
          nb_space_around++;
          if (game_get_square(g, i_tree + 1, j_tree) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (j_tree < game_nb_cols(g) - 1) {
          nb_space_around++;
          if (game_get_square(g, i_tree, j_tree + 1) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (j_tree > 0) {
          nb_space_around++;
          if (game_get_square(g, i_tree, j_tree - 1) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (nb_grass_around_tree == nb_space_around) {
          return LOSING;
        }
      }

      if (j < game_nb_cols(g) - 1 && game_get_square(g, i, j + 1) == TREE) {
        uint i_tree = i;
        uint j_tree = j + 1;
        uint nb_grass_around_tree = 0;
        uint nb_space_around = 0;
        if (i_tree < game_nb_rows(g) - 1) {
          nb_space_around++;
          if (game_get_square(g, i_tree + 1, j_tree) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (j_tree < game_nb_cols(g) - 1) {
          nb_space_around++;
          if (game_get_square(g, i_tree, j_tree + 1) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (i_tree > 0) {
          nb_space_around++;
          if (game_get_square(g, i_tree - 1, j_tree) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (nb_grass_around_tree == nb_space_around) {
          return LOSING;
        }
      }

      if (i > 0 && game_get_square(g, i - 1, j) == TREE) {
        uint i_tree = i - 1;
        uint j_tree = j;
        uint nb_grass_around_tree = 0;
        uint nb_space_around = 0;
        if (j_tree > 0) {
          nb_space_around++;
          if (game_get_square(g, i_tree, j_tree - 1) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (j_tree < game_nb_cols(g) - 1) {
          nb_space_around++;
          if (game_get_square(g, i_tree, j_tree + 1) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (i_tree > 0) {
          nb_space_around++;
          if (game_get_square(g, i_tree - 1, j_tree) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (nb_grass_around_tree == nb_space_around) {
          return LOSING;
        }
      }

      if (j > 0 && game_get_square(g, i, j - 1) == TREE) {
        uint i_tree = i;
        uint j_tree = j - 1;
        uint nb_grass_around_tree = 0;
        uint nb_space_around = 0;
        if (j_tree > 0) {
          nb_space_around++;
          if (game_get_square(g, i_tree, j_tree - 1) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (i_tree < game_nb_rows(g) - 1) {
          nb_space_around++;
          if (game_get_square(g, i_tree + 1, j_tree) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (i_tree > 0) {
          nb_space_around++;
          if (game_get_square(g, i_tree - 1, j_tree) == GRASS) {
            nb_grass_around_tree++;
          }
        }
        if (nb_grass_around_tree == nb_space_around) {
          return LOSING;
        }
      }
    }

    // surrounding tree by grass is losing (in case of wrapping = true)
    if (game_is_wrapping(g)){
      uint i_tree_over, i_tree_under, j_tree_left, j_tree_right;
      if (game_get_square(g, i_under, j) == TREE) {
        uint i_tree = i_under;
        uint j_tree = j;
        if (i_tree == 0){
          i_tree_over = game_nb_rows(g) - 1;
          i_tree_under = i_tree + 1;
        }else if (i_tree == game_nb_rows(g) - 1){
          i_tree_over = i_tree-1;
          i_tree_under = 0;
        }else{
          i_tree_over = i_tree - 1;
          i_tree_under = i_tree + 1;
        }
        if (j_tree == 0){
          j_tree_left = game_nb_cols(g) - 1;
          j_tree_right = j_tree + 1;
        }else if (j_tree == game_nb_cols(g) - 1){
          j_tree_left = j_tree-1;
          j_tree_right = 0;
        }else{
          j_tree_left = j_tree - 1;
          j_tree_right = j_tree + 1;
        }
        if (game_get_square(g, i_tree_under, j_tree) == GRASS && game_get_square(g, i_tree, j_tree_right) == GRASS && game_get_square(g, i_tree, j_tree_left) == GRASS){
          return LOSING;
        }
      }
      if (game_get_square(g, i, j_right) == TREE) {
        uint i_tree = i;
        uint j_tree = j_right;
        if (i_tree == 0){
          i_tree_over = game_nb_rows(g) - 1;
          i_tree_under = i_tree + 1;
        }else if (i_tree == game_nb_rows(g) - 1){
          i_tree_over = i_tree-1;
          i_tree_under = 0;
        }else{
          i_tree_over = i_tree - 1;
          i_tree_under = i_tree + 1;
        }
        if (j_tree == 0){
          j_tree_left = game_nb_cols(g) - 1;
          j_tree_right = j_tree + 1;
        }else if (j_tree == game_nb_cols(g) - 1){
          j_tree_left = j_tree-1;
          j_tree_right = 0;
        }else{
          j_tree_left = j_tree - 1;
          j_tree_right = j_tree + 1;
        }
        if (game_get_square(g, i_tree_under, j_tree) == GRASS && game_get_square(g, i_tree, j_tree_right) == GRASS && game_get_square(g, i_tree_over, j_tree) == GRASS){
          return LOSING;
        }
      }
      if (game_get_square(g, i_over, j) == TREE) {
        uint i_tree = i_over;
        uint j_tree = j;
        if (i_tree == 0){
          i_tree_over = game_nb_rows(g) - 1;
          i_tree_under = i_tree + 1;
        }else if (i_tree == game_nb_rows(g) - 1){
          i_tree_over = i_tree-1;
          i_tree_under = 0;
        }else{
          i_tree_over = i_tree - 1;
          i_tree_under = i_tree + 1;
        }
        if (j_tree == 0){
          j_tree_left = game_nb_cols(g) - 1;
          j_tree_right = j_tree + 1;
        }else if (j_tree == game_nb_cols(g) - 1){
          j_tree_left = j_tree-1;
          j_tree_right = 0;
        }else{
          j_tree_left = j_tree - 1;
          j_tree_right = j_tree + 1;
        }
        if (game_get_square(g, i_tree_over, j_tree) == GRASS && game_get_square(g, i_tree, j_tree_right) == GRASS && game_get_square(g, i_tree, j_tree_left) == GRASS){
          return LOSING;
        }
      }
      if (game_get_square(g, i, j_left) == TREE) {
        uint i_tree = i;
        uint j_tree = j_left;
        if (i_tree == 0){
          i_tree_over = game_nb_rows(g) - 1;
          i_tree_under = i_tree + 1;
        }else if (i_tree == game_nb_rows(g) - 1){
          i_tree_over = i_tree-1;
          i_tree_under = 0;
        }else{
          i_tree_over = i_tree - 1;
          i_tree_under = i_tree + 1;
        }
        if (j_tree == 0){
          j_tree_left = game_nb_cols(g) - 1;
          j_tree_right = j_tree + 1;
        }else if (j_tree == game_nb_cols(g) - 1){
          j_tree_left = j_tree-1;
          j_tree_right = 0;
        }else{
          j_tree_left = j_tree - 1;
          j_tree_right = j_tree + 1;
        }
        if (game_get_square(g, i_tree_under, j_tree) == GRASS && game_get_square(g, i_tree_over, j_tree) == GRASS && game_get_square(g, i_tree, j_tree_left) == GRASS){
          return LOSING;
        }
      }
    }
  }
  return REGULAR;
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer!\n");
    exit(EXIT_FAILURE);
  }
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_current_nb_tents_row(g, i) !=
        game_get_expected_nb_tents_row(g, i)) {
      return false;
    }
  }
  for (uint j = 0; j < game_nb_cols(g); j++){
    if (game_get_current_nb_tents_col(g, j) !=
        game_get_expected_nb_tents_col(g, j)) {
      return false;
    }
  }
  for (uint i = 0; i < game_nb_rows(g); i++){
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) != TREE) {
        if (game_check_move(g, i, j, game_get_square(g, i, j)) != REGULAR) {
          return false;
        }
      }
    }
  }
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g)) {
    fprintf(stderr, "row number is invalid\n");
    exit(EXIT_FAILURE);
  }
  for (uint j = 0; j < game_nb_cols(g); j++) {
    if (game_get_square(g, i, j) == EMPTY) {
      game_set_square(g, i, j, GRASS);
    }
  }
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  if (j >= game_nb_cols(g)) {
    fprintf(stderr, "column number is invalid\n");
    exit(EXIT_FAILURE);
  }
  for (uint i = 0; i < game_nb_rows(g); i++) {
    if (game_get_square(g, i, j) == EMPTY) {
      game_set_square(g, i, j, GRASS);
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
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      if (game_get_square(g, i, j) == TENT ||
          game_get_square(g, i, j) == GRASS) {
        game_set_square(g, i, j, EMPTY);
      }
    }
  }
}

game game_new_ext(uint nb_rows, uint nb_cols, square *squares, uint *nb_tents_row, uint *nb_tents_col, bool wrapping,
                  bool diagadj){
  if (squares == NULL || nb_tents_row == NULL || nb_tents_col == NULL) {
    fprintf(stderr, "Function called on NULL pointer!\n");
    exit(EXIT_FAILURE);
  }
  game g = game_new_empty_ext(nb_rows, nb_cols, wrapping, diagadj);
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
game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping, bool diagadj){
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
  g->nb_tents_col = (uint *)malloc(sizeof(uint) * nb_cols);
  if (g->nb_tents_col == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
  g->nb_tents_row = (uint *)malloc(sizeof(uint) * nb_rows);
  if (g->nb_tents_row == NULL) {
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
  }
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

  for (uint i = 0; i < nb_rows; i++) {
    game_set_expected_nb_tents_row(g, i, 0);
  }
  for (uint j = 0; j < nb_cols; j++){
    game_set_expected_nb_tents_col(g, j, 0);
  }
  for (uint i = 0; i < nb_rows; i++){
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
uint game_nb_rows(cgame g){
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  return g->nb_rows;
}

/**
 * @brief Gets the number of columns (or width).
 * @param g the game
 * @return the the number of columns on this game
 * @pre @p g is a valid pointer toward a cgame structure
 **/
uint game_nb_cols(cgame g){
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  return g->nb_cols;
}

/**
 * @brief Checks if the game has the wrapping option
 * @return true, if wrapping, false otherwise
 * @pre @p g is a valid pointer toward a cgame structure
 **/
bool game_is_wrapping(cgame g){
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  return g->wrapping;
}

/**
 * @brief Checks if the game has the diagadj option
 * @return true, if diagadj, false otherwise
 * @pre @p g is a valid pointer toward a cgame structure
 **/
bool game_is_diagadj(cgame g){
  if (g == NULL) {
    fprintf(stderr, "function called on NULL pointer\n");
    exit(EXIT_FAILURE);
  }
  return g->diagadj;
}

/**
 * @brief Undoes the last move.
 * @details Searches in the history the last move played (by calling
 * @ref game_play_move or @ref game_redo), and restores the state of the game
 * before that move. If no moves have been played, this function does nothing.
 * The @ref game_restart function clears the history.
 * @param g the game
 * @pre @p g is a valid pointer toward a cgame structure
 */
void game_undo(game g){

  if ((g == NULL) || ((g->undo_hist)==NULL) || (g->redo_hist == NULL)) exit(EXIT_FAILURE);

  if(!queue_is_empty(g->undo_hist)){
    move * UndoMove = queue_pop_head(g->undo_hist);
    move * RedoMove= create_move( game_get_square(g , UndoMove->i , UndoMove->j) ,  UndoMove->i , UndoMove->j);

    queue_push_head(g->redo_hist, RedoMove);
    game_set_square(g , UndoMove->i , UndoMove->j , UndoMove->s);
    free(UndoMove);
    UndoMove=NULL;

  }
  return;
}

/**
 * @brief Redoes the last move.
 * @details Searches in the history the last cancelled move (by calling @ref
 * game_undo), and replays it. If there are no more moves to be replayed, this
 * function does nothing. After playing a new move with @ref game_play_move, it
 * is no longer possible to redo an old cancelled move.
 * @param g the game
 * @pre @p g is a valid pointer toward a cgame structure
 */
void game_redo(game g){
  if ((g == NULL) || ((g->redo_hist)==NULL) || (g->undo_hist == NULL)) exit(EXIT_FAILURE);

  if (!queue_is_empty(g->redo_hist)){
    move * RedoMove = queue_pop_head(g->redo_hist);
    move * UndoMove= create_move( game_get_square(g , RedoMove->i , RedoMove->j) ,  RedoMove->i , RedoMove->j);

    queue_push_head(g->undo_hist, UndoMove);
    game_set_square(g , RedoMove->i , RedoMove->j , RedoMove->s);
    free(RedoMove);
    RedoMove= NULL;
  }
  return;
}