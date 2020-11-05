#version minimale de cmake nécessaire
cmake_minimum_required(VERSION 2.6)

#nom du projet
project(game_text C)

enable_testing()

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
add_axecutable(game_test_alleymarie game_test_alleymarie.c)

#créer la librairie
add_library(game game.o game_aux.c)

#définit les bibliothèques utilisées
target_link_libraries(game_text game)

add_test(test_marbeites_dummy ./game_test_marbeites dummy)
add_test(test_khorvath_dummy ./game_test_khorvath dummy)
add_test(test_amastouri_dummy ./game_test_amastouri dummy)
add_test(test_alleymarie_dummy ./game_test_alleymarie dummy)
