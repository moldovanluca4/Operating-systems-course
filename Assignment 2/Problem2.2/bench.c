#define _POSIX_C_SOURCE 199309L                         //using this we enable the feature clock_gettime() in order to measure our program time
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]){
    int opt;
    time_t start = time(NULL);
    time_t end = time(NULL);
    int default_duration = 5;                                   //how many timed runs to do the default value is 5
    int default_warmup = 0;
    int total_runs = 0;
    int failed_runs =0;
    double sum_time= 0.0;
    double min_time = 1e9;
    double max_time = 0.0;

    while((opt = getopt(argc, argv, "d:w:")) != -1){                  //command line option parsing using the getopt() function
        switch(opt){
            case 'd':                                                 //-d is the number of measured runs
                default_duration = atoi(optarg);
                break;

            case 'w':                                                //-w is the number of warmup runs
                default_warmup = atoi(optarg);
                break;
                
            
        }
    }

    char **child_argv = &argv[optind];                     //we prepare the child program arguments. child_argv is an array expected by the execvp() function

//Warmup runs

    for(int i =0 ; i < default_warmup; i++){               
        pid_t pid = fork();                               //we create a child process using fork() function in the child we run the target program execvp()
        if(pid < 0){
            printf("Fork failed\n");
            exit(1);
        }else if(pid == 0){
            execvp(child_argv[0], child_argv);
            printf("Exec failed\n");
            _exit(1);
        }else{
            waitpid(pid, NULL, 0);                     //waitpid() function is used to make the parent process wait until the child process is done
        }
    }

//timed runs

    for(int i = 0 ; i < default_duration; i++){
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);             //we start the timer
        pid_t pid = fork();                                      //fork -> child runs the target program
        if(pid < 0){
            printf("Fork failed\n");
            _exit(1);
        }else if(pid == 0){
            execvp(child_argv[0], child_argv);
            printf("Exec failed\n");
            _exit(1);
        }else{
            int status;
            waitpid(pid, NULL, 0);                           //parent waits for child process to finish
    }

        clock_gettime(CLOCK_MONOTONIC, &end_time);                            //we stop the timer and afterwards we calculate the min time, avg time and max time
        double elapsed = (end_time.tv_sec - start_time.tv_sec) + 
                         (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        sum_time += elapsed;
        if(elapsed < min_time) min_time = elapsed;
        if(elapsed > max_time) max_time = elapsed;
        total_runs++;
    }

    double avg_time = sum_time / total_runs;

    printf("Total runs: %d\n", total_runs);
    printf("Failed runs: %d\n", failed_runs);
    printf("Warmups: %d\n", default_warmup);
    printf("Average time: %.6f seconds\n", avg_time);
    printf("Minimum time: %.6f seconds\n", min_time);
    printf("Maximum time: %.6f seconds\n", max_time);

    return 0;
}
