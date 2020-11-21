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

#créer la librairie
add_library(libgame.a game.c game_aux.c)

#définit les bibliothèques utilisées
target_link_libraries(game_text libgame.a)
