#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

pthread_mutex_t global_lock= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t per_coin_lock[20];

char global_array[20]={'0','0','0','0','0','0','0','0','0','0','X','X','X','X','X','X','X','X','X','X'}; 

//thread attributes
typedef struct{
    int thread_id;
    int num_flips;
    int strategy;
}thread_data_t;

//thread function
void* thread_person(void* arg){
thread_data_t* attributes = (thread_data_t*) arg;
int id = attributes->thread_id;
int flips = attributes->num_flips;
int strat = attributes->strategy;
 
if(strat == 1){
    pthread_mutex_lock(&global_lock);
for(int flip = 0; flip < flips-1; flip++){
    for(int k=0; k<20; k++){
        if(global_array[k] == '0'){
            global_array[k] ='X';
        }else if(global_array[k]=='X'){
            global_array[k] ='0';
        }
    }
}
    pthread_mutex_unlock(&global_lock);

}else if(strat == 2){
    
for(int flip=0; flip < flips-1; flip++){
    pthread_mutex_lock(&global_lock);
    for(int l =0; l<20; l++){
    
        if(global_array[l] == '0'){
            global_array[l] = 'X';
        }else if(global_array[l]=='X'){
            global_array[l] = '0';
        }
        
    }
    pthread_mutex_unlock(&global_lock);
}
}else if(strat == 3){

for(int flip =0; flip<flips; flip++){
    for(int y=0 ; y<20; y++){
        pthread_mutex_lock(&per_coin_lock[y]);
        if(global_array[y] == '0'){
            global_array[y] = 'X';
        }else if(global_array[y]=='X'){
            global_array[y] = '0';
        }
        pthread_mutex_unlock(&per_coin_lock[y]);
    }
}
}

free(attributes);
return NULL;
}

//run threads helper function
void run_threads(int num_threads, void* (*strategy_func)(void*)) {
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);

    for(int t = 0; t < num_threads; t++) {
        thread_data_t* attr = (thread_data_t*)malloc(sizeof(thread_data_t));
        attr->thread_id = t;
        attr->num_flips = 1000;  
        attr->strategy = 1;          
        pthread_create(&threads[t], NULL, strategy_func, attr);
    }

    for(int t = 0; t < num_threads; t++)
        pthread_join(threads[t], NULL);

    free(threads);
}

//timeit function
static double timeit(int n, void* (*proc)(void *)){
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc);
    t2 = clock();
    return ((double)t2 - (double)t1) / CLOCKS_PER_SEC * 1000;
}

//main function
int main(int argc, char *argv[]){
    int opt;
    int default_flips=1000;
    int num_threads=100;
    pthread_t threads;
    int choose_strategy;
    size_t j;
    int strategies[] = {1, 2, 3};
    const char* strat_names[] = {"global lock", "iteration lock", "coin lock"};

    
    for(int i=0; i<20; i++){
    pthread_mutex_init(&per_coin_lock[i], NULL);
}

    
    printf("coins(initial state): ");
    for(int i =0; i < sizeof(global_array); i++){
        printf("%c ", global_array[i]);
    }
    
    printf("\n");

    
    while((opt = getopt(argc, argv, "p:n:")) != -1){
        switch(opt){
            case 'p':
                num_threads = atoi(optarg);
                break;
            case 'n':
                default_flips = atoi(optarg);
                break;

        }

    }

    pthread_t* thread_sizes = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    if(thread_sizes == NULL){
        perror("Error: unable to allocate memory for threads\n");
        exit(EXIT_FAILURE);
}
    
for(j =0; j < num_threads; j++){
    thread_data_t* attr = (thread_data_t*)malloc(sizeof(thread_data_t));
    attr->thread_id = j;
    attr->num_flips = default_flips;
    attr->strategy = 1;
    pthread_create(&thread_sizes[j], NULL, thread_person, attr);                    
}

for(j=0; j< num_threads; j++){
    pthread_join(thread_sizes[j], NULL);
}
   free(thread_sizes);

   printf("coins(final state):");
   for(int i=0; i<20; i++){
    printf("%c", global_array[i]);
   }

for(int y=0; y<20; y++){
    pthread_mutex_destroy(&per_coin_lock[y]);
}

for(int s = 0; s < 3; s++) {
    int choose_strategy = strategies[s];

    char initial_state[20] = {'0','0','0','0','0','0','0','0','0','0',
                              'X','X','X','X','X','X','X','X','X','X'};
    memcpy(global_array, initial_state, 20);

    printf("coins: ");
    for(int i = 0; i < 20; i++) printf("%c", global_array[i]);
    printf(" (start - %s)\n", strat_names[s]);

    clock_t t1 = clock();

    
    for(j =0; j < num_threads; j++){
        thread_data_t* attr = (thread_data_t*)malloc(sizeof(thread_data_t));
        attr->thread_id = j;
        attr->num_flips = default_flips;
        attr->strategy = choose_strategy; 
        pthread_create(&thread_sizes[j], NULL, thread_person, attr);                    
    }

    for(j=0; j< num_threads; j++){
        pthread_join(thread_sizes[j], NULL);
    }

    clock_t t2 = clock();
    double ms = ((double)(t2 - t1)) / CLOCKS_PER_SEC * 1000;

    printf("coins: ");
    for(int i=0; i<20; i++) printf("%c", global_array[i]);
    printf(" (end - %s)\n", strat_names[s]);

    printf("%d threads x %d flips: %.3f ms\n", num_threads, default_flips, ms);
}



    return 0;
}