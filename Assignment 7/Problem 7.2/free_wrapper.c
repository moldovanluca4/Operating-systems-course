#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void (*original_free)(void *) = NULL;

void free(void *nulptr){
    if(!original_free){
        original_free = dlsym(RTLD_NEXT, "free");
    }

   

    fprintf(stderr, "free,,%p,\n", nulptr);

    original_free(nulptr);


    return;

}