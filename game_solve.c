#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4){
        fprintf(stderr, "You didn't give the right number of arguments!\n");
        exit(EXIT_FAILURE);
    } 
    game g = game_load(argv[2]);
    if (strcmp("-s", argv[1]) == 0){
        if (game_solve(g)){
            if (argc == 3){
                game_print(g);
            }else if (argc == 4){
                game_save(g, argv[3]);
            }
        }else{
            return EXIT_FAILURE;
        }
    }else if (strcmp("-c", argv[1]) == 0){
        if (argc == 3){
            printf("%u\n", game_nb_solutions(g));
        }else if (argc == 4){
            FILE *f = fopen(argv[3], "w");
            if (f == NULL) {
                fprintf(stderr, "file couldn't open!\n");
                exit(EXIT_FAILURE);
            }
            fprintf(f, "%u\n", game_nb_solutions(g));
        }
    }else{
        fprintf(stderr, "Option given isn't valid!\n");
        exit(EXIT_FAILURE);
    }
    game_delete(g);
    return EXIT_SUCCESS;
}