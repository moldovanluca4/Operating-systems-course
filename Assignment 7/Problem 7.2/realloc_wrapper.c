#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void* (*original_realloc)(void *, size_t) = NULL;

void *realloc(void *nulptr, size_t size){

    if(!original_realloc){
        original_realloc = dlsym(RTLD_NEXT, "realloc");
    }

    void* address = original_realloc(nulptr, size);

    fprintf(stderr, "realloc,%zu,%p,%p\n", size, nulptr, address);

    return address;
}