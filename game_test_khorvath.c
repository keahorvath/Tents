#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    if (strcmp("dummy", argv[1]) == 0){
        exit(EXIT_SUCCESS);
    }
    else{
        fprintf(stderr, "wrong argument!\n");
        exit(EXIT_FAILURE);
    }
}