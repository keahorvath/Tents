#version minimale de cmake nécessaire
cmake_minimum_required(VERSION 2.6)

#nom du projet
project(game_text C)

enable_testing()
include(CTest)

#positionne la variable
set(CMAKE_C_FLAGS "-std=c99 -g -Wall")

set(CMAKE_BUILD_TYPE_DEBUG)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_COMPILER gcc)

#définit le nom du programme ainsi que ses sources
add_executable(game_text game_text.c)
add_executable(game_test_marbeites game_test_marbeites.c)
add_executable(game_test_khorvath game_test_khorvath.c)
add_executable(game_test_amastouri game_test_amastouri.c)
add_executable(game_test_alleymarie game_test_alleymarie.c)

#créer la librairie
add_library(game game.o game_aux.c)

#définit les bibliothèques utilisées
target_link_libraries(game_text game)
target_link_libraries(game_test_marbeites game)
target_link_libraries(game_test_amastouri game)


#tests marbeites
add_test(test_marbeites_test_get_expected_nb_tents_row ./game_test_marbeites get_expected_nb_tents_row)
add_test(test_marbeites_test_get_expected_nb_tents_col ./game_test_marbeites get_expected_nb_tents_col)
add_test(test_marbeites_test_get_expected_nb_tents_all ./game_test_marbeites get_expected_nb_tents_all)
add_test(test_marbeites_test_get_current_nb_tents_row ./game_test_marbeites get_current_nb_tents_row)
add_test(test_marbeites_test_get_current_nb_tents_col ./game_test_marbeites get_current_nb_tents_col)
add_test(test_marbeites_test_get_current_nb_tents_all ./game_test_marbeites get_current_nb_tents_all)

#tests amastouri
add_test(test_amastouri_test_game_set_square ./game_test_amastouri game_set_square)
add_test(test_amastouri_test_game_get_square ./game_test_amastouri game_get_square)
add_test(test_amastouri_test_game_equal ./game_test_amastouri game_equal)
add_test(test_amastouri_test_game_delete ./game_test_amastouri game_delete)
add_test(test_amastouri_test_game_set_expected_nb_tents_row ./game_test_amastouri game_set_expected_nb_tents_row)
add_test(test_amastouri_test_game_set_expected_nb_tents_col ./game_test_amastouri game_set_expected_nb_tents_col)


add_test(test_khorvath_dummy ./game_test_khorvath dummy)
add_test(test_alleymarie_dummy ./game_test_alleymarie dummy)
