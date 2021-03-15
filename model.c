// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"
#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "queue.h"

#define PALM_TREE "tree.png"
#define WATER "water.png"
#define RAFT "raft.jpg"
#define L_WATER "losing_water.png"
#define L_RAFT "losing_raft.jpg"
#define BACKGROUND "blue_background.jpg"
#define FONT "Calibri.ttf"
#define FONT_RATIO 0.8  // ratio of font size to cell size
#define GRID_RATIO 0.75 //ratio of grid size to window size

/* **************************************************************** */

struct Env_t {
  SDL_Texture *tree;
  SDL_Texture *water;
  SDL_Texture *raft;
  SDL_Texture *losing_water;
  SDL_Texture *losing_raft;
  SDL_Texture *background;
  SDL_Texture **text;
  SDL_Texture *game_over_text;
  SDL_Texture *text_undo;
  SDL_Texture *text_redo;
  int grid_beginning_x;
  int grid_beginning_y;
  int cell_size;
  game g;
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  PRINT("Left click to place a tent, right click to place water.\n");

  // create the game
  if (argc == 2) {
    env->g = game_load(argv[1]);
    if (env->g == NULL) {
      fprintf(stderr, "File couldn't open!\n");
      exit(EXIT_FAILURE);
    }
  } else if (argc == 1) {
    env->g = game_default();
  } else {
    fprintf(stderr, "Wrong number of arguments!\n");
    exit(EXIT_FAILURE);
  }

  env->tree = IMG_LoadTexture(ren, PALM_TREE);
  if (!env->tree) ERROR("IMG_LoadTexture: %s\n", PALM_TREE);
  env->water = IMG_LoadTexture(ren, WATER);
  if (!env->water) ERROR("IMG_LoadTexture: %s\n", WATER);
  env->raft = IMG_LoadTexture(ren, RAFT);
  if (!env->raft) ERROR("IMG_LoadTexture: %s\n", RAFT);
  env->losing_water = IMG_LoadTexture(ren, L_WATER);
  if (!env->losing_water) ERROR("IMG_LoadTexture: %s\n", L_WATER);
  env->losing_raft = IMG_LoadTexture(ren, L_RAFT);
  if (!env->losing_raft) ERROR("IMG_LoadTexture: %s\n", L_RAFT);
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  env->text = (SDL_Texture **)malloc(
      sizeof(SDL_Texture *) * (game_nb_cols(env->g) + game_nb_rows(env->g)));
  SDL_Color color = {255, 255, 255, 255}; /* blue color in RGBA */
  TTF_Font *font = TTF_OpenFont(FONT, w);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font,
                   TTF_STYLE_BOLD);  // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  char text_tents[2];
  for (uint i = 0; i < game_nb_rows(env->g); i++) {
    sprintf(text_tents, "%u", game_get_expected_nb_tents_row(env->g, i));
    SDL_Surface *surf = TTF_RenderText_Blended(
        font, text_tents, color);  // blended rendering for ultra nice text
    env->text[i] = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
  }
  for (uint j = 0; j < game_nb_cols(env->g); j++) {
    sprintf(text_tents, "%u", game_get_expected_nb_tents_col(env->g, j));
    SDL_Surface *surf = TTF_RenderText_Blended(
        font, text_tents, color);  // blended rendering for ultra nice text
    env->text[game_nb_rows(env->g) + j] =
        SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
  }
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);                                   // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  SDL_Surface* surf = TTF_RenderText_Blended(font, "Congratulations! Trump found his new home!", color);  // blended rendering for ultra nice text
  //SDL_Surface* undo = TTF_RenderText_Blended(font, "Undo", color);
  //SDL_Surface* redo = TTF_RenderText_Blended(front, "Redo", color);
  env->game_over_text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren,
            Env *env) { /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;
  /*SDL_Rect button_undo;
  button_undo.x = 0;
  button_undo.y = 0;
  button_undo.w = 1;
  button_undo.h = 1;
  SDL_Rect button_redo;
  button_redo.x = 0;
  button_redo.y = 0;
  button_undo.w = 0;
  button_undo.h = 0;*/
  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white */
  SDL_RenderCopy(ren, env->background, &rect, NULL);            /* stretch it */

  uint space_avail_per_cell_x = w / game_nb_cols(env->g);
  uint space_avail_per_cell_y = h / game_nb_rows(env->g);
  if (space_avail_per_cell_x > space_avail_per_cell_y) {
    env->cell_size = (int)(GRID_RATIO*h/ game_nb_rows(env->g));
    env->grid_beginning_x = w / 2 - env->cell_size * game_nb_cols(env->g) / 2;
    env->grid_beginning_y = (int)((1-GRID_RATIO)*h/2);
  } else {
    env->cell_size = (int)(GRID_RATIO*w/ game_nb_cols(env->g));
    env->grid_beginning_x = (int)((1-GRID_RATIO)*w/2);
    env->grid_beginning_y = h / 2 - env->cell_size * game_nb_rows(env->g) / 2;
  }

  rect.x = env->grid_beginning_x;
  rect.y = env->grid_beginning_y;
  rect.w = env->cell_size * game_nb_cols(env->g);
  rect.h = env->cell_size * game_nb_rows(env->g);
  SDL_RenderFillRect(ren, &rect);
  //SDL_RenderFillRect(ren, &button_undo);
  //SDL_RenderFillRect(ren, &button_redo);
  /* render the tents, water and trees */
  for (uint i = 0; i < game_nb_rows(env->g); i++) {
    for (uint j = 0; j < game_nb_cols(env->g); j++) {
      rect.x = env->grid_beginning_x + j * env->cell_size;
      rect.y = env->grid_beginning_y + i * env->cell_size;
      rect.w = env->cell_size;
      rect.h = env->cell_size;
      if (game_get_square(env->g, i, j) == TREE) {
        SDL_RenderCopy(ren, env->tree, NULL, &rect);
      } else if (game_get_square(env->g, i, j) == GRASS) {
        if (game_check_move(env->g, i, j, GRASS) == REGULAR) {
          SDL_RenderCopy(ren, env->water, NULL, &rect);
        } else if (game_check_move(env->g, i, j, GRASS) == LOSING) {
          SDL_RenderCopy(ren, env->losing_water, NULL, &rect);
        }
      } else if (game_get_square(env->g, i, j) == TENT) {
        if (game_check_move(env->g, i, j, TENT) == REGULAR) {
          SDL_RenderCopy(ren, env->raft, NULL, &rect);
        } else if (game_check_move(env->g, i, j, TENT) == LOSING) {
          SDL_RenderCopy(ren, env->losing_raft, NULL, &rect);
        }
      }
    }
  }

  /* render the grid */
  SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE); /* black */
  for (uint i = 0; i < game_nb_cols(env->g) + 1; i++) {
    SDL_RenderDrawLine(
        ren, env->grid_beginning_x + i * env->cell_size, env->grid_beginning_y,
        env->grid_beginning_x + i * env->cell_size,
        env->grid_beginning_y + game_nb_rows(env->g) * env->cell_size);
  }
  for (uint j = 0; j < game_nb_rows(env->g) + 1; j++) {
    SDL_RenderDrawLine(
        ren, env->grid_beginning_x, env->grid_beginning_y + j * env->cell_size,
        env->grid_beginning_x + game_nb_cols(env->g) * env->cell_size,
        env->grid_beginning_y + j * env->cell_size);
  }

  /*render the nb_tents text*/
  rect.w = (int)(FONT_RATIO/2*env->cell_size);
  rect.h = (int)(FONT_RATIO*env->cell_size);
  for (uint i = 0; i < (game_nb_rows(env->g)); i++) {
    rect.x = env->grid_beginning_x + game_nb_cols(env->g) * env->cell_size +
             (int)(FONT_RATIO/2*env->cell_size);
    rect.y = env->grid_beginning_y + env->cell_size * i + (int)((1-FONT_RATIO)/2*env->cell_size);

    SDL_RenderCopy(ren, env->text[i], NULL, &rect);
  }
  for (uint j = 0; j < (game_nb_cols(env->g)); j++) {
    rect.y = env->grid_beginning_y + game_nb_rows(env->g) * env->cell_size +
             (int)((1-FONT_RATIO)*env->cell_size);
    rect.x = env->grid_beginning_x + env->cell_size * j + (int)(FONT_RATIO/2*env->cell_size);
    SDL_RenderCopy(ren, env->text[j + game_nb_rows(env->g)], NULL, &rect);
  }

  /*render game_over*/
  if (game_is_over(env->g)){
    rect.w = w;
    rect.h = h / game_nb_rows(env->g);
    rect.x = w / 2 - rect.w / 2;
    rect.y = h / 2 - rect.h / 2;
    SDL_RenderCopy(ren, env->game_over_text, NULL, &rect);
  }
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // check if mouse is in the grid
    if (mouse.x < env->grid_beginning_x ||
        mouse.x >
            env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) ||
        mouse.y < env->grid_beginning_y ||
        mouse.y >
            env->grid_beginning_y + env->cell_size * game_nb_rows(env->g)) {
      return false;
    }
    SDL_MouseButtonEvent b = e->button;
    // convert mouse position to cell in grid
    uint row, col;
    row = (uint)(mouse.y - env->grid_beginning_y) / env->cell_size;
    col = (uint)(mouse.x - env->grid_beginning_x) / env->cell_size;
    if (b.button == SDL_BUTTON_LEFT) {
      if (game_get_square(env->g, row, col) == TENT ||
          game_get_square(env->g, row, col) == GRASS) {
        game_play_move(env->g, row, col, EMPTY);
      } else if (game_get_square(env->g, row, col) == EMPTY) {
        game_play_move(env->g, row, col, TENT);
      }
    } else if (b.button == SDL_BUTTON_RIGHT) {
      if (game_get_square(env->g, row, col) == TENT ||
          game_get_square(env->g, row, col) == GRASS) {
        game_play_move(env->g, row, col, EMPTY);
      } else if (game_get_square(env->g, row, col) == EMPTY) {
        game_play_move(env->g, row, col, GRASS);
      }
    }
  }
  /*if(mouse.x = button_undo.x && mouse.y = button_undo.y){
    game_undo();
  }
  if(mouse.x = button_redo.x && mouse.y = button_redo.y){
    game_redo();
  } */
  return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /* PUT YOUR CODE HERE TO CLEAN MEMORY */

  free(env);
}

/* **************************************************************** */
