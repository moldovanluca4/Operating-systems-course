#include "quizz.h"
#include <stdio.h>
#include <stdlib.h>


int main(){
    char *url = "https://opentdb.com/api.php?amount=1&category=18&type=multiple";

    int score = 0;
    int rounds =0;

    while(1){
        printf("Round %d\n", rounds +1);


        char *get_json = fetch(url);

        if(!get_json){
            printf("Failed to fetch quiz data\n");
            break;
        }

        quiz_t current_quizz;

        if(parse(&current_quizz, get_json) !=0){
            printf("Failed to parse quiz data\n");
            free(get_json);
            break;
        }

        free(get_json);

        int game_status = play(&current_quizz);

        if(game_status !=0){
            printf("Game encountered an error\n");
            break;
        }

        rounds++;

    }
    return 0;
}