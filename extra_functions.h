#include "game.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game_aux.h"
#include "game_ext.h"
#include "queue.h"

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
uint *make_array_of_all_adjacent_cells(cgame g, uint i, uint j);

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
uint *make_array_of_ortho_adjacent_cells(cgame g, uint i, uint j);

/**
 * @brief Checks if the given game is NULL and exits the program if it is
 * @param g the game
 **/
void test_pointer(cgame g);

/**
 * @brief Checks if the given i value is bigger than the nb rows in game
 * and exits program if it is
 * @param g the game
 * @param i row index
 **/
void test_i_value(cgame g, uint i);

/**
 * @brief Checks if the given j value is bigger than the nb columns in game
 * and exits program if it is
 * @param g the game
 * @param j column index
 **/
void test_j_value(cgame g, uint j);