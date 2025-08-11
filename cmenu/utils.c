#include <stdlib.h>
#include <stdio.h>

#include "cmenu.h"

void panic_if_null(void* ptr){
    if (ptr == NULL){
        fwrite("Memory allocation failed\n", 1, 25, stderr);
        exit(CMENU_SYSTEM_ERROR);
    }
}
