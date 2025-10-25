#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


int reapall(void) {
    int count = 0;
    int status;
    pid_t pid;

    while (1) {
        pid = waitpid(-1, &status, WNOHANG);

        if (pid > 0) {
            if (WIFEXITED(status)) {
                count++;
            }
        } else if (pid == 0) {
            break;
        } else {
            if (errno == ECHILD) {
                break;
            }
        }
    }

    return count;
}



int main(int argc, char *argv[]){
    pid_t pid;
    int i;

    for (i = 0; i < 3; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
          
            exit(0);
        }
    }

    
    int count = reapall();
    printf("Number of children reaped: %d\n", count);

    return 0;


    return 0;
}