#include <stdio.h>
#include <stdlib.h>

void print_err(char *msg){
    fprintf(stdout, "Error when calling: %s \n", msg);
}