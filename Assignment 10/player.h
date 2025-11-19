#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "chlng.h"

typedef struct player{
    int solved;                                    //correctly solved challanges
    int total;                                   //total number of challanges
    bool finished;                              //true if we are done                
    chlng_t *chlng;                            //current challange
} player_t;

player_t* player_new(void);                               //allocate a new player

void player_reset(player_t*);                           //reset the internal state of a player         

void player_del(player_t*);                             //delete a player and all its resources

int player_fetch_chlng(player_t*);                      //allocate a new challange for the  player

int player_get_greeting(player_t*, char **);            //retrieve a greeting message for the player

int player_get_challange(player_t*, char **);              //retrieve the challenge message
 
int player_post_challange(player_t*, char *, char **);         //post a message to the player and retrieve the response message

#endif
