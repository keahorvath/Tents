#include "graphic_mode.h"
#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

#define MAX_GAME_SIZE 20
/* home screen */
#define HOME_SCREEN "images/home_screen.png"
#define HOME_SCREEN_VERT "images/home_screen_small.png"
#define HELP_BUTTON "buttons/help.png"
#define PLAY_BUTTON "buttons/play.png"
#define EXIT_BUTTON "buttons/exit.png"

/*help screen */
#define HELP_SCREEN "images/help_screen.png"
#define HELP_SCREEN_VERT "images/help_screen_small.png"
#define BACK_BUTTON "buttons/back_button.png"

/* game screen */
#define GAME_SCREEN "images/game_screen.jpg"
#define GAME_SCREEN_VERT "images/background_small.png"
#define PALM_TREE "images/tree.png"
#define WATER "images/water.png"
#define RAFT "images/happy_trump.jpg"
#define L_WATER "images/losing_water.png"
#define L_RAFT "images/angry_trump.jpg"
#define HOME_BUTTON "buttons/home_button.png"
#define HELP_BUTTON_J "buttons/help_button_j.png"
#define UNDO "buttons/undo.png"
#define REDO "buttons/redo.png"
#define RESTART "buttons/restart.png"
#define SOLVE "buttons/solve.png"
#define FONT "Walkway_Bold.ttf"
#define FONT_LEVEL "Roboto-Regular.ttf"
#define SWITCH_1 "images/switch1.png"
#define SWITCH_2 "images/switch2.png"

#define LEVEL_SIZE 50
#define FONT_RATIO 0.7  // ratio of font size to cell size
#define BUTTON_SIZE 30
#define FONT_SIZE 16
#define TEXT_COLOR \
  { 0, 0, 0, 255 }
#define ABOVE_GRID_RATIO 0.2
#define LEFT_FROM_GRID_RATIO 0.03

/* game_over and end screens */
#define END_SCREEN "images/end_screen.jpg"
#define END_SCREEN_VERT "images/end_screen_small.png"
#define GAME_OVER_SCREEN "images/game_over_screen.jpg"
#define GAME_OVER_SCREEN_VERT "images/game_over_screen_small.png"
#define NEXT_LEVEL "buttons/next_level.png"
#define PREVIOUS_LEVEL "buttons/previous_level.png"
#define RESTART_GAMEOVER "buttons/restart_game_over.png"
#define QUIT_GAMEOVER "buttons/quit_button.png"

#define BUTTON_WIDTH 1 / 8    // button width according to window width
#define BUTTON_HEIGHT 1 / 16  // button height according to window width

/* **************************************************************** */
static void create_tents_text(SDL_Window *win, SDL_Renderer *ren, Env *env);
static void create_level_text(SDL_Window *win, SDL_Renderer *ren, Env *env);
static bool mouse_is_in_grid(Env *env, int x, int y);
static void render_home(SDL_Window *win, SDL_Renderer *ren, Env *env);
static void render_help(SDL_Window *win, SDL_Renderer *ren, Env *env);
static void render_game(SDL_Window *win, SDL_Renderer *ren, Env *env);
static void render_game_over(SDL_Window *win, SDL_Renderer *ren, Env *env);
static void render_end(SDL_Window *win, SDL_Renderer *ren, Env *env);
static bool process_home(SDL_Window *win, SDL_Renderer *ren, Env *env,
                         SDL_Event *e);
static bool process_help(SDL_Window *win, SDL_Renderer *ren, Env *env,
                         SDL_Event *e);
static bool process_game(SDL_Window *win, SDL_Renderer *ren, Env *env,
                         SDL_Event *e);
static bool process_game_over(SDL_Window *win, SDL_Renderer *ren, Env *env,
                              SDL_Event *e);
static bool process_end(SDL_Window *win, SDL_Renderer *ren, Env *env,
                        SDL_Event *e);

typedef enum {
  HOME = 0,      /* main menu screen  */
  GAME = 1,      /* in game screen */
  HELP = 2,      /* help (rules) screen */
  GAME_OVER = 3, /* game_over screen */
  END = 4        /* end screen */
} screen;

struct Env_t {
  screen current_screen;
  screen previous_screen;
  uint current_level;

  SDL_Texture *home_screen;
  SDL_Texture *home_screen_vert;
  SDL_Texture *help_button;
  SDL_Texture *play_button;
  SDL_Texture *exit_button;

  SDL_Texture *help_screen;
  SDL_Texture *help_screen_vert;
  SDL_Texture *back_button;

  SDL_Texture *game_screen;
  SDL_Texture *game_screen_vert;
  bool restricted_by_height;
  int grid_width;
  SDL_Texture *tree;
  SDL_Texture *water;
  SDL_Texture *raft;
  SDL_Texture *losing_water;
  SDL_Texture *losing_raft;
  SDL_Texture *undo;
  SDL_Texture *redo;
  SDL_Texture *restart;
  SDL_Texture *solve;
  int small_button_size;
  SDL_Texture *home_button;
  SDL_Texture *help_button_j;
  int side_button_size;
  SDL_Texture *current_level_text;
  float level_ratio;
  SDL_Texture **text;
  SDL_Texture *wrapping_text;
  SDL_Texture *diagadj_text;
  float wrap_diag_ratio;
  int grid_beginning_x;
  int grid_beginning_y;
  int cell_size;
  bool switch_button;
  SDL_Texture *switch_1;
  SDL_Texture *switch_2;
  SDL_Texture *game_over_screen;
  SDL_Texture *game_over_screen_vert;
  SDL_Texture *end_screen;
  SDL_Texture *end_screen_vert;
  SDL_Texture *next_level_button;
  SDL_Texture *previous_level_button;
  SDL_Texture *restart_game_over_button;
  SDL_Texture *quit_button;
  DList games;
  uint max_game_rows_reached, max_game_cols_reached;
  game g;
};

#ifdef __ANDROID__
static void copy_asset(char *src, char *dst) {
  SDL_RWops *file = SDL_RWFromFile(src, "r");
  if (!file) ERROR("[ERROR] SDL_RWFromFile: %s\n", src);
  int size = SDL_RWsize(file);
  PRINT("copy file %s (%d bytes) into %s\n", src, size, dst);
  char *buf = (char *)malloc(size + 1);
  if (!buf) ERROR("[ERROR] malloc\n");
  int r = SDL_RWread(file, buf, 1, size);
  PRINT("read %d\n", r);
  if (r != size) ERROR("[ERROR] fail to read all file (%d bytes)\n", r);
  FILE *out = fopen(dst, "w+");
  if (!out) ERROR("[ERROR] fail to create file %s\n", dst);
  int w = fwrite(buf, 1, r, out);
  if (r != w) ERROR("[ERROR] fail to write all file (%d bytes)\n", w);
  fclose(out);
  SDL_RWclose(file);
  free(buf);
}
#endif

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));
  // create the game
  if (argc == 2) {
    env->g = game_load(argv[1]);
    if (env->g == NULL) {
      fprintf(stderr, "File couldn't open!\n");
      exit(EXIT_FAILURE);
    }
    if (game_nb_rows(env->g) > MAX_GAME_SIZE ||
        game_nb_cols(env->g) > MAX_GAME_SIZE) {
      fprintf(stderr, "Game is too big!\n");
      exit(EXIT_FAILURE);
    }
  } else if (argc == 1) {
    env->g = game_default();
  } else {
    fprintf(stderr, "Wrong number of arguments!\n");
    exit(EXIT_FAILURE);
  }
  env->max_game_cols_reached = game_nb_cols(env->g);
  env->max_game_rows_reached = game_nb_rows(env->g);

  /* home screen textures */
  env->home_screen = IMG_LoadTexture(ren, HOME_SCREEN);
  if (!env->home_screen) ERROR("IMG_LoadTexture: %s\n", HOME_SCREEN);
  env->home_screen_vert = IMG_LoadTexture(ren, HOME_SCREEN_VERT);
  if (!env->home_screen_vert) ERROR("IMG_LoadTexture: %s\n", HOME_SCREEN_VERT);
  env->help_button = IMG_LoadTexture(ren, HELP_BUTTON);
  if (!env->help_button) ERROR("IMG_LoadTexture: %s\n", HELP_BUTTON);
  env->play_button = IMG_LoadTexture(ren, PLAY_BUTTON);
  if (!env->play_button) ERROR("IMG_LoadTexture: %s\n", PLAY_BUTTON);
  env->exit_button = IMG_LoadTexture(ren, EXIT_BUTTON);
  if (!env->exit_button) ERROR("IMG_LoadTexture: %s\n", EXIT_BUTTON);

  /* help screen textures */
  env->help_screen = IMG_LoadTexture(ren, HELP_SCREEN);
  if (!env->help_screen) ERROR("IMG_LoadTexture: %s\n", HELP_SCREEN);
  env->help_screen_vert = IMG_LoadTexture(ren, HELP_SCREEN_VERT);
  if (!env->help_screen_vert) ERROR("IMG_LoadTexture: %s\n", HELP_SCREEN_VERT);
  env->back_button = IMG_LoadTexture(ren, BACK_BUTTON);
  if (!env->back_button) ERROR("IMG_LoadTexture: %s\n", BACK_BUTTON);

  /* game screen textures */
  env->home_button = IMG_LoadTexture(ren, HOME_BUTTON);
  if (!env->home_button) ERROR("IMG_LoadTexture: %s\n", HOME_BUTTON);
  env->help_button_j = IMG_LoadTexture(ren, HELP_BUTTON_J);
  if (!env->help_button_j) ERROR("IMG_LoadTexture: %s\n", HELP_BUTTON_J);
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
  env->game_screen = IMG_LoadTexture(ren, GAME_SCREEN);
  if (!env->game_screen) ERROR("IMG_LoadTexture: %s\n", GAME_SCREEN);
  env->game_screen_vert = IMG_LoadTexture(ren, GAME_SCREEN_VERT);
  if (!env->game_screen_vert) ERROR("IMG_LoadTexture: %s\n", GAME_SCREEN_VERT);
  env->solve = IMG_LoadTexture(ren, SOLVE);
  if (!env->solve) ERROR("IMG_LoadTexture: %s\n", SOLVE);
  env->restart = IMG_LoadTexture(ren, RESTART);
  if (!env->restart) ERROR("IMG_LoadTexture: %s\n", RESTART);
  env->undo = IMG_LoadTexture(ren, UNDO);
  if (!env->undo) ERROR("IMG_LoadTexture: %s\n", UNDO);
  env->redo = IMG_LoadTexture(ren, REDO);
  if (!env->redo) ERROR("IMG_LoadTexture: %s\n", REDO);

  /* game_over and end screens */
  env->game_over_screen = IMG_LoadTexture(ren, GAME_OVER_SCREEN);
  if (!env->game_over_screen) ERROR("IMG_LoadTexture: %s\n", GAME_OVER_SCREEN);
  env->game_over_screen_vert = IMG_LoadTexture(ren, GAME_OVER_SCREEN_VERT);
  if (!env->game_over_screen_vert)
    ERROR("IMG_LoadTexture: %s\n", GAME_OVER_SCREEN_VERT);
  env->end_screen = IMG_LoadTexture(ren, END_SCREEN);
  if (!env->end_screen) ERROR("IMG_LoadTexture: %s\n", END_SCREEN);
  env->end_screen_vert = IMG_LoadTexture(ren, END_SCREEN_VERT);
  if (!env->end_screen_vert) ERROR("IMG_LoadTexture: %s\n", END_SCREEN_VERT);
  env->next_level_button = IMG_LoadTexture(ren, NEXT_LEVEL);
  if (!env->next_level_button) ERROR("IMG_LoadTexture: %s\n", NEXT_LEVEL);
  env->previous_level_button = IMG_LoadTexture(ren, PREVIOUS_LEVEL);
  if (!env->previous_level_button)
    ERROR("IMG_LoadTexture: %s\n", PREVIOUS_LEVEL);
  env->restart_game_over_button = IMG_LoadTexture(ren, RESTART_GAMEOVER);
  if (!env->restart_game_over_button)
    ERROR("IMG_LoadTexture: %s\n", RESTART_GAMEOVER);
  env->quit_button = IMG_LoadTexture(ren, QUIT_GAMEOVER);
  if (!env->restart_game_over_button)
    ERROR("IMG_LoadTexture: %s\n", QUIT_GAMEOVER);

  env->current_level = 0;
  env->current_screen = HOME;
  env->previous_screen = HOME;
  env->restricted_by_height = true;
  env->switch_button = true;
  env->switch_1 = IMG_LoadTexture(ren, SWITCH_1);
  if (!env->switch_1) ERROR("IMG_LoadTexture: %s\n", SWITCH_1);
  env->switch_2 = IMG_LoadTexture(ren, SWITCH_2);
  if (!env->switch_2) ERROR("IMG_LoadTexture: %s\n", SWITCH_2);

  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  /* level text */
  create_level_text(win, ren, env);

  /* nb_tents text */
  env->text =
      (SDL_Texture **)malloc(sizeof(SDL_Texture *) * (MAX_GAME_SIZE * 2));
  create_tents_text(win, ren, env);

  /* wrapping and diagadj text */
  SDL_Color color = TEXT_COLOR;  // black
  TTF_Font *font = TTF_OpenFont(FONT_LEVEL, FONT_SIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
  if (game_is_wrapping(env->g)) {
    SDL_Surface *surf = TTF_RenderText_Blended(
        font, "WRAPPING : ON", color);  // blended rendering for ultra nice text
    env->wrapping_text = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
  } else {
    SDL_Surface *surf =
        TTF_RenderText_Blended(font, "WRAPPING : OFF",
                               color);  // blended rendering for ultra nice text
    env->wrapping_text = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
  }
  SDL_Rect rect;
  SDL_QueryTexture(env->wrapping_text, NULL, NULL, &rect.w, &rect.h);
  env->wrap_diag_ratio = rect.w / rect.h;
  if (game_is_diagadj(env->g)) {
    SDL_Surface *surf = TTF_RenderText_Blended(
        font, "DIAGADJ : ON", color);  // blended rendering for ultra nice text
    env->diagadj_text = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
  } else {
    SDL_Surface *surf = TTF_RenderText_Blended(
        font, "DIAGADJ : OFF", color);  // blended rendering for ultra nice text
    env->diagadj_text = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
  }
  TTF_CloseFont(font);
  char *levels[] = {"games/level10.tnt", "games/level9.tnt", "games/level8.tnt",
                    "games/level7.tnt",  "games/level6.tnt", "games/level5.tnt",
                    "games/level4.tnt",  "games/level3.tnt", "games/level2.tnt",
                    "games/level1.tnt"};
#ifdef __ANDROID__
  char *levels_android[] = {
      "level10.tnt", "level9.tnt", "level8.tnt", "level7.tnt", "level6.tnt",
      "level5.tnt",  "level4.tnt", "level3.tnt", "level2.tnt", "level1.tnt"};
#endif

  env->games = dlist_create_empty();
  for (uint i = 0; i < 10; i++) {
#ifdef __ANDROID__
    env->games = dlist_prepend(env->games, levels_android[i]);
#else
    env->games = dlist_prepend(env->games, levels[i]);
#endif
  }
  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren,
            Env *env) { /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
  if (env->current_screen == HOME) {
    render_home(win, ren, env);
  } else if (env->current_screen == GAME) {
    render_game(win, ren, env);
  } else if (env->current_screen == HELP) {
    render_help(win, ren, env);
  } else if (env->current_screen == GAME_OVER) {
    render_game_over(win, ren, env);
  } else if (env->current_screen == END) {
    render_end(win, ren, env);
  }
}

/* **************************************************************** */
void render_home(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;
  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE);
  if (w > h) {
    SDL_RenderCopy(ren, env->home_screen, NULL, NULL);
    rect.w = (int)(w * BUTTON_WIDTH);
    rect.h = (int)(w * BUTTON_HEIGHT);
    rect.y = (int)(h - h * 1 / 5);
    // play button
    rect.x = (int)(w * 1 / 10);
    SDL_RenderCopy(ren, env->play_button, NULL, &rect);
    // help button
    rect.x = (int)(w * 3 / 10);
    SDL_RenderCopy(ren, env->help_button, NULL, &rect);
    // exit button
    rect.x = (int)(w * 7.5 / 10);
    SDL_RenderCopy(ren, env->exit_button, NULL, &rect);
  } else {
    SDL_RenderCopy(ren, env->home_screen_vert, NULL, NULL);
    rect.w = (int)(w * 1 / 3);
    rect.h = (int)(h * 1 / 12);
    rect.x = (int)(w * 1 / 12);
    // play button
    rect.y = (int)(h * 9 / 12);
    SDL_RenderCopy(ren, env->play_button, NULL, &rect);
    // help button
    rect.y = (int)(h * 9 / 12 + 1.5 * rect.h);
    SDL_RenderCopy(ren, env->help_button, NULL, &rect);
    // exit button
    rect.x = (int)(w * 7 / 12);
    SDL_RenderCopy(ren, env->exit_button, NULL, &rect);
  }
}

void render_help(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  if (w > h) {
    SDL_RenderCopy(ren, env->help_screen, NULL, NULL);
    // back button
    rect.w = w * BUTTON_WIDTH;
    rect.h = w * BUTTON_HEIGHT;
    rect.x = (int)(w * 2 / 3);
    rect.y = (int)(h * 4 / 5);
    SDL_RenderCopy(ren, env->back_button, NULL, &rect);
  } else {
    SDL_RenderCopy(ren, env->help_screen_vert, NULL, NULL);
    // back button
    rect.w = w * 1 / 3;
    rect.h = w * 1 / 6;
    rect.x = (int)(w * 1 / 3);
    rect.y = (int)(h - 1.2 * rect.h);
    SDL_RenderCopy(ren, env->back_button, NULL, &rect);
  }
}

void render_game(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;

  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE);
  if (w > h) {
    SDL_RenderCopy(ren, env->game_screen, NULL, NULL);
  } else {
    SDL_RenderCopy(ren, env->game_screen_vert, NULL, NULL);
  }

  uint space_avail_per_cell_x =
      (int)((w - w * 2 * LEFT_FROM_GRID_RATIO) / (game_nb_cols(env->g) + 1));
  uint space_avail_per_cell_y =
      (int)((h - h * ABOVE_GRID_RATIO) / (game_nb_rows(env->g) + 1));
  if (space_avail_per_cell_x > space_avail_per_cell_y) {
    env->restricted_by_height = true;
    env->cell_size =
        (int)((1 - ABOVE_GRID_RATIO) * h / (game_nb_rows(env->g) + 1));
    env->grid_beginning_x =
        w / 2 - env->cell_size * (game_nb_cols(env->g) + 1) / 2;
    env->grid_beginning_y = (int)(ABOVE_GRID_RATIO * h);
  } else {
    env->restricted_by_height = false;
    env->cell_size =
        (int)((w - w * LEFT_FROM_GRID_RATIO) / (game_nb_cols(env->g) + 1));
    if (h / 2 - env->cell_size * (game_nb_rows(env->g) + 1) / 2 <
        h * ABOVE_GRID_RATIO) {
      env->grid_beginning_y = h - env->cell_size * (game_nb_rows(env->g) + 3);
    } else {
      env->grid_beginning_y =
          h / 2 - env->cell_size * (game_nb_rows(env->g) + 1) / 2;
    }
    env->grid_beginning_x = (int)(w * LEFT_FROM_GRID_RATIO);
  }
  env->grid_width = env->cell_size * game_nb_cols(env->g);
  /* render current level text */
  if (env->restricted_by_height) {
    rect.h = (int)(ABOVE_GRID_RATIO * h / 3);
    rect.w = (int)(rect.h * env->level_ratio);
  } else {
    rect.h = (int)(env->grid_beginning_y / 3);
    rect.w = (int)(rect.h * env->level_ratio);
    if (rect.w > env->grid_width * 3 / 4) {
      rect.w = (int)(env->grid_width * 3 / 4);
      rect.h = (int)rect.w / env->level_ratio;
    }
  }
  rect.x = (int)(w / 2 - rect.w / 2);
  rect.y = (int)(rect.h / 2);
  SDL_RenderCopy(ren, env->current_level_text, NULL, &rect);

  /* render grid background */
  rect.x = env->grid_beginning_x;
  rect.y = env->grid_beginning_y;
  rect.w = env->grid_width;
  rect.h = env->cell_size * game_nb_rows(env->g);
  SDL_RenderFillRect(ren, &rect);

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
  rect.w = (int)(FONT_RATIO / 2 * env->cell_size);
  rect.h = (int)(FONT_RATIO * env->cell_size);
  for (uint i = 0; i < (game_nb_rows(env->g)); i++) {
    rect.x = env->grid_beginning_x + env->grid_width +
             (int)((1 - FONT_RATIO / 2) / 2 * env->cell_size);
    rect.y = env->grid_beginning_y + env->cell_size * i +
             (int)((1 - FONT_RATIO) / 2 * env->cell_size);

    SDL_RenderCopy(ren, env->text[i], NULL, &rect);
  }
  for (uint j = 0; j < (game_nb_cols(env->g)); j++) {
    rect.y = env->grid_beginning_y + game_nb_rows(env->g) * env->cell_size +
             (int)((1 - FONT_RATIO) / 2 * env->cell_size);
    rect.x = env->grid_beginning_x + env->cell_size * j +
             (int)((1 - FONT_RATIO / 2) / 2 * env->cell_size);
    SDL_RenderCopy(ren, env->text[j + game_nb_rows(env->g)], NULL, &rect);
  }

#ifdef __ANDROID__
  /*render buttons*/
  rect.w = (int)(env->grid_width * 1 / 10);
  rect.h = (int)(env->grid_width * 1 / 10);
  env->small_button_size = rect.w;
  rect.y = env->grid_beginning_y - (int)(rect.w + rect.w * 1 / 4);
  // undo
  rect.x = (int)(w * 1 / 10);
  SDL_RenderCopy(ren, env->undo, NULL, &rect);
  // redo
  rect.x = (int)(w * 3 / 10);
  SDL_RenderCopy(ren, env->redo, NULL, &rect);
  // restart
  rect.x = (int)(w * 6 / 10);
  SDL_RenderCopy(ren, env->restart, NULL, &rect);
  // solve
  rect.x = (int)(w * 8 / 10);
  SDL_RenderCopy(ren, env->solve, NULL, &rect);
  // render switch
  rect.w = (int)(w * 1 / 3);
  rect.h = (int)(w * 1 / 6);
  rect.x = (int)(w * 1 / 3);
  rect.y = (int)(h - 1.5 * rect.h);
  if (env->switch_button) {
    SDL_RenderCopy(ren, env->switch_1, NULL, &rect);
  } else {
    SDL_RenderCopy(ren, env->switch_2, NULL, &rect);
  }
#else
  /*render wrapping and diagadj texts*/
  rect.w = (int)(env->grid_width / 4);
  rect.h = (int)(rect.w / env->wrap_diag_ratio);
  rect.x = env->grid_beginning_x;
  rect.y = (int)(env->grid_beginning_y - rect.h * 2.4);
  SDL_RenderCopy(ren, env->wrapping_text, NULL, &rect);
  rect.x = env->grid_beginning_x;
  rect.y = (int)(env->grid_beginning_y - rect.h * 1.3);
  SDL_RenderCopy(ren, env->diagadj_text, NULL, &rect);

  /*render buttons*/
  rect.w = (int)(env->grid_width * 1 / 14);
  rect.h = (int)(env->grid_width * 1 / 14);
  env->small_button_size = rect.w;
  rect.y = env->grid_beginning_y - (int)(rect.w + rect.w * 1 / 4);
  // undo
  rect.x = env->grid_beginning_x + env->grid_width / 2 - (int)(1.5 * rect.w);
  SDL_RenderCopy(ren, env->undo, NULL, &rect);
  // redo
  rect.x = env->grid_beginning_x + env->grid_width / 2 + (int)(0.5 * rect.w);
  SDL_RenderCopy(ren, env->redo, NULL, &rect);
  // restart
  rect.x = env->grid_beginning_x + env->grid_width - (int)(3.5 * rect.w);
  SDL_RenderCopy(ren, env->restart, NULL, &rect);
  // solve
  rect.x = env->grid_beginning_x + env->grid_width - (int)(1.5 * rect.w);
  SDL_RenderCopy(ren, env->solve, NULL, &rect);
#endif
  /* render home and help buttons */
  if (env->restricted_by_height) {
    rect.w = (int)w / 15;
    rect.h = (int)w / 15;
  } else {
    rect.w = (int)h / 15;
    rect.h = (int)h / 15;
  }
  env->side_button_size = rect.w;
  rect.x = (int)rect.w * 0.5;
  rect.y = (int)(h - 1.5 * rect.w);
  SDL_RenderCopy(ren, env->home_button, NULL, &rect);

  rect.x = (int)(w - rect.w * 1.5);
  SDL_RenderCopy(ren, env->help_button_j, NULL, &rect);
}

void render_game_over(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;

  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE);
  if (w > h) {
    SDL_RenderCopy(ren, env->game_over_screen, NULL, NULL);

    rect.w = (int)(w * BUTTON_WIDTH);
    rect.h = (int)(w * BUTTON_HEIGHT);
    rect.y = (int)(h * 3 / 10);
    if (env->current_level == 0 || env->current_level == 1) {
      // restart button
      rect.x = (int)(w * 2 / 8);
      SDL_RenderCopy(ren, env->restart_game_over_button, NULL, &rect);
      // exit button
      rect.x = (int)(w * 5 / 8);
      SDL_RenderCopy(ren, env->quit_button, NULL, &rect);
      // next level button
      rect.w = (int)(w * (1.5 * BUTTON_WIDTH));
      rect.x = (int)(w * 3.25 / 8);
      SDL_RenderCopy(ren, env->next_level_button, NULL, &rect);
    } else {
      // restart button
      rect.x = (int)(w * 1 / 8);
      SDL_RenderCopy(ren, env->restart_game_over_button, NULL, &rect);
      // exit button
      rect.x = (int)(w * 6 / 8);
      SDL_RenderCopy(ren, env->quit_button, NULL, &rect);
      rect.w = (int)(w / 6);
      // previous level button
      rect.x = (int)(w * 2.5 / 8);
      SDL_RenderCopy(ren, env->previous_level_button, NULL, &rect);
      // next button
      rect.x = (int)(w * 4.25 / 8);
      SDL_RenderCopy(ren, env->next_level_button, NULL, &rect);
    }
  } else {
    SDL_RenderCopy(ren, env->game_over_screen_vert, NULL, NULL);
    rect.w = (int)(w * 1 / 4);
    rect.h = (int)(h * 1 / 12);
    rect.y = (int)(h * 8 / 20);
    // restart button
    rect.x = (int)(w * 7 / 36);
    SDL_RenderCopy(ren, env->restart_game_over_button, NULL, &rect);
    // exit button
    rect.x = (int)(w * 10 / 18);
    SDL_RenderCopy(ren, env->quit_button, NULL, &rect);
    rect.w = (int)(w * 1 / 3);
    rect.y = (int)(h * 1 / 4);
    if (env->current_level == 0 || env->current_level == 1) {
      // next level button
      rect.x = (int)(w * 1 / 3);
      SDL_RenderCopy(ren, env->next_level_button, NULL, &rect);
    } else {
      // previous level button
      rect.x = (int)(w * 1 / 9);
      SDL_RenderCopy(ren, env->previous_level_button, NULL, &rect);
      // next button
      rect.x = (int)(w * 10 / 18);
      SDL_RenderCopy(ren, env->next_level_button, NULL, &rect);
    }
  }
}

void render_end(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;

  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white */
  if (w > h) {
    SDL_RenderCopy(ren, env->end_screen, NULL, NULL);
    rect.w = (int)(w * BUTTON_WIDTH);
    rect.h = (int)(w * BUTTON_HEIGHT);
    rect.y = (int)(h * 3 / 10);
    // restart button
    rect.x = (int)(w * 2 / 8);
    SDL_RenderCopy(ren, env->restart_game_over_button, NULL, &rect);
    // exit button
    rect.x = (int)(w * 5.5 / 8);
    SDL_RenderCopy(ren, env->quit_button, NULL, &rect);
    // previous level button
    rect.w = (int)(w * 2 * BUTTON_WIDTH);
    rect.x = (int)(w * 3.25 / 8);
    SDL_RenderCopy(ren, env->previous_level_button, NULL, &rect);
  } else {
    SDL_RenderCopy(ren, env->end_screen_vert, NULL, NULL);
    rect.w = (int)(w * 1 / 4);
    rect.h = (int)(h * 1 / 12);
    rect.y = (int)(h * 7 / 20);
    // restart button
    rect.x = (int)(w * 7 / 36);
    SDL_RenderCopy(ren, env->restart_game_over_button, NULL, &rect);
    // exit button
    rect.x = (int)(w * 10 / 18);
    SDL_RenderCopy(ren, env->quit_button, NULL, &rect);
    rect.w = (int)(w * 1 / 3);
    rect.y = (int)(h * 1 / 4);
    // previous level button
    rect.x = (int)(w * 1 / 3);
    SDL_RenderCopy(ren, env->previous_level_button, NULL, &rect);
  }
}

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }
  if (env->current_screen == HOME) {
    return process_home(win, ren, env, e);
  } else if (env->current_screen == GAME) {
    return process_game(win, ren, env, e);
  } else if (env->current_screen == HELP) {
    return process_help(win, ren, env, e);
  } else if (env->current_screen == GAME_OVER) {
    return process_game_over(win, ren, env, e);
  } else if (env->current_screen == END) {
    return process_end(win, ren, env, e);
  }
  return true;
}

bool process_home(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
#ifdef __ANDROID__
  if (e->type == SDL_FINGERDOWN) {
    // check if mouse is pressing one of the buttons
    // start game
    if (e->tfinger.x * w < w * 1 / 3 + w * 1 / 12 &&
        e->tfinger.x * w > w * 1 / 12 &&
        e->tfinger.y * h < h * 1 / 12 + h * 9 / 12 &&
        e->tfinger.y * h > h * 9 / 12) {
      env->current_screen = GAME;
      env->previous_screen = HOME;
      return false;
    } else if (e->tfinger.x * w < w * 1 / 3 + w * 1 / 12 &&
               e->tfinger.x * w > w * 1 / 12 &&
               e->tfinger.y * h < h * 1 / 12 + h * 9 / 12 + 1.5 * h * 1 / 12 &&
               e->tfinger.y * h > h * 9 / 12 + 1.5 * h * 1 / 12) {
      env->current_screen = HELP;
      env->previous_screen = HOME;
      return false;
    } else if (e->tfinger.x * w < w * 1 / 3 + w * 7 / 12 &&
               e->tfinger.x * w > w * 7 / 12 &&
               e->tfinger.y * h < h * 1 / 12 + h * 9 / 12 + 1.5 * h * 1 / 12 &&
               e->tfinger.y * h > h * 9 / 12 + 1.5 * h * 1 / 12) {
      return true;
    }
  }
#else
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // check if mouse is pressing one of the buttons
    // start game
    if (w > h) {
      if (mouse.x < w * 1 / 10 + w * BUTTON_WIDTH && mouse.x > w * 1 / 10 &&
          mouse.y < h * 4 / 5 + w * BUTTON_HEIGHT && mouse.y > h * 4 / 5) {
        env->current_screen = GAME;
        env->previous_screen = HOME;
        return false;
      } else if (mouse.x < w * 3 / 10 + w * BUTTON_WIDTH &&
                 mouse.x > w * 3 / 10 &&
                 mouse.y < h * 4 / 5 + w * BUTTON_HEIGHT &&
                 mouse.y > h * 4 / 5) {
        env->current_screen = HELP;
        env->previous_screen = HOME;
        return false;
      } else if (mouse.x < w * 7.5 / 10 + w * BUTTON_WIDTH &&
                 mouse.x > (w * 7.5 / 10) &&
                 mouse.y < h * 4 / 5 + w * BUTTON_HEIGHT &&
                 mouse.y > h * 4 / 5) {
        return true;
      }
    } else {
      if (mouse.x < w * 1 / 3 + w * 1 / 12 && mouse.x > w * 1 / 12 &&
          mouse.y < h * 1 / 12 + h * 9 / 12 && mouse.y > h * 9 / 12) {
        env->current_screen = GAME;
        env->previous_screen = HOME;
        return false;
      } else if (mouse.x < w * 1 / 3 + w * 1 / 12 && mouse.x > w * 1 / 12 &&
                 mouse.y < h * 1 / 12 + h * 9 / 12 + 1.5 * h * 1 / 12 &&
                 mouse.y > h * 9 / 12 + 1.5 * h * 1 / 12) {
        env->current_screen = HELP;
        env->previous_screen = HOME;
        return false;
      } else if (mouse.x < w * 1 / 3 + w * 7 / 12 && mouse.x > w * 7 / 12 &&
                 mouse.y < h * 1 / 12 + h * 9 / 12 + 1.5 * h * 1 / 12 &&
                 mouse.y > h * 9 / 12 + 1.5 * h * 1 / 12) {
        return true;
      }
    }
  }
#endif
  return false;
}

bool process_help(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    if (w > h) {
      // check if mouse is pressing one of the buttons
      // start game
      if (mouse.x < w * 2 / 3 + w * BUTTON_WIDTH && mouse.x > w * 2 / 3 &&
          mouse.y < h * 4 / 5 + w * BUTTON_HEIGHT && mouse.y > h * 4 / 5) {
        env->current_screen = env->previous_screen;
        env->previous_screen = HELP;
        return false;
      }
    } else {
      // check if mouse is pressing one of the buttons
      // start game
      if (mouse.x < w * 2 / 3 && mouse.x > w * 1 / 3 &&
          mouse.y < h - 1.2 * w * 1 / 6 + w * 1 / 6 &&
          mouse.y > h - 1.2 * w * 1 / 6) {
        env->current_screen = env->previous_screen;
        env->previous_screen = HELP;
        return false;
      }
    }
  }
  return false;
}

bool process_game(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (game_is_over(env->g)) {
    render(win, ren, env);
    SDL_RenderPresent(ren);
    SDL_Delay(600);
    if (env->current_level == 10) {
      env->current_screen = END;
      return false;
    } else {
      env->current_screen = GAME_OVER;
    }
  }
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
#ifdef __ANDROID__
  if (e->type == SDL_FINGERDOWN) {
    // check if mouse is pressing one of the buttons
    // start game
    if (e->tfinger.x * w < w * 2 / 10 && e->tfinger.x * w > w * 1 / 10 &&
        e->tfinger.y * h <
            w * 1 / 10 + env->grid_beginning_y - w * 1 / 10 * 5 / 4 &&
        e->tfinger.y * h > env->grid_beginning_y - w * 1 / 10 * 5 / 4) {
      game_undo(env->g);
    }
    // redo the last move
    else if (e->tfinger.x * w < w * 4 / 10 && e->tfinger.x * w > w * 3 / 10 &&
             e->tfinger.y * h <
                 w * 1 / 10 + env->grid_beginning_y - w * 1 / 10 * 5 / 4 &&
             e->tfinger.y * h > env->grid_beginning_y - w * 1 / 10 * 5 / 4) {
      game_redo(env->g);
    }
    // restart the game from the beginning
    else if (e->tfinger.x * w < w * 7 / 10 && e->tfinger.x * w > w * 6 / 10 &&
             e->tfinger.y * h <
                 w * 1 / 10 + env->grid_beginning_y - w * 1 / 10 * 5 / 4 &&
             e->tfinger.y * h > env->grid_beginning_y - w * 1 / 10 * 5 / 4) {
      game_restart(env->g);
    }
    // solve the game
    else if (e->tfinger.x * w < w * 9 / 10 && e->tfinger.x * w > w * 8 / 10 &&
             e->tfinger.y * h <
                 w * 1 / 10 + env->grid_beginning_y - w * 1 / 10 * 5 / 4 &&
             e->tfinger.y * h > env->grid_beginning_y - w * 1 / 10 * 5 / 4) {
      for (uint i = 0; i < game_nb_rows(env->g); i++) {
        for (uint j = 0; j < game_nb_cols(env->g); j++) {
          if (game_get_square(env->g, i, j) == TENT ||
              game_get_square(env->g, i, j) == GRASS) {
            game_set_square(env->g, i, j, EMPTY);
          }
        }
      }
      game_solve(env->g);
    }
    // switch button press
    else if (e->tfinger.x * w < w * 1 / 2 + w * 1 / 6 &&
             e->tfinger.x * w > w * 1 / 2 &&
             e->tfinger.y * h < w * 1 / 6 + h - 1.5 * w * 1 / 6 &&
             e->tfinger.y * h > h - 1.5 * w * 1 / 6) {
      if (env->switch_button) {
        env->switch_button = false;
      }
    } else if (e->tfinger.x * w < w * 1 / 2 &&
               e->tfinger.x * w > w * 1 / 2 - w * 1 / 6 &&
               e->tfinger.y * h < w * 1 / 6 + h - 1.5 * w * 1 / 6 &&
               e->tfinger.y * h > h - 1.5 * w * 1 / 6) {
      if (!env->switch_button) {
        env->switch_button = true;
      }
    }
    // go to the home page
    if (e->tfinger.x * w > env->side_button_size * 0.5 &&
        e->tfinger.x * w < env->side_button_size * 1.5) {
      if (e->tfinger.y * h > (h - 1.5 * env->side_button_size) &&
          e->tfinger.y * h < (h - 0.5 * env->side_button_size)) {
        env->current_screen = HOME;
        env->previous_screen = GAME;
      }
    }
    // go to help page
    if (e->tfinger.x * w > (w - env->side_button_size * 1.5) &&
        e->tfinger.x * w < (w - env->side_button_size * 0.5)) {
      if (e->tfinger.y * h > (h - 1.5 * env->side_button_size) &&
          e->tfinger.y * h < (h - 0.5 * env->side_button_size)) {
        env->current_screen = HELP;
        env->previous_screen = GAME;
      }
    }
    // convert mouse position to cell in grid
    if (mouse_is_in_grid(env, (int)(e->tfinger.x * w),
                         (int)(e->tfinger.y * h))) {
      uint row, col;
      row = (uint)(e->tfinger.y * h - env->grid_beginning_y) / env->cell_size;
      col = (uint)(e->tfinger.x * w - env->grid_beginning_x) / env->cell_size;
      if (env->switch_button == true) {
        if (game_get_square(env->g, row, col) == TENT ||
            game_get_square(env->g, row, col) == GRASS) {
          game_play_move(env->g, row, col, EMPTY);
        } else if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, TENT);
        }
      } else {
        if (game_get_square(env->g, row, col) == TENT ||
            game_get_square(env->g, row, col) == GRASS) {
          game_play_move(env->g, row, col, EMPTY);
        } else if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, GRASS);
        }
      }
    }
  } else if (e->type == SDL_FINGERMOTION) {
    if (mouse_is_in_grid(env, (int)(e->tfinger.x * w),
                         (int)(e->tfinger.y * h))) {
      uint row, col;
      row = (uint)(e->tfinger.y * h - env->grid_beginning_y) / env->cell_size;
      col = (uint)(e->tfinger.x * w - env->grid_beginning_x) / env->cell_size;
      if (env->switch_button == true) {
        if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, TENT);
        }
      } else {
        if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, GRASS);
        }
      }
    }
  }
#else
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // check if mouse is pressing one of the buttons
    // undo the last move
    if (mouse.x > env->grid_beginning_x + env->grid_width / 2 -
                      (1.5 * env->small_button_size) &&
        mouse.x < env->grid_beginning_x + env->grid_width / 2 -
                      (0.5 * env->small_button_size)) {
      if (mouse.y > env->grid_beginning_y - (env->small_button_size * 5 / 4) &&
          mouse.y < env->grid_beginning_y - (env->small_button_size * 1 / 4)) {
        game_undo(env->g);
      }
    }
    // redo the last move
    if (mouse.x > env->grid_beginning_x + env->grid_width / 2 +
                      (0.5 * env->small_button_size) &&
        mouse.x < env->grid_beginning_x + env->grid_width / 2 +
                      (1.5 * env->small_button_size)) {
      if (mouse.y > env->grid_beginning_y - (env->small_button_size * 5 / 4) &&
          mouse.y < env->grid_beginning_y - (env->small_button_size * 1 / 4)) {
        game_redo(env->g);
      }
    }
    // restart the game from the beginning
    if (mouse.x > env->grid_beginning_x + env->grid_width -
                      (3.5 * env->small_button_size) &&
        mouse.x < env->grid_beginning_x + env->grid_width -
                      (2.5 * env->small_button_size)) {
      if (mouse.y > env->grid_beginning_y - (env->small_button_size * 5 / 4) &&
          mouse.y < env->grid_beginning_y - (env->small_button_size * 1 / 4)) {
        game_restart(env->g);
      }
    }
    // solve the game
    if (mouse.x > env->grid_beginning_x + env->grid_width -
                      (1.5 * env->small_button_size) &&
        mouse.x < env->grid_beginning_x + env->grid_width -
                      (0.5 * env->small_button_size)) {
      if (mouse.y > env->grid_beginning_y - (env->small_button_size * 5 / 4) &&
          mouse.y < env->grid_beginning_y - (env->small_button_size * 1 / 4)) {
        for (uint i = 0; i < game_nb_rows(env->g); i++) {
          for (uint j = 0; j < game_nb_cols(env->g); j++) {
            if (game_get_square(env->g, i, j) == TENT ||
                game_get_square(env->g, i, j) == GRASS) {
              game_set_square(env->g, i, j, EMPTY);
            }
          }
        }
        game_solve(env->g);
      }
    }
    // go to the home page
    if (mouse.x > env->side_button_size * 0.5 &&
        mouse.x < env->side_button_size * 1.5) {
      if (mouse.y > (h - 1.5 * env->side_button_size) &&
          mouse.y < (h - 0.5 * env->side_button_size)) {
        env->current_screen = HOME;
        env->previous_screen = GAME;
      }
    }
    // go to help page
    if (mouse.x > (w - env->side_button_size * 1.5) &&
        mouse.x < (w - env->side_button_size * 0.5)) {
      if (mouse.y > (h - 1.5 * env->side_button_size) &&
          mouse.y < (h - 0.5 * env->side_button_size)) {
        env->current_screen = HELP;
        env->previous_screen = GAME;
      }
    }
    // convert mouse position to cell in grid
    if (mouse_is_in_grid(env, mouse.x, mouse.y)) {
      uint row, col;
      row = (uint)(mouse.y - env->grid_beginning_y) / env->cell_size;
      col = (uint)(mouse.x - env->grid_beginning_x) / env->cell_size;
      if (e->button.button == SDL_BUTTON_LEFT) {
        if (game_get_square(env->g, row, col) == TENT ||
            game_get_square(env->g, row, col) == GRASS) {
          game_play_move(env->g, row, col, EMPTY);
        } else if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, TENT);
        }
      } else if (e->button.button == SDL_BUTTON_RIGHT) {
        if (game_get_square(env->g, row, col) == TENT ||
            game_get_square(env->g, row, col) == GRASS) {
          game_play_move(env->g, row, col, EMPTY);
        } else if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, GRASS);
        }
      }
    }
  } else if (e->type == SDL_MOUSEMOTION) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    if (mouse_is_in_grid(env, mouse.x, mouse.y)) {
      uint row, col;
      row = (uint)(mouse.y - env->grid_beginning_y) / env->cell_size;
      col = (uint)(mouse.x - env->grid_beginning_x) / env->cell_size;
      if (e->button.button == SDL_BUTTON_LEFT) {
        if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, TENT);
        }
      } else if (e->button.button ==
                 SDL_BUTTON(SDL_BUTTON_RIGHT)) {  // bug with SDL_BUTTON_RIGHT
                                                  // on its own
        if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, GRASS);
        }
      }
    }
  }
  // allow to undo or redo the last move by pressing the corresponding key on
  // the board
  else if (e->type == SDL_KEYDOWN) {
    if (e->key.keysym.sym == SDLK_u) {
      game_undo(env->g);
    }
    if (e->key.keysym.sym == SDLK_r) {
      game_redo(env->g);
    }
  }
#endif
  return false;
}

bool process_game_over(SDL_Window *win, SDL_Renderer *ren, Env *env,
                       SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
#ifdef __ANDROID__
  if (e->type == SDL_FINGERDOWN) {
    SDL_Point mouse;
    if (env->current_level == 0 || env->current_level == 1) {
      // player presses restart button
      if ((w > h && (e->tfinger.x * w > w * 2 / 8 &&
                     e->tfinger.x * w < w * 2 / 8 + w * BUTTON_WIDTH &&
                     e->tfinger.y * h > h * 3 / 10 &&
                     e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
          (w <= h && (e->tfinger.x * w > w * 7 / 36 &&
                      e->tfinger.x * w < w * 7 / 36 + w * 1 / 4 &&
                      e->tfinger.y * h > h * 8 / 20 &&
                      e->tfinger.y * h < h * 8 / 20 + h * 1 / 12))) {
        game_restart(env->g);
        env->current_screen = GAME;
        return false;
      }
      // player presses next level button
      else if ((w > h &&
                (e->tfinger.x * w > w * 3.25 / 8 &&
                 e->tfinger.x * w < w * 3.25 / 8 + w * (1.5 * BUTTON_WIDTH) &&
                 e->tfinger.y * h > h * 3 / 10 &&
                 e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h &&
                (e->tfinger.x * w > w * 1 / 3 && e->tfinger.x * w < w * 2 / 3 &&
                 e->tfinger.y * h > h * 1 / 4 &&
                 e->tfinger.y * h < h * 1 / 4 + h * 1 / 12))) {
        game_delete(env->g);
        if (env->current_level == 1) {
          env->games = dlist_next(env->games);
        }
        env->current_level++;
        char *level = dlist_data(env->games);
#ifdef __ANDROID__
        const char *dir = SDL_AndroidGetInternalStoragePath();
        char filename[1024];
        sprintf(filename, "%s/%s", dir, level);
        copy_asset(level, filename);
        env->g = game_load(filename);
#else
        env->g = game_load(level);
#endif
        create_level_text(win, ren, env);
        create_tents_text(win, ren, env);
        if (game_nb_cols(env->g) > env->max_game_cols_reached) {
          env->max_game_cols_reached = game_nb_cols(env->g);
        }
        if (game_nb_rows(env->g) > env->max_game_rows_reached) {
          env->max_game_rows_reached = game_nb_rows(env->g);
        }
        env->current_screen = GAME;
        return false;
      }
      // player presses quit button
      else if ((w > h && (e->tfinger.x * w > w * 5 / 8 &&
                          e->tfinger.x * w < w * 5 / 8 + w * BUTTON_WIDTH &&
                          e->tfinger.y * h > h * 3 / 10 &&
                          e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h && (e->tfinger.x * w > w * 10 / 18 &&
                           e->tfinger.x * w < w * 10 / 18 + w * 1 / 4 &&
                           e->tfinger.y * h > h * 8 / 20 &&
                           e->tfinger.y * h < h * 8 / 20 + h * 1 / 12))) {
        return true;
      }
    } else {
      // player presses restart button
      if ((w > h && (e->tfinger.x * w > w * 1 / 8 &&
                     e->tfinger.x * w < w * 1 / 8 + w * BUTTON_WIDTH &&
                     e->tfinger.y * h > h * 3 / 10 &&
                     e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
          (w <= h && (e->tfinger.x * w > w * 7 / 36 &&
                      e->tfinger.x * w < w * 7 / 36 + w * 1 / 4 &&
                      e->tfinger.y * h > h * 8 / 20 &&
                      e->tfinger.y * h < h * 8 / 20 + h * 1 / 12))) {
        game_restart(env->g);
        env->current_screen = GAME;
        return false;
      }
      // player presses previous level button
      else if ((w > h && (e->tfinger.x * w > w * 2.5 / 8 &&
                          e->tfinger.x * w < w * 2.5 / 8 + w * 1 / 6 &&
                          e->tfinger.y * h > h * 3 / 10 &&
                          e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h && (e->tfinger.x * w > w * 1 / 9 &&
                           e->tfinger.x * w < w * 1 / 9 + w * 1 / 3 &&
                           e->tfinger.y * h > h * 1 / 4 &&
                           e->tfinger.y * h < h * 1 / 4 + h * 1 / 12))) {
        game_delete(env->g);
        env->games = dlist_prev(env->games);
        env->current_level--;
        char *level = dlist_data(env->games);
#ifdef __ANDROID__
        const char *dir = SDL_AndroidGetInternalStoragePath();
        char filename[1024];
        sprintf(filename, "%s/%s", dir, level);
        copy_asset(level, filename);
        env->g = game_load(filename);
#else
        env->g = game_load(level);
#endif
        create_level_text(win, ren, env);
        create_tents_text(win, ren, env);
        env->current_screen = GAME;
        return false;
      }
      // player presses next level button
      else if ((w > h && (e->tfinger.x * w > w * 4.25 / 8 &&
                          e->tfinger.x * w < w * 4.25 / 8 + w * 1 / 6 &&
                          e->tfinger.y * h > h * 3 / 10 &&
                          e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h && (e->tfinger.x * w > w * 10 / 18 &&
                           e->tfinger.x * w < w * 10 / 18 + w * 1 / 3 &&
                           e->tfinger.y * h > h * 1 / 4 &&
                           e->tfinger.y * h < h * 1 / 4 + h * 1 / 12))) {
        game_delete(env->g);
        env->games = dlist_next(env->games);
        env->current_level++;
        char *level = dlist_data(env->games);
#ifdef __ANDROID__
        const char *dir = SDL_AndroidGetInternalStoragePath();
        char filename[1024];
        sprintf(filename, "%s/%s", dir, level);
        copy_asset(level, filename);
        env->g = game_load(filename);
#else
        env->g = game_load(level);
#endif
        create_level_text(win, ren, env);
        create_tents_text(win, ren, env);
        if (game_nb_cols(env->g) > env->max_game_cols_reached) {
          env->max_game_cols_reached = game_nb_cols(env->g);
        }
        if (game_nb_rows(env->g) > env->max_game_rows_reached) {
          env->max_game_rows_reached = game_nb_rows(env->g);
        }
        env->current_screen = GAME;
        return false;
      }
      // player presses quit button
      else if ((w > h && (e->tfinger.x * w > w * 6 / 8 &&
                          e->tfinger.x * w < w * 5.75 / 8 + w * BUTTON_WIDTH &&
                          e->tfinger.y * h > h * 3 / 10 &&
                          e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h && (e->tfinger.x * w > w * 10 / 18 &&
                           e->tfinger.x * w < w * 10 / 18 + w * 1 / 4 &&
                           e->tfinger.y * h > h * 8 / 20 &&
                           e->tfinger.y * h < h * 8 / 20 + h * 1 / 12))) {
        return true;
      }
    }
  }
#else
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    if (env->current_level == 0 || env->current_level == 1) {
      // player presses restart button
      if ((w > h &&
           (mouse.x > w * 2 / 8 && mouse.x < w * 2 / 8 + w * BUTTON_WIDTH &&
            mouse.y > h * 3 / 10 &&
            mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
          (w <= h &&
           (mouse.x > w * 7 / 36 && mouse.x < w * 7 / 36 + w * 1 / 4 &&
            mouse.y > h * 8 / 20 && mouse.y < h * 8 / 20 + h * 1 / 12))) {
        game_restart(env->g);
        env->current_screen = GAME;
        return false;
      }
      // player presses next level button
      else if ((w > h && (mouse.x > w * 3.25 / 8 &&
                          mouse.x < w * 3.25 / 8 + w * (1.5 * BUTTON_WIDTH) &&
                          mouse.y > h * 3 / 10 &&
                          mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h &&
                (mouse.x > w * 1 / 3 && mouse.x < w * 2 / 3 &&
                 mouse.y > h * 1 / 4 && mouse.y < h * 1 / 4 + h * 1 / 12))) {
        game_delete(env->g);
        if (env->current_level == 1) {
          env->games = dlist_next(env->games);
        }
        env->current_level++;
        char *level = dlist_data(env->games);
#ifdef __ANDROID__
        const char *dir = SDL_AndroidGetInternalStoragePath();
        char filename[1024];
        sprintf(filename, "%s/%s", dir, level);
        copy_asset(level, filename);
        env->g = game_load(filename);
#else
        env->g = game_load(level);
#endif
        create_level_text(win, ren, env);
        create_tents_text(win, ren, env);
        if (game_nb_cols(env->g) > env->max_game_cols_reached) {
          env->max_game_cols_reached = game_nb_cols(env->g);
        }
        if (game_nb_rows(env->g) > env->max_game_rows_reached) {
          env->max_game_rows_reached = game_nb_rows(env->g);
        }
        env->current_screen = GAME;
        return false;
      }
      // player presses quit button
      else if ((w > h && (mouse.x > w * 5 / 8 &&
                          mouse.x < w * 5 / 8 + w * BUTTON_WIDTH &&
                          mouse.y > h * 3 / 10 &&
                          mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h &&
                (mouse.x > w * 10 / 18 && mouse.x < w * 10 / 18 + w * 1 / 4 &&
                 mouse.y > h * 8 / 20 && mouse.y < h * 8 / 20 + h * 1 / 12))) {
        return true;
      }
    } else {
      // player presses restart button
      if ((w > h &&
           (mouse.x > w * 1 / 8 && mouse.x < w * 1 / 8 + w * BUTTON_WIDTH &&
            mouse.y > h * 3 / 10 &&
            mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
          (w <= h &&
           (mouse.x > w * 7 / 36 && mouse.x < w * 7 / 36 + w * 1 / 4 &&
            mouse.y > h * 8 / 20 && mouse.y < h * 8 / 20 + h * 1 / 12))) {
        game_restart(env->g);
        env->current_screen = GAME;
        return false;
      }
      // player presses previous level button
      else if ((w > h &&
                (mouse.x > w * 2.5 / 8 && mouse.x < w * 2.5 / 8 + w * 1 / 6 &&
                 mouse.y > h * 3 / 10 &&
                 mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h &&
                (mouse.x > w * 1 / 9 && mouse.x < w * 1 / 9 + w * 1 / 3 &&
                 mouse.y > h * 1 / 4 && mouse.y < h * 1 / 4 + h * 1 / 12))) {
        game_delete(env->g);
        env->games = dlist_prev(env->games);
        env->current_level--;
        char *level = dlist_data(env->games);
#ifdef __ANDROID__
        const char *dir = SDL_AndroidGetInternalStoragePath();
        char filename[1024];
        sprintf(filename, "%s/%s", dir, level);
        copy_asset(level, filename);
        env->g = game_load(filename);
#else
        env->g = game_load(level);
#endif
        create_level_text(win, ren, env);
        create_tents_text(win, ren, env);
        env->current_screen = GAME;
        return false;
      }
      // player presses next level button
      else if ((w > h &&
                (mouse.x > w * 4.25 / 8 && mouse.x < w * 4.25 / 8 + w * 1 / 6 &&
                 mouse.y > h * 3 / 10 &&
                 mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h &&
                (mouse.x > w * 10 / 18 && mouse.x < w * 10 / 18 + w * 1 / 3 &&
                 mouse.y > h * 1 / 4 && mouse.y < h * 1 / 4 + h * 1 / 12))) {
        game_delete(env->g);
        env->games = dlist_next(env->games);
        env->current_level++;
        char *level = dlist_data(env->games);
#ifdef __ANDROID__
        const char *dir = SDL_AndroidGetInternalStoragePath();
        char filename[1024];
        sprintf(filename, "%s/%s", dir, level);
        copy_asset(level, filename);
        env->g = game_load(filename);
#else
        env->g = game_load(level);
#endif
        create_level_text(win, ren, env);
        create_tents_text(win, ren, env);
        if (game_nb_cols(env->g) > env->max_game_cols_reached) {
          env->max_game_cols_reached = game_nb_cols(env->g);
        }
        if (game_nb_rows(env->g) > env->max_game_rows_reached) {
          env->max_game_rows_reached = game_nb_rows(env->g);
        }
        env->current_screen = GAME;
        return false;
      }
      // player presses quit button
      else if ((w > h && (mouse.x > w * 6 / 8 &&
                          mouse.x < w * 5.75 / 8 + w * BUTTON_WIDTH &&
                          mouse.y > h * 3 / 10 &&
                          mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
               (w <= h &&
                (mouse.x > w * 10 / 18 && mouse.x < w * 10 / 18 + w * 1 / 4 &&
                 mouse.y > h * 8 / 20 && mouse.y < h * 8 / 20 + h * 1 / 12))) {
        return true;
      }
    }
  }
#endif
  return false;
}

bool process_end(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
#ifdef __ANDROID__
  if (e->type == SDL_FINGERDOWN) {
    SDL_Point mouse;
    // check if mouse is pressing one of the buttons
    // player presses restart button
    if ((w > h && (e->tfinger.x * w > w * 2 / 8 &&
                   e->tfinger.x * w < w * 2 / 8 + w * BUTTON_WIDTH &&
                   e->tfinger.y * h > h * 3 / 10 &&
                   e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
        (w <= h && (e->tfinger.x * w > w * 7 / 36 &&
                    e->tfinger.x * w < w * 7 / 36 + w * 1 / 4 &&
                    e->tfinger.y * h > h * 7 / 20 &&
                    e->tfinger.y * h < h * 7 / 20 + h * 1 / 12))) {
      game_restart(env->g);
      env->current_screen = GAME;
      return false;
    }
    // player presses previous level button
    else if ((w > h &&
              (e->tfinger.x * w > w * 3.25 / 8 &&
               e->tfinger.x * w < w * 3.25 / 8 + w * 2 * BUTTON_WIDTH &&
               e->tfinger.y * h > h * 3 / 10 &&
               e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
             (w <= h &&
              (e->tfinger.x * w > w * 1 / 3 && e->tfinger.x * w < w * 2 / 3 &&
               e->tfinger.y * h > h * 1 / 4 &&
               e->tfinger.y * h < h * 1 / 4 + h * 1 / 12))) {
      game_delete(env->g);
      env->games = dlist_prev(env->games);
      env->current_level--;
      char *level = dlist_data(env->games);
#ifdef __ANDROID__
      const char *dir = SDL_AndroidGetInternalStoragePath();
      char filename[1024];
      sprintf(filename, "%s/%s", dir, level);
      copy_asset(level, filename);
      env->g = game_load(filename);
#else
      env->g = game_load(level);
#endif
      create_level_text(win, ren, env);
      create_tents_text(win, ren, env);
      env->current_screen = GAME;
      return false;
    }
    // player presses quit button
    else if ((w > h && (e->tfinger.x * w > w * 5.5 / 8 &&
                        e->tfinger.x * w < w * 5.5 / 8 + w * BUTTON_WIDTH &&
                        e->tfinger.y * h > h * 3 / 10 &&
                        e->tfinger.y * h < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
             (w <= h && (e->tfinger.x * w > w * 10 / 18 &&
                         e->tfinger.x * w < w * 10 / 18 + w * 1 / 4 &&
                         e->tfinger.y * h > h * 7 / 20 &&
                         e->tfinger.y * h < h * 7 / 20 + h * 1 / 12))) {
      return true;
    }
  }
#else
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // check if mouse is pressing one of the buttons
    // player presses restart button
    if ((w > h &&
         (mouse.x > w * 2 / 8 && mouse.x < w * 2 / 8 + w * BUTTON_WIDTH &&
          mouse.y > h * 3 / 10 && mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
        (w <= h &&
         (mouse.x > w * 7 / 36 && mouse.x < w * 7 / 36 + w * 1 / 4 &&
          mouse.y > h * 7 / 20 && mouse.y < h * 7 / 20 + h * 1 / 12))) {
      game_restart(env->g);
      env->current_screen = GAME;
      return false;
    }
    // player presses previous level button
    else if ((w > h && (mouse.x > w * 3.25 / 8 &&
                        mouse.x < w * 3.25 / 8 + w * 2 * BUTTON_WIDTH &&
                        mouse.y > h * 3 / 10 &&
                        mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
             (w <= h &&
              (mouse.x > w * 1 / 3 && mouse.x < w * 2 / 3 &&
               mouse.y > h * 1 / 4 && mouse.y < h * 1 / 4 + h * 1 / 12))) {
      game_delete(env->g);
      env->games = dlist_prev(env->games);
      env->current_level--;
      char *level = dlist_data(env->games);
#ifdef __ANDROID__
      const char *dir = SDL_AndroidGetInternalStoragePath();
      char filename[1024];
      sprintf(filename, "%s/%s", dir, level);
      copy_asset(level, filename);
      env->g = game_load(filename);
#else
      env->g = game_load(level);
#endif
      create_level_text(win, ren, env);
      create_tents_text(win, ren, env);
      env->current_screen = GAME;
      return false;
    }
    // player presses quit button
    else if ((w > h && (mouse.x > w * 5.5 / 8 &&
                        mouse.x < w * 5.5 / 8 + w * BUTTON_WIDTH &&
                        mouse.y > h * 3 / 10 &&
                        mouse.y < h * 3 / 10 + w * BUTTON_HEIGHT)) ||
             (w <= h &&
              (mouse.x > w * 10 / 18 && mouse.x < w * 10 / 18 + w * 1 / 4 &&
               mouse.y > h * 7 / 20 && mouse.y < h * 7 / 20 + h * 1 / 12))) {
      return true;
    }
  }
#endif
  return false;
}

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /*Clean all the textures of the game */

  SDL_DestroyTexture(env->home_screen);
  SDL_DestroyTexture(env->home_screen_vert);
  SDL_DestroyTexture(env->help_button);
  SDL_DestroyTexture(env->play_button);
  SDL_DestroyTexture(env->exit_button);

  SDL_DestroyTexture(env->help_screen);
  SDL_DestroyTexture(env->help_screen_vert);
  SDL_DestroyTexture(env->back_button);

  SDL_DestroyTexture(env->game_screen);
  SDL_DestroyTexture(env->game_screen_vert);
  SDL_DestroyTexture(env->tree);
  SDL_DestroyTexture(env->water);
  SDL_DestroyTexture(env->raft);
  SDL_DestroyTexture(env->losing_water);
  SDL_DestroyTexture(env->losing_raft);
  SDL_DestroyTexture(env->game_screen);
  SDL_DestroyTexture(env->undo);
  SDL_DestroyTexture(env->redo);
  SDL_DestroyTexture(env->restart);
  SDL_DestroyTexture(env->solve);
  SDL_DestroyTexture(env->home_button);
  SDL_DestroyTexture(env->help_button_j);
  SDL_DestroyTexture(env->current_level_text);

  for (uint i = 0; i < env->max_game_cols_reached + env->max_game_rows_reached;
       i++) {
    SDL_DestroyTexture(env->text[i]);
  }
  free(env->text);
  SDL_DestroyTexture(env->wrapping_text);
  SDL_DestroyTexture(env->diagadj_text);
  SDL_DestroyTexture(env->switch_1);
  SDL_DestroyTexture(env->switch_2);

  SDL_DestroyTexture(env->game_over_screen);
  SDL_DestroyTexture(env->game_over_screen_vert);
  SDL_DestroyTexture(env->end_screen);
  SDL_DestroyTexture(env->end_screen_vert);
  SDL_DestroyTexture(env->next_level_button);
  SDL_DestroyTexture(env->previous_level_button);
  SDL_DestroyTexture(env->restart_game_over_button);
  SDL_DestroyTexture(env->quit_button);

  while (dlist_next(env->games) != NULL) {
    dlist_delete_after(env->games, env->games);
  }
  while (dlist_prev(env->games) != NULL) {
    dlist_delete_before(env->games, env->games);
  }
  dlist_free(env->games);
  game_delete(env->g);
  free(env);
}
/* **************************************************************** */

bool mouse_is_in_grid(Env *env, int x, int y) {
  if (x < env->grid_beginning_x ||
      x > env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) ||
      y < env->grid_beginning_y ||
      y > env->grid_beginning_y + env->cell_size * game_nb_rows(env->g)) {
    return false;
  }
  return true;
}

void create_tents_text(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Color color = TEXT_COLOR; /* blue color in RGBA */
  TTF_Font *font = TTF_OpenFont(FONT, w);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
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
  TTF_CloseFont(font);
}

void create_level_text(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  int font_size = (int)(h * ABOVE_GRID_RATIO * 1 / 3);
  SDL_Color color = TEXT_COLOR;
  TTF_Font *font = TTF_OpenFont(FONT_LEVEL, font_size);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT_LEVEL);
  TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
  char level_text[10];
  sprintf(level_text, "LEVEL %u", env->current_level);
  SDL_Surface *surf = TTF_RenderText_Blended(font, level_text, color);
  env->current_level_text = SDL_CreateTextureFromSurface(ren, surf);
  TTF_CloseFont(font);
  SDL_QueryTexture(env->current_level_text, NULL, NULL, &rect.w, &rect.h);
  env->level_ratio = rect.w / rect.h;
}