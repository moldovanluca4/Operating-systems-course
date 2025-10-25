#include <stdio.h>
#include <stdlib.h>

int main(){
    int *ptr = (int *)malloc(5 * sizeof(int));
    int *arr = (int *)calloc(5, sizeof(int));
    
    if(ptr == NULL){
        printf("memory allocation failed for ptr\n");
        exit(0);
    }

    if(arr == NULL){
        printf("memory allocation failed for arr\n");
        exit(0);
    }


    ptr = (int *)realloc(ptr, 8 * sizeof(int));

    if(ptr == NULL){
        printf("memory allocation failed for ptr\n");
        exit(0);
    }


    free(ptr);

    return 0;
}