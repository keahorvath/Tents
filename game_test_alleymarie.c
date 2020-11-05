#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	if(strcmp("dummy", argv[1])==0){
		exit(EXIT_SUCCESS);
	 }
	else{
		fprintf(stderr, "Error : \n expected dummy \n %s found!\n", argv[1]);
		exit(EXIT_FAILURE);
	}
}
