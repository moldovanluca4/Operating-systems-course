#ifndef QUIZ_H
#define QUIZ_H

typedef struct{
    unsigned n;
    unsigned score;
    unsigned max;
    char *question;
    char *answer;
    char *choices[4];
} quiz_t;

char* fetch(const char *url);
int parse(quiz_t *quiz, const char *json);
int play(quiz_t *quiz);


#endif