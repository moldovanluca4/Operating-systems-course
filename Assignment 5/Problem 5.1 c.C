#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <threads.h>

int shared_data = 40;
int read_count =0;
sem_t mutex;
sem_t writer_sem;

void* reader(void* arg){                //the readeer functions has teh same structure as before
    sem_wait(&mutex);
    read_count += 1;
    if(read_count == 1){
        sem_wait(&writer_sem);
    }
    sem_post(&mutex);
    printf("%d\n", shared_data);
    sem_wait(&mutex);
    read_count -= 1;
    if(read_count == 0){
        sem_post(&writer_sem);
    }
    sem_post(&mutex);
    return NULL;
}

void* writer(void* args){                    // the mutex here is not needed it doesnt harm the correctness of the code but it may reduce concurrency
    sem_wait(&writer_sem);
    sem_wait(&mutex);
    shared_data = 80;
    printf("%d\n", shared_data);
    sem_post(&mutex);
    sem_post(&writer_sem);
    return NULL;
}

int main(int argc, char* argv[]){
    sem_init(&mutex, 0, 1);
    sem_init(&writer_sem, 0, 1);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, reader, NULL);
    pthread_create(&t2, NULL, writer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    

    sem_destroy(&mutex);
    sem_destroy(&writer_sem);
    return 0;
}