#include <stdlib.h>
#include <stdio.h>

#include "cmenu.h"

void error(const char* message){
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

void panic_if_null(void* ptr){
    if (ptr == NULL){
        error("Memory allocation failed");
    }
}
