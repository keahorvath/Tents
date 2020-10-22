#include "game_aux.h"
#include "game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*Main routine*/

int main(int argc, char *argv[]){
    //start test
    if(strcmp("dummy", argv[1])==0){
        exit(EXIT_SUCCESS);
    }
    else{
        fprintf(stderr, "Error : test \"%s\"not found!\n", argv[1]);
        exit(EXIT_FAILURE);
    }
}