#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

int shared_value = 21;
int read_count = 0;
sem_t mutex;
sem_t writer_sem;

void* reader(void* arg){
    sem_wait(&mutex);                             //we enter the reader critical section to modify the read count variable using mutex we ensure atomic updates for the variable if we have multiple readers
    read_count += 1;                              //count of active readers

    if(read_count == 1){                    //if this is the first reader block the writer semaphore preventing any writer from writing while there are active readers
        sem_wait(&writer_sem);
    }
    sem_post(&mutex);                         //we leave the reader critical section
    printf("%d \n", shared_value);           //readers read the shared data
    sem_wait(&mutex);                        //we enter the reader critical section again to decrement read count
    read_count -= 1;                         //decrement the number of active readers
                          
    if(read_count == 0){                       //if we have no readers left we release the writer semaphore so he can start writing again
        sem_post(&writer_sem);               //this if condition should be inside the reader critical section in order to prevent race conditions
    }

    sem_post(&mutex);                       //moved the exit point after the if condition because if we have multiple threads this will cause issues

    return NULL;
}

void* writer(void* args){
    sem_wait(&writer_sem);                 //enter writer critical section
    shared_value = 42;                      //write new shared data
    printf("%d \n", shared_value);
    sem_post(&writer_sem);                  //leave write critical section

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

//the solution works correctly both for when the last if condition is inside the readers critical section and outisde, this is the case for when we have only one thread for one reader and a thread for one writer
//but if we have multiple threads and we assume both of the if conditions were outside of the critical section the threads would race for who increments first and for who decrements first we would not have an order of increments and decrements and we would not know when the last reader would decrement
//this will into prematurely go into the writer function and write data when we still have some threads reading 