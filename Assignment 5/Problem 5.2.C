#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

//the coffee tastes so good here

sem_t s1;
sem_t s2;
sem_t s3;
int read_count = 0;

void* a(void* arg){
   sem_wait(&s1);
   printf("the ");
   sem_post(&s2);
   
   sem_wait(&s1);
   printf("tastes ");
   sem_post(&s3);

   sem_wait(&s1);
   printf("good ");
   sem_post(&s2);

    
   return NULL;
}

void* b(void* args){
    sem_wait(&s2);
   printf("coffee ");
    sem_post(&s1);

    sem_wait(&s2);
    printf("here ");
    

    
    return NULL;
}

void* c(void* arg){
    sem_wait(&s3);
    printf("so ");
    sem_post(&s1);

    return NULL;
}

int main(int argc ,char* argv[]){
   sem_init(&s1, 0, 1);
   sem_init(&s2, 0, 1);
   sem_init(&s3, 0, 1);

   pthread_t t1, t2, t3;

   pthread_create(&t1, NULL, a, NULL);
   pthread_create(&t2, NULL, b, NULL);
   pthread_create(&t3, NULL, c, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    sem_destroy(&s1);
    sem_destroy(&s2);
    sem_destroy(&s3);

    return 0;
}