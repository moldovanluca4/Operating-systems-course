#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void* (*original_calloc)(size_t, size_t) = NULL;

void *calloc(size_t nmemb,size_t size){
    if(!original_calloc){
        original_calloc = dlsym(RTLD_NEXT, "calloc");
    }

    void* address = original_calloc(nmemb, size);

    size_t total_size = nmemb *size;

    fprintf(stderr, "calloc,%zu,,%p\n", total_size, address);

    return address;
}