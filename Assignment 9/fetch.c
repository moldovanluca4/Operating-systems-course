#include "quizz.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>



char* fetch(const char *url){
    
    int filedes[2];
    pipe(filedes);                   //this is a system call that asks the linux kernel to create a pipe, the kernel teh creates an in memory buffer and give back 2 file descriptots in the filedes array
    pid_t kiddo = fork();            //another system call to creat the a clone of the current process
    int bytes_read;
    char temp_chunk[2048];          //this allocates 2kb buffer on the stack its basically a temporary area for datat we read from the pipe
//filedes[0] is the read end of the pipe and filedes[1] is the write end of the pipe

    if(kiddo < 0){
        perror("Fork failed");
        return NULL;
    }

//this code only runs in the child process the task is to become the curl and redirect its output into the pipe
    if(kiddo == 0){
        
        //the child process only writes to the pipe so the read end of the pipe its closed
        close(filedes[0]);   
        //dup2 is a system call that makes the file descriptor a stdout out of the pipes write end
        //under the hood the kernels file descriptor table for this specific process has stdout pointing to the same pipe that filedes[1] points to
        dup2(filedes[1], STDOUT_FILENO);
        //now that stdout points to the pipe we can close the original pipe end
        close(filedes[1]);
        //this system call replaces the child process image with the curl, basically the curl inherits the parents process file descriptor
        //when curl runs and tries to print the webpage to its stdout will be writing all the data directly into to the pipe instead this is because of the dup2 sys call
        execlp("curl", "curl", "-s", url, NULL);
        fprintf(stderr, "[Debug: fetch-child] Child running. Redirecting stdout...\n");
        exit(0);
    }


    close(filedes[1]);


    //initial 2kb dynamic buffer on the heap that will hold the final response buffer
    char *response_buffer = malloc(2048);
    long total_size = 0;
    long current_capacity = 2048;


    //main loop where we call read that will pause the parent process until we have data in the pipe
    //when curl writes data read wakes up and copies up to 2048 bytes into the temp chunk and return the nr of bytes instead
    //when curl is done read will return 0 and loop terminates
    while((bytes_read = read(filedes[0], temp_chunk, sizeof(temp_chunk))) > 0){
        //dynamic resizing logic to avoid overflowing
        if(total_size + bytes_read > current_capacity){
            current_capacity = current_capacity * 2;
            response_buffer = realloc(response_buffer, current_capacity * 2);
            
        }

        //we copy the data from temp chunk into the response buffer
        memcpy(response_buffer + total_size, temp_chunk, bytes_read);

        total_size += bytes_read; //update the new total length
}


//we pause the parent until the child process is done to avoid zombie process
wait(NULL);
//close the parents read end
close(filedes[0]);
//if the buffer allocated is to large we shrink it to avoid empty useless spaces
response_buffer = realloc(response_buffer, total_size+1);
//and we format the raw data inro a valid usable string by addid the null terminator
response_buffer[total_size] = '\0';

//return the pointer to the memory allocated string
return response_buffer;
}