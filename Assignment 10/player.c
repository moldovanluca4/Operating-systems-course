#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "player.h"
#include "chlng.h"


player_t* player_new(void){
    
    player_t *p = malloc(sizeof(player_t));
    if(!p){
        return NULL;
    }

    p->solved = 0;
    p->total = 0;
    p->finished = false;

    p->chlng = chlng_new();

    if(!p->chlng){
        free(p);
        return NULL;
    }
    return p;
}

void player_del(player_t* p){
    if(!p){
        return;
    }
    if(p->chlng){
       chlng_del(p->chlng);
    }
    free(p);
}

void player_reset(player_t* p){
    if(!p){
        return;
    }
    p->solved = 0;
    p->total = 0;
    p->finished = false;

    if(p->chlng){
        chlng_reset(p->chlng);

    }
}

int player_fetch_chlng(player_t* p){
    if(!p || !p->chlng){
        return -1;
    }
    chlng_reset(p->chlng);

   if(chlng_fetch_text(p->chlng) != 0){
        return -1;
    }


    return chlng_hide_word(p->chlng);
}

int player_get_greeting(player_t* p, char **msg){
    if(!p || !msg){
        return -1;
    }

    const char *greeting = "M: Guess the missing word!\r\n";
    *msg = malloc(strlen(greeting) + 1);
    if(!*msg){
        return -1;
    }
    strcpy(*msg, greeting);
    return 0;
}

int player_get_challange(player_t* p, char **msg){
    if(p->chlng->text == NULL){
        if(player_fetch_chlng(p) !=0){
            return -1;
        }
    }

    size_t len = strlen(p->chlng->text) + 7;
    *msg = malloc(len);
    
    if(!*msg){
        return -1;
    }

    sprintf(*msg, "C: %s\r\n", p->chlng->text);
    return 0;
}

int player_post_challange(player_t* p, char *input, char **response){
    if(!p || !response || !p->chlng) return -1;

    if (strncmp(input, "Q:", 2) == 0) {
        p->finished = true;
        
        char temp[128];
        snprintf(temp, sizeof(temp), "M: You mastered %d/%d challenges. Good bye!\r\n", p->solved, p->total);
        *response = strdup(temp);
        return 0;
    }

    if (strncmp(input, "R: ", 3) == 0) {
        char *guess = input + 3; // Skip "R: " prefix
        
        
        guess[strcspn(guess, "\r\n")] = 0;

        p->total++; 

        if (strcmp(guess, p->chlng->word) == 0) {
            p->solved++;
            *response = strdup("O: Congratulation challenge passed!\r\n");
        } else {
            
            char temp[128];
            snprintf(temp, 128, "F: Wrong guess '%s', expected '%s'\r\n", guess, p->chlng->word);
            *response = strdup(temp);
        }
        
        
        chlng_reset(p->chlng);
        return 0;
    }
    
    
    *response = strdup("M: Send your guess in the form 'R: word'.\r\n");
    return 0;
}