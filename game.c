#include "game.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game_aux.c"

struct game_s {
  square *squares;
  uint *nb_tents_row;
  uint *nb_tents_col;
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
  g->squares = squares;
  g->nb_tents_col = nb_tents_col;
  g->nb_tents_row = nb_tents_row;
  return g;
}

/**
 * @brief Creates a new empty game with defaut size.
 * @details All squares are initialized with empty squares, and the expected
 * numbers of tents for each row/column are set to zero.
 * @return the created game
 **/
game game_new_empty(void) {
  game g;
  g = (game)malloc(sizeof(struct game_s));
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
  game g_copy = game_new_empty();
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    uint nb_c = game_get_expected_nb_tents_col(g, i);
    uint nb_r = game_get_expected_nb_tents_row(g, i);
    game_set_expected_nb_tents_col(g_copy, i, nb_c);
    game_set_expected_nb_tents_row(g_copy, i, nb_r);
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
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
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    if (game_get_expected_nb_tents_col(g1, i) !=
            game_get_expected_nb_tents_col(g2, i) ||
        game_get_expected_nb_tents_row(g1, i) !=
            game_get_expected_nb_tents_row(g2, i)) {
      return false;
    }
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g1, i, j) != game_get_square(g2, i, j)) {
        return false;
      }
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
  if ((g==NULL)|| (g->squares==NULL)){
    printf("game doesn't exist");
    exit(EXIT_FAILURE);
  }
  g->squares[j+i*DEFAULT_SIZE]=s;
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
  if ((g==NULL)|| (g->squares==NULL)){
    printf("not enough memory");
    exit(EXIT_FAILURE); 
  }
  return g->squares[j+i*DEFAULT_SIZE]; 
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
  if ((g==NULL)|| (g->nb_tents_row==NULL)){
    printf("not enough memory");
    exit(EXIT_FAILURE); 
  }
  g->nb_tents_row[i]=nb_tents; 


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
  if ((g==NULL)|| (g->nb_tents_col==NULL)){
    printf("not enough memory");
    exit(EXIT_FAILURE); 
  }
  g->nb_tents_col[j]=nb_tents; 
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
  return g->nb_tents_col[j];
}

/**
 * @brief Gets the expected number of tents on the whole grid.
 * @param g the game
 * @return the expected number of tents on this game
 * @pre @p g must be a valid pointer toward a game structure.
 **/
uint game_get_expected_nb_tents_all(cgame g) {
  // set counter for the amount of tents.
  int tents = 0;
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
  // setting a counter for the amount of tents.
  int tents = 0;
  // checking each square of the row.
  for (int j = 0; j < DEFAULT_SIZE; j++) {
    // if the square is a tent then increase the counter by 1, otherwise the
    // counter is unchanged.
    if (game_get_square(g, i, j) == 2) {
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
  // setting a counter for the amount of tents.
  int tents = 0;
  // checking each square of the row.
  for (int i = 0; i < DEFAULT_SIZE; i++) {
    // if the square is a tent then increase the counter by 1, otherwise the
    // counter is unchanged.
    if (game_get_square(g, i, j) == 2) {
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
  // setting a counter for the amount of tents.
  int tents = 0;
  // checking each arrow to retrieve the amount of tents for all of them.
  // it could also be done by checking the columns instead.
  for (int i = 0; i < DEFAULT_SIZE; i++) {
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
  if (i >= DEFAULT_SIZE || j >= DEFAULT_SIZE) {
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
  game_set_square(g, i, j, s);
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
  if (i >= DEFAULT_SIZE || j >= DEFAULT_SIZE) {
    fprintf(stderr, "row or column number is invalid\n");
    exit(EXIT_FAILURE);
  }
  if (s != EMPTY && s != GRASS && s != TENT && s != TREE) {
    fprintf(stderr, "You can't place this kind of square\n");
    exit(EXIT_FAILURE);
  }
  // placing or replacing TREE is illegal
  if (s == TREE || game_get_square(g, i, j) == TREE) {
    return ILLEGAL;
  }
  if (s == TENT) {
    // placing n+1 tents in column or row is losing
    if (game_get_current_nb_tents_col(g, j) >=
        game_get_expected_nb_tents_col(g, j)) {
      return LOSING;
    }
    if (game_get_current_nb_tents_row(g, i) >=
        game_get_expected_nb_tents_row(g, i)) {
      return LOSING;
    }
    // placing tent adjacent to another tent is losing
    if (game_get_square(g, i - 1, j) == TENT ||
        game_get_square(g, i - 1, j - 1) == TENT ||
        game_get_square(g, i, j - 1) == TENT ||
        game_get_square(g, i + 1, j - 1) == TENT ||
        game_get_square(g, i + 1, j) == TENT ||
        game_get_square(g, i + 1, j + 1) == TENT ||
        game_get_square(g, i, j + 1) == TENT ||
        game_get_square(g, i - 1, j + 1) == TENT) {
      return LOSING;
    }
    // placing tent with no tree around is losing
    if (game_get_square(g, i - 1, j) != TREE &&
        game_get_square(g, i, j - 1) != TREE &&
        game_get_square(g, i + 1, j) != TREE &&
        game_get_square(g, i, j + 1) != TREE) {
      return LOSING;
    }
  }
  // placing grass and not enough empty spaces for tents is losing

  // surrounding tree with grass is losing
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
  for (uint i=0; i<DEFAULT_SIZE; i++){
    if (game_get_current_nb_tents_row(g,i)!=
        game_get_expected_nb_tents_row(g,i)){
          return false;
        }
    if (game_get_current_nb_tents_col(g,i)!=
        game_get_expected_nb_tents_col(g,i)){
          return false;
        }
    for (uint j = 0 ; j<DEFAULT_SIZE ; j++){
      if (game_get_square(g,i,j)!=TREE){
        if (game_check_move(g,i,j,game_get_square(g,i,j))!= REGULAR){
          return false ;
        }
      }
    }
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
  if (i >= DEFAULT_SIZE) {
    fprintf(stderr, "row number is invalid\n");
    exit(EXIT_FAILURE);
  }
  for (uint j = 0; j < DEFAULT_SIZE; j++) {
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
  if (j >= DEFAULT_SIZE) {
    fprintf(stderr, "column number is invalid\n");
    exit(EXIT_FAILURE);
  }
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
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
  for (uint i = 0; i < DEFAULT_SIZE; i++) {
    for (uint j = 0; j < DEFAULT_SIZE; j++) {
      if (game_get_square(g, i, j) == TENT ||
          game_get_square(g, i, j) == GRASS) {
        game_set_square(g, i, j, EMPTY);
      }
    }
  }
}