// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"
#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
#include "queue.h"

#define GAMES_DIR "games"
#define PALM_TREE "tree.png"
#define WATER "water.png"
#define RAFT "happy_trump.jpg"
#define L_WATER "losing_water.png"
#define L_RAFT "angry_trump.jpg"
#define BACKGROUND "background.jpg"
#define MAIN_MENU "main_menu.jpg"
#define HELP_BUTTON "help.png"
#define PLAY_BUTTON "play.png"
#define EXIT_BUTTON "exit.png"
#define MENU_BUTTON "home_button.png"
#define HELP_BUTTON_J "help_button_j.png"
#define UNDO "undo.png"
#define REDO "redo.png"
#define RESTART "restart.png"
#define SOLVE "solve.png"
#define FONT "Walkway_Oblique_Black.ttf"
#define FONT_NIVEAU "Eastman-Grotesque-Bold-Italic-trial.otf"
#define FONT_RATIO 0.7   // ratio of font size to cell size
#define GRID_RATIO 0.75  // ratio of grid size to window size
#define BUTTON_SIZE 30
#define FONT_SIZE 16
#define TEXT_COLOR {65, 65, 143, 255}

/* **************************************************************** */
static void initialize_tents_text(SDL_Window *win, SDL_Renderer *ren, Env *env);
static bool mouse_is_in_grid(Env *env, int x, int y);
static bool final_message_box (SDL_Window *win ,Env *env );
static void render_menu(SDL_Window *win, SDL_Renderer *ren, Env *env);
static void render_game(SDL_Window *win, SDL_Renderer *ren, Env *env);
static bool process_menu(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e);
static bool process_game(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e);
typedef enum {
  MENU = 0, /**< main menu screen  */
  GAME = 1,  /**< in game screen */
  HELP = 2,  /**< help (rules) screen */
} screen;

struct Env_t {
  screen current_screen;
  SDL_Texture *main_menu;
  SDL_Texture *help_button;
  SDL_Texture *play_button;
  SDL_Texture *exit_button;
  SDL_Texture *menu_button;
  SDL_Texture *help_button_j;
  SDL_Texture *tree;
  SDL_Texture *water;
  SDL_Texture *raft;
  SDL_Texture *losing_water;
  SDL_Texture *losing_raft;
  SDL_Texture *background;
  SDL_Texture **text;
  SDL_Texture *undo;
  SDL_Texture *redo;
  SDL_Texture *restart;
  SDL_Texture *solve;
  SDL_Texture *wrapping_text;
  SDL_Texture *diagadj_text;
  int grid_beginning_x;
  int grid_beginning_y;
  int cell_size;
  queue *games;
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

  env->main_menu = IMG_LoadTexture(ren, MAIN_MENU);
  if (!env->main_menu) ERROR("IMG_LoadTexture: %s\n", MAIN_MENU);
  env->help_button = IMG_LoadTexture(ren, HELP_BUTTON);
  if (!env->help_button) ERROR("IMG_LoadTexture: %s\n", HELP_BUTTON);
  env->play_button = IMG_LoadTexture(ren, PLAY_BUTTON);
  if (!env->play_button) ERROR("IMG_LoadTexture: %s\n", PLAY_BUTTON);
  env->exit_button = IMG_LoadTexture(ren, EXIT_BUTTON);
  if (!env->exit_button) ERROR("IMG_LoadTexture: %s\n", EXIT_BUTTON);
  env->menu_button = IMG_LoadTexture(ren, MENU_BUTTON);
  if (!env->menu_button) ERROR("IMG_LoadTexture: %s\n", MENU_BUTTON);
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
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);
  env->solve = IMG_LoadTexture(ren, SOLVE);
  if (!env->solve) ERROR("IMG_LoadTexture: %s\n", SOLVE);
  env->restart = IMG_LoadTexture(ren, RESTART);
  if (!env->restart) ERROR("IMG_LoadTexture: %s\n", RESTART);
  env->undo = IMG_LoadTexture(ren, UNDO);
  if (!env->undo) ERROR("IMG_LoadTexture: %s\n", UNDO);
  env->redo = IMG_LoadTexture(ren, REDO);
  if (!env->redo) ERROR("IMG_LoadTexture: %s\n", REDO);

  env->current_screen = MENU;
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  env->text = (SDL_Texture **)malloc(
      sizeof(SDL_Texture *) * (game_nb_cols(env->g) + game_nb_rows(env->g)));
  initialize_tents_text(win, ren, env);

  /* render wrapping and diagadj text */
  SDL_Color color = {0, 0, 0, 255};  // black
  TTF_Font *font = TTF_OpenFont(FONT, FONT_SIZE);
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

  env->games = queue_new();
  queue_push_head(env->games, "level1.tnt");
  queue_push_head(env->games, "level2.tnt");
  queue_push_head(env->games, "level3.tnt");
  queue_push_head(env->games, "level4.tnt");
  queue_push_head(env->games, "level5.tnt");
  queue_push_head(env->games, "level6.tnt");
  queue_push_head(env->games, "level7.tnt");
  queue_push_head(env->games, "level8.tnt");
  queue_push_head(env->games, "level9.tnt");
  queue_push_head(env->games, "level10.tnt");
  return env;
}

/* **************************************************************** */

void render(SDL_Window *win, SDL_Renderer *ren,
            Env *env) { /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
  if (env->current_screen == MENU){
    render_menu(win, ren, env);
  }else if (env->current_screen == GAME){
    render_game(win, ren, env);
  }
    /*
  }else if (env->current_screen == HELP){
    render_help(win, ren, env);
  }
*/
}

/* **************************************************************** */
void render_menu(SDL_Window *win, SDL_Renderer *ren, Env *env){
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;
  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white */
  SDL_RenderCopy(ren, env->main_menu, NULL, NULL);            /* stretch it */

  rect.w = (int)(w/12) ;
  rect.h = (int)(w/12);
  rect.y = (int)(h - h*1/5);
  // play button
  rect.x = (int)(w - w *9/10);
  SDL_RenderCopy(ren, env->play_button, NULL, &rect);
  // help button
  rect.x = (int)(w - w *7/10);
  SDL_RenderCopy(ren, env->help_button, NULL, &rect);
  // exit button
  SDL_QueryTexture(env->exit_button, NULL, NULL, &rect.w, &rect.h);
  rect.w = w/6;
  rect.h = h/10;
  rect.y = (int)(h - h*1/6);
  rect.x = (int)(w - w*2.5/10);
  SDL_RenderCopy(ren, env->exit_button, NULL, &rect);
}

void render_game(SDL_Window *win, SDL_Renderer *ren, Env *env){
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;
  /* render background texture */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white */
  SDL_RenderCopy(ren, env->background, NULL, NULL);            /* stretch it */

  uint space_avail_per_cell_x = w / game_nb_cols(env->g);
  uint space_avail_per_cell_y = h / game_nb_rows(env->g);
  if (space_avail_per_cell_x > space_avail_per_cell_y) {
    env->cell_size = (int)(GRID_RATIO * h / (game_nb_rows(env->g) + 1));
    env->grid_beginning_x =
        w / 2 - env->cell_size * (game_nb_cols(env->g) + 1) / 2;
    env->grid_beginning_y = (int)((1 - GRID_RATIO) * h / 2);
  } else {
    env->cell_size = (int)(GRID_RATIO * w / (game_nb_cols(env->g) + 1));
    env->grid_beginning_x = (int)((1 - GRID_RATIO) * w / 2);
    env->grid_beginning_y =
        h / 2 - env->cell_size * (game_nb_rows(env->g) + 1) / 2;
  }

  rect.x = env->grid_beginning_x;
  rect.y = env->grid_beginning_y;
  rect.w = env->cell_size * game_nb_cols(env->g);
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
    rect.x = env->grid_beginning_x + game_nb_cols(env->g) * env->cell_size +
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

  /*render buttons*/
  rect.w = BUTTON_SIZE;
  rect.h = BUTTON_SIZE;
  rect.y = env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5);
  // undo
  rect.x = env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) / 2 -
           (int)(1.5 * BUTTON_SIZE);
  SDL_RenderCopy(ren, env->undo, NULL, &rect);
  // redo
  rect.x = env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) / 2 +
           (int)(0.5 * BUTTON_SIZE);
  SDL_RenderCopy(ren, env->redo, NULL, &rect);
  // restart
  rect.x = env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) -
           (int)(3.5 * BUTTON_SIZE);
  SDL_RenderCopy(ren, env->restart, NULL, &rect);
  // solve
  rect.x = env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) -
           (int)(1.5 * BUTTON_SIZE);
  SDL_RenderCopy(ren, env->solve, NULL, &rect);

  /*render wrapping and diagadj texts*/
  SDL_QueryTexture(env->wrapping_text, NULL, NULL, &rect.w, &rect.h);
  rect.x = env->grid_beginning_x;
  rect.y = env->grid_beginning_y - (int)(3 * rect.h);
  SDL_RenderCopy(ren, env->wrapping_text, NULL, &rect);
  SDL_QueryTexture(env->diagadj_text, NULL, NULL, &rect.w, &rect.h);
  rect.x = env->grid_beginning_x;
  rect.y = env->grid_beginning_y - (int)(1.5 * rect.h);
  SDL_RenderCopy(ren, env->diagadj_text, NULL, &rect);

  //menu 
  rect.w = 2*BUTTON_SIZE*h/SCREEN_HEIGHT;
  rect.h = 2*BUTTON_SIZE*h/SCREEN_HEIGHT;
  rect.x = BUTTON_SIZE;
  rect.y = h-2.5*BUTTON_SIZE*h/SCREEN_HEIGHT;
  SDL_RenderCopy(ren, env->menu_button, NULL, &rect);

  //help 
  rect.x = w-BUTTON_SIZE*4;
  rect.w = 2*BUTTON_SIZE*h/SCREEN_HEIGHT;
  rect.h = 2*BUTTON_SIZE*h/SCREEN_HEIGHT;
  SDL_RenderCopy(ren, env->help_button_j, NULL, &rect);


}

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }
  if (env->current_screen == MENU){
    return process_menu(win, ren, env, e);
  }else if (env->current_screen == GAME){
    return process_game(win, ren, env, e);
  }
  return true;
}

bool process_menu(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e){
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // check if mouse is pressing one of the buttons
    // start game
    if (mouse.x < w-w*9/10+w/12 && mouse.x > w-w*9/10 && mouse.y < h-h*1/5+w/12 && mouse.y > h-h*1/5){
      env->current_screen = GAME;
      return false;
    }
  }
  return false;
}

bool process_game(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e){
  if (game_is_over(env->g)) {
    render(win, ren, env);
    SDL_RenderPresent(ren);
    SDL_Delay(DELAY);
    const SDL_MessageBoxButtonData buttons[] = {
        {/* .flags, .buttonid, .text */ 0, 0, "Restart"},
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Next Level"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Quit"},
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        {/* .colors (.r, .g, .b) */
         /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
         {46, 88, 185},
         /* [SDL_MESSAGEBOX_COLOR_TEXT] */
         {255, 255, 255},
         /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
         {255, 255, 255},
         /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
         {0, 122, 153},
         /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
         {0, 153, 255}}};
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,                   /* .flags */
        win,                                          /* .window */
        "Congratulations",                            /* .title */
        "Congratulations! Trump found his new home!", /* .message */
        SDL_arraysize(buttons),                       /* .numbuttons */
        buttons,                                      /* .buttons */
        &colorScheme                                  /* .colorScheme */
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
      SDL_Log("error displaying message box");
      return false;
    }
    while (buttonid == -1) {
      return false;
    }
    if (buttonid == 0) {
      SDL_Log("selection was %s", buttons[buttonid].text);
      game_restart(env->g);
      return false;

    } else if (buttonid == 1) {
      SDL_Log("selection was %s", buttons[buttonid].text);
      //game_delete(env->g);
      if (queue_is_empty(env->games)) {
        printf(
            "For more levels, please send a 100 euro check to Amira Mastouri, "
            "Kea Horvath, Marvin Beites and Alexandre Leymarie.\n");
        return final_message_box(win ,env);
      } else {
        game_delete(env->g);
        char *file_name = queue_pop_tail(env->games);
        env->g = game_load(file_name);
        initialize_tents_text(win, ren, env);
      }
    } else if (buttonid == 2) {
      SDL_Log("selection was %s", buttons[buttonid].text);
      return true;
    }
  }
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    // check if mouse is pressing one of the buttons
    // undo the last move
    if (mouse.x > env->grid_beginning_x +
                      env->cell_size * game_nb_cols(env->g) / 2 -
                      (int)(1.5 * BUTTON_SIZE) &&
        mouse.x <
            (env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) / 2 -
             (int)(1.5 * BUTTON_SIZE) + BUTTON_SIZE)) {
      if (mouse.y > env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5) &&
          mouse.y < (env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5)) +
                        BUTTON_SIZE) {
        game_undo(env->g);
      }
    }
    // redo the last move
    if (mouse.x > env->grid_beginning_x +
                      env->cell_size * game_nb_cols(env->g) / 2 +
                      (int)(0.5 * BUTTON_SIZE) &&
        mouse.x <
            (env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) / 2 +
             (int)(0.5 * BUTTON_SIZE) + BUTTON_SIZE)) {
      if (mouse.y > env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5) &&
          mouse.y < (env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5)) +
                        BUTTON_SIZE) {
        game_redo(env->g);
      }
    }
    // restart the game from the beginning
    if (mouse.x > env->grid_beginning_x +
                      env->cell_size * game_nb_cols(env->g) -
                      (int)(3.5 * BUTTON_SIZE) &&
        mouse.x <
            (env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) -
             (int)(3.5 * BUTTON_SIZE) + BUTTON_SIZE)) {
      if (mouse.y > env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5) &&
          mouse.y < (env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5)) +
                        BUTTON_SIZE) {
        game_restart(env->g);
      }
    }
    // solve the game
    if (mouse.x > env->grid_beginning_x +
                      env->cell_size * game_nb_cols(env->g) -
                      (int)(1.5 * BUTTON_SIZE) &&
        mouse.x <
            (env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) -
             (int)(1.5 * BUTTON_SIZE) + BUTTON_SIZE)) {
      if (mouse.y > env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5) &&
          mouse.y < (env->grid_beginning_y - (int)(BUTTON_SIZE * 1.5)) +
                        BUTTON_SIZE) {
        for (uint i = 0; i < game_nb_rows(env->g); i++){
          for (uint j = 0; j < game_nb_cols(env->g); j++){
            if (game_get_square(env->g, i, j) == TENT || game_get_square(env->g, i, j) == GRASS){
              game_set_square(env->g, i, j, EMPTY);
            }
          }
        }
        game_solve(env->g);
      }
    }
    //go to the menu
    if (mouse.x > BUTTON_SIZE &&
        mouse.x < BUTTON_SIZE +2*BUTTON_SIZE*h/SCREEN_HEIGHT) {
      if (mouse.y >h-2.5*BUTTON_SIZE*h/SCREEN_HEIGHT &&
          mouse.y < h-2.5*BUTTON_SIZE*h/SCREEN_HEIGHT +
                        2*BUTTON_SIZE*h/SCREEN_HEIGHT) {
        env->current_screen = MENU;
      }
    }

    //go to help page 
    if (mouse.x > w-BUTTON_SIZE*4 &&
        mouse.x < w-BUTTON_SIZE*4 +2*BUTTON_SIZE*h/SCREEN_HEIGHT) {
      if (mouse.y >h-2.5*BUTTON_SIZE*h/SCREEN_HEIGHT &&
          mouse.y < h-2.5*BUTTON_SIZE*h/SCREEN_HEIGHT +
                        2*BUTTON_SIZE*h/SCREEN_HEIGHT) {
        env->current_screen = MENU;
      }
    }
    // convert mouse position to cell in grid
    if (mouse_is_in_grid(env, mouse.x, mouse.y)){
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
  }
  else if (e->type == SDL_MOUSEMOTION){
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    if (mouse_is_in_grid(env, mouse.x, mouse.y)){
      uint row, col;
      row = (uint)(mouse.y - env->grid_beginning_y) / env->cell_size;
      col = (uint)(mouse.x - env->grid_beginning_x) / env->cell_size;
      if (e->button.button == SDL_BUTTON_LEFT) {
        if (game_get_square(env->g, row, col) == EMPTY) {
          game_play_move(env->g, row, col, TENT);
        }
      }else if (e->button.button == SDL_BUTTON(SDL_BUTTON_RIGHT)){ //bug avec SDL_BUTTON_RIGHT tout seul ->confirmé par internet
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

  return false;


}
/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /*Clean all the textures of the game */
  for (uint i = 0; i < game_nb_rows(env->g) + game_nb_cols(env->g); i++) {
    SDL_DestroyTexture(env->text[i]);
  }
  free(env->text);
  SDL_DestroyTexture(env->main_menu);
  SDL_DestroyTexture(env->help_button);
  SDL_DestroyTexture(env->help_button_j);
  SDL_DestroyTexture(env->play_button);
  SDL_DestroyTexture(env->exit_button);
  SDL_DestroyTexture(env->menu_button);
  SDL_DestroyTexture(env->wrapping_text);
  SDL_DestroyTexture(env->diagadj_text);

  SDL_DestroyTexture(env->tree);
  SDL_DestroyTexture(env->water);
  SDL_DestroyTexture(env->raft);
  SDL_DestroyTexture(env->losing_water);
  SDL_DestroyTexture(env->losing_raft);
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->undo);
  SDL_DestroyTexture(env->redo);
  SDL_DestroyTexture(env->restart);
  SDL_DestroyTexture(env->solve);
  queue_clear(env->games);
  game_delete(env->g);
  free(env);
}
/* **************************************************************** */

bool mouse_is_in_grid(Env *env, int x, int y){
    if (x < env->grid_beginning_x ||
        x >
            env->grid_beginning_x + env->cell_size * game_nb_cols(env->g) ||
        y < env->grid_beginning_y ||
        y >
            env->grid_beginning_y + env->cell_size * game_nb_rows(env->g)) {
      return false;
    }
    return true;
}

void initialize_tents_text(SDL_Window *win, SDL_Renderer *ren, Env *env) {
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


bool final_message_box (SDL_Window *win , Env *env){
  const SDL_MessageBoxButtonData buttons[] = {
        {/* .flags, .buttonid, .text */ 0, 0, "Restart"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Quit"},
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        {/* .colors (.r, .g, .b) */
         /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
         {46, 88, 185},
         /* [SDL_MESSAGEBOX_COLOR_TEXT] */
         {255, 255, 255},
         /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
         {255, 255, 255},
         /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
         {0, 122, 153},
         /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
         {0, 153, 255}}};
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,                   /* .flags */
        win,                                          /* .window */
        "Ouuuupss !",                            /* .title */
        "For more levels, please send a 100 euro check to Amira Mastouri \n"
          "Kea Horvath, Marvin Beites and Alexandre Leymarie.\n", /* .message */
        SDL_arraysize(buttons),                       /* .numbuttons */
        buttons,                                      /* .buttons */
        &colorScheme                                  /* .colorScheme */
    };
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
      SDL_Log("error displaying message box");
      return false;
    }
    while (buttonid == -1) {
      return false;
    }
    if (buttonid == 0) {
      SDL_Log("selection was %s", buttons[buttonid].text);
      //game_restart(env->g);
      return false;

    } else if (buttonid == 1) {
      SDL_Log("selection was %s", buttons[buttonid].text);
      return true;      
    }
    return false;
}