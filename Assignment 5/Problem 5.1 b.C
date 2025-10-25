#include <stdio.h>
#include <unistd.h>
#include <threads.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>


int shared_data = 30;
int read_count =0;

sem_t mutex;
sem_t writer_sem;

void* reader(void* arg){

//ENTER READER CRITICAL SECTION

    sem_wait(&mutex);                        //we lock the mutex to increment read count
    read_count += 1;                         //the number of active readers is incremented
    if(read_count == 1){                     //if we have an active reader block the writers semaphore to prevent from writing while we have active readers
        sem_wait(&writer_sem);
    }
    sem_post(&mutex);                         //we unlock the mutex so other reader can update the read count
    printf("%d \n", shared_data);             //the active readers can read the shared data outside the mutex this allows concurrency

//EXIT READER CRITICAL SECTION

    sem_wait(&mutex);                         //we lock the mutex so we can decrement read count safely
    read_count -= 1;                          //the number of readers is decremented
    if(read_count == 0){                                     
        sem_post(&writer_sem);                 //if we have no active readers we release the writers semaphore in order to start writing again
    }
        sem_post(&mutex);                      //we unlock the mutex 
    

    return NULL;
}

void* writer(void* args){
    sem_wait(&writer_sem);
    shared_data = 120;
    printf("%d \n", shared_data);
    sem_post(&writer_sem);

    return NULL;
}

int main(int argc, char* argv[]){
    sem_init(&writer_sem, 0, 1);
    sem_init(&mutex, 0, 1);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, reader, NULL);
    pthread_create(&t2, NULL, writer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&mutex);
    sem_destroy(&writer_sem);

    return 0;
}

//in the task implementation readers block writers until every reader is finished. only when read count is 0 writer is allowed to enter and update the shared data
//new readers will only see the updated value
//the release of mutex in the second if condition does not harm the process but is unnecessary
//if we have multiple readers we will leave the readers critical section prematurely and we dont get to release the writers lock only when all the readers left 
//the actual implementation avoids race conditions
//we can also keep the old implementation since it does not harm the program
