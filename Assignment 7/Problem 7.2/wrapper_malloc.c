#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void* (*original_malloc)(size_t) = NULL;

void *malloc(size_t size){
    if(!original_malloc){
        original_malloc = dlsym(RTLD_NEXT, "malloc");
    }

    void* address = original_malloc(size);

    fprintf(stderr, "malloc,%zu,,%p\n", size, address);



    return address;
}