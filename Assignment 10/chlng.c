#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "chlng.h"



chlng_t* chlng_new(void) {
    chlng_t *c = malloc(sizeof(chlng_t));
    if (!c) return NULL;
    c->text = NULL;
    c->word = NULL;
    return c;
}

void chlng_reset(chlng_t *c) {
    if (!c) return;
    if (c->text) {
        free(c->text);
        c->text = NULL;
    }
    if (c->word) {
        free(c->word);
        c->word = NULL;
    }
}


void chlng_del(chlng_t *c) {
    if (!c) return;
    chlng_reset(c); 
    free(c);        
}

int chlng_fetch_text(chlng_t *c){
    int filedes[2];
    pipe(filedes);
    pid_t pid = fork();
    char temp_chunk[256];
    int bytes_read;


    if(pid < 0){
        perror("fork has failed");
        return -1;
    }

    if(pid == 0){
        close(filedes[0]); 
        dup2(filedes[1], STDOUT_FILENO);
        close(filedes[1]);
        execlp("/usr/bin/fortune", "fortune", "-s", "-n", "80", NULL);
        fprintf(stderr, "execlp failed\n");
        exit(0);
    }

    close(filedes[1]);

    char *response_buffer = malloc(2048);
    if(!response_buffer){
        return -1;
    }

    long total_size = 0;
    long current_capacity = 2048;


    while((bytes_read = read(filedes[0], temp_chunk, sizeof(temp_chunk))) > 0){
        if(total_size + bytes_read > current_capacity){
            current_capacity *= 2;
            response_buffer = realloc(response_buffer, current_capacity*2);
        }
        memcpy(response_buffer + total_size, temp_chunk, bytes_read);
        total_size += bytes_read;
    }

    wait(NULL);

    close(filedes[0]);
    response_buffer = realloc(response_buffer, total_size + 1);
    response_buffer[total_size] = '\0';

    if(c->text){
        free(c->text);
    }
    c->text = response_buffer;

    return 0;
}



int chlng_hide_word(chlng_t *c){
    if(!c->text){
        return -1;
    }

    char *text_copy = strdup(c->text);
    if(!text_copy){
        return -1;
    }

    char *token;
    char *rest = text_copy;
    char *words[256];
    int word_count = 0;

    while((token = strtok_r(rest, " \n\t.,;:!?'\"()-", &rest)) && word_count < 256){
        words[word_count++] = token;
    }

    if(word_count == 0){
        free(text_copy);
        return -1;
    }

    int random_index = rand() % word_count;;
    char *selected_word = words[random_index];
    c->word = strdup(selected_word);
    if(!c->word){
        free(text_copy);
        return -1;
    }
    char *pos = strstr(c->text, selected_word);
    if(pos){
        memset(pos, '_', strlen(selected_word));
    }
    free(text_copy);
    return 0;
}