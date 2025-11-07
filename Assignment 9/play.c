
#include "quizz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



int play(quiz_t *quiz){

    //printf("Answer multiple choice questions about computer science. You score points for each correctly answered question. If you need multiple attempts to answer a question, the points you score for a correct answer go down.");

    int point_counter = 0;

    printf("Question: %s\n", quiz->question);
    printf("a) %s\n", quiz->choices[0]);
    printf("b) %s\n", quiz->choices[1]);
    printf("c) %s\n", quiz->choices[2]);
    printf("d) %s\n", quiz->choices[3]);
    printf("choose one of the options \t");

    char buffer[10];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL){
        printf("input error");
        return 1;
    }

    char user_response = buffer[0];

    if(user_response == 'b'){
        printf("Correct answer");
        point_counter+=8;
    }else{
        printf("Incorrect answer");
        point_counter+=0;
    }
    
  
    printf("\nYour total score is: %d\n", point_counter);

    return 0;
}
