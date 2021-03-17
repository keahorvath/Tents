#version minimale de cmake nécessaire
cmake_minimum_required(VERSION 2.6)

#nom du projet
project(tents C)

enable_testing()
include(CTest)

#positionne la variable
set(CMAKE_C_FLAGS "-std=c99 -g --coverage -Wall")

set(CMAKE_BUILD_TYPE_DEBUG)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS_RELEASE "-O3")

## use a CMake3 package to solve SDL2 dependencies
find_package(SDL2 REQUIRED)
set(SDL2_ALL_INC ${SDL2_INCLUDE_DIRS})
set(SDL2_ALL_LIBS ${SDL2_LIBRARIES} -lSDL2_ttf -lSDL2_image) # add extra SDL2 libraries

## print this 
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

## compilation rules
include_directories(${SDL2_ALL_INC})
add_executable(game_sdl game_sdl.c model.c)
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} m game)

#définit le nom du programme ainsi que ses sources
add_executable(game_text game_text.c)
add_executable(game_test_khorvath game_test_khorvath.c)
add_executable(game_test_amastouri game_test_amastouri.c)
add_executable(game_test_alleymarie game_test_alleymarie.c)
add_executable(game_test_marbeites game_test_marbeites.c)
add_executable(game_solve game_solve.c)

#crée la librairie
add_library(game game.c game_aux.c queue.c game_tools.c)

#définit les bibliothèques utilisées
target_link_libraries(game_text game)
target_link_libraries(game_solve game)
target_link_libraries(game_test_amastouri game)
target_link_libraries(game_test_khorvath game)
target_link_libraries(game_test_marbeites game)
target_link_libraries(game_test_alleymarie game)

## copy useful data files in build dir
foreach(file "Walkway_Bold.ttf" "level1.tnt" "level2.tnt" "level3.tnt" "level4.tnt" "level5.tnt" "level6.tnt" "level7.tnt" "level8.tnt" "level9.tnt" "level10.tnt" "tree.png" "water.png" "happy_trump.jpg" "background.jpg" "Calibri.ttf" "angry_trump.jpg" "losing_water.png" "restart.png" "redo.png" "undo.png" "solve.png")
  file(COPY ${file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(file)

#Tests alleymarie
add_test(test_alleymarie_game_print ./game_test_alleymarie game_print)
add_test(test_alleymarie_game_default ./game_test_alleymarie game_default)
add_test(test_alleymarie_game_default_solution ./game_test_alleymarie game_default_solution)
add_test(test_alleymarie_game_new ./game_test_alleymarie game_new)
add_test(test_alleymarie_game_new_empty ./game_test_alleymarie game_new_empty)
add_test(test_alleymarie_game_copy ./game_test_alleymarie game_copy)

#Tests amastouri
add_test(test_amastouri_game_get_square ./game_test_amastouri game_get_square)
add_test(test_amastouri_game_set_square ./game_test_amastouri game_set_square)
add_test(test_amastouri_game_equal ./game_test_amastouri game_equal)
add_test(test_amastouri_game_delete ./game_test_amastouri game_delete)
add_test(test_amastouri_game_set_expected_nb_tents_row ./game_test_amastouri game_set_expected_nb_tents_row)
add_test(test_amastouri_game_set_expected_nb_tents_col ./game_test_amastouri game_set_expected_nb_tents_col)
add_test(test_amastouri_game_undo ./game_test_amastouri game_undo)
add_test(test_amastouri_game_redo ./game_test_amastouri game_redo)

#Tests marbeites
add_test(test_marbeites_game_get_expected_nb_tents_row ./game_test_marbeites game_get_expected_nb_tents_row)
add_test(test_marbeites_game_get_expected_nb_tents_col ./game_test_marbeites game_get_expected_nb_tents_col)
add_test(test_marbeites_game_get_expected_nb_tents_all ./game_test_marbeites game_get_expected_nb_tents_all)
add_test(test_marbeites_game_get_current_nb_tents_row ./game_test_marbeites game_get_current_nb_tents_row)
add_test(test_marbeites_game_get_current_nb_tents_col ./game_test_marbeites game_get_current_nb_tents_col)
add_test(test_marbeites_game_get_current_nb_tents_all ./game_test_marbeites game_get_current_nb_tents_all)
add_test(test_marbeites_game_nb_rows ./game_test_marbeites game_nb_rows)
add_test(test_marbeites_game_nb_cols ./game_test_marbeites game_nb_cols)
add_test(test_marbeites_game_is_wrapping ./game_test_marbeites game_is_wrapping)
add_test(test_marbeites_game_is_diagadj ./game_test_marbeites game_is_diagadj)

#Tests khorvath
add_test(test_khorvath_game_play_move ./game_test_khorvath game_play_move)
add_test(test_khorvath_game_check_move ./game_test_khorvath game_check_move)
add_test(test_khorvath_game_is_over ./game_test_khorvath game_is_over)
add_test(test_khorvath_game_fill_grass_row ./game_test_khorvath game_fill_grass_row)
add_test(test_khorvath_game_fill_grass_col ./game_test_khorvath game_fill_grass_col)
add_test(test_khorvath_game_restart ./game_test_khorvath game_restart)
add_test(test_khorvath_game_new_ext ./game_test_khorvath game_new_ext)
add_test(test_khorvath_game_new_empty_ext ./game_test_khorvath game_new_empty_ext)
add_test(test_khorvath_game_load ./game_test_khorvath game_load)
add_test(test_khorvath_game_save ./game_test_khorvath game_save)
add_test(test_khorvath_game_solve ./game_test_khorvath game_solve)
