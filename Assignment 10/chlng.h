#ifndef CHLNG_H
#define CHLNG_H

typedef struct{
    char *text;                           //text with a missing word
    char* word;                          //the missing word     
} chlng_t;


chlng_t* chlng_new(void);                        //allocate a new challange

void chlng_reset(chlng_t*);                     //reset the internal state of the challange

void chlng_del(chlng_t*);                       //delete a challange and all its resources

int chlng_fetch_text(chlng_t *c);               //fetch new text from an invocation of fortune

int chlng_hide_word(chlng_t *c);                //select and hide a word in the text

#endif