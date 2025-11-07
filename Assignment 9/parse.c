#include "quizz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

static void cleaner_function(quiz_t *quiz){
    free(quiz->question);
    free(quiz->answer);

    for(int i =0; i < 4; i++){
        free(quiz->choices[i]);
    }

    quiz->question = NULL;
    quiz->answer = NULL;
    memset(quiz->choices, 0, sizeof(quiz->choices));
}


int parse(quiz_t *quiz, const char *msg){
    json_t *root = NULL;
    json_error_t error;
    json_t *question = NULL;
    json_t *answers_correct = NULL;
    json_t *answers_incorrect = NULL;
    json_t *category = NULL;
    json_t *type = NULL;
    json_t *difficulty = NULL;
    json_t *result_array = NULL;
    json_t *quiz_object = NULL;

    quiz->question = NULL;
    quiz->answer = NULL;
    memset(quiz->choices, 0, sizeof(quiz->choices));
    
   
    root = json_loads(msg, 0, &error);
    if(!root){
        printf("syntax error in json msg : line %d, error %s \n", error.line, error.text);
        return -1;
    }

    if(!json_is_object(root)){
        printf("json is not an obj\n");
        json_decref(root);
        return -1;
    }
    result_array = json_object_get(root, "results");
    if(!result_array || !json_is_array(result_array)){
        printf("error");
        json_decref(root);
        return -1;
    }


    quiz_object = json_array_get(result_array, 0);
    if(!quiz_object){
        printf("!quizobj error");
        return -1;
    }

    if(!json_is_object(quiz_object)){
        printf("quiz object is not an obj");
        json_decref(root);
        return -1;
    }


    question = json_object_get(quiz_object, "question");
    if(!question){
        printf("!question error");
        return -1;
    }


    if(!json_is_string(question)){
        printf("question is not a string\n");
        json_decref(root);
        return -1;
    }

    category = json_object_get(quiz_object, "category");
    if(!category){
        printf("!category error");
        return -1;
    }

    if(!json_is_string(category)){
        printf("category is not a string");
        json_decref(root);
        return -1;
    }

    
    type = json_object_get(quiz_object, "type");
    if(!type){
        printf("!type error");
        return 1;
    }

    if(!json_is_string(type)){
        printf("type is not a string");
        json_decref(root);
        return -1;
    }


    difficulty = json_object_get(quiz_object, "difficulty");
    if(!difficulty){
        printf("!difficulty error");
        return -1;
    }
   
    if(!json_is_string(difficulty)){
        printf("difficulty is not a string");
        json_decref(root);
        return -1;
    }

    answers_correct = json_object_get(quiz_object, "correct_answer");
    if(!answers_correct){
        printf("!answers correct error");
        return -1;
    }    

    if(!json_is_string(answers_correct)){
        printf("correct answers are not strings");
        json_decref(root);
        return -1;
    }

    answers_incorrect = json_object_get(quiz_object, "incorrect_answers");
    if(!answers_incorrect){
        printf("!answers incorrect error");
        return -1;
    }

    if(!json_is_array(answers_incorrect)){
        printf("incorrect answers are not a string\n");
        json_decref(root);
        return -1;
    }
    
    quiz->answer = strdup(json_string_value(answers_correct));
    quiz->question = strdup(json_string_value(question));

int i =0;
    for(i=0 ; i < json_array_size(answers_incorrect) && i < 3; i++){
        json_t *incorrect_answers = json_array_get(answers_incorrect, i);  
        if(json_is_string(incorrect_answers)){
            quiz->choices[i] = strdup(json_string_value(incorrect_answers)); 
        } 

    }

   
   quiz->choices[i] = strdup(json_string_value(answers_correct));
    

   json_decref(root);
    return 0;
}