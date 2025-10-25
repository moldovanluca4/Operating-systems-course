#include <stdio.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

    

int main(int argc, char *argv[]){
                                                              //argc is the argument count, so basically argc is an int that tells you how many argument were passed to the program from the command line
                                                              //for ex we have ./program hello world argc is 3 because we have 3 arguments and argv is an array containing this 3 strings
                                                              
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file1 = "/this/path/does/not/exist.txt";             //a file that does not exist

    const char *file2 = "/Desktop/Operating systems/Assignment 1/exists.txt";    //file is set to be in read only mode and we attempt to open it for writing

    int op = open(file1, O_RDONLY);

    if(op == -1){                                       //first error should be opening a file that does not exist
        perror("Error opening file 1");           
        
    }

   int op2 = open(file2, O_WRONLY);                     //second error should be opening a file that is read only for writing

    if(op2 == -1){
        perror("Error opening file 2 for writing");
        exit(EXIT_FAILURE);
    }

    close(op);                                       //a close error trying to close a file that was never opened
    if(close(op) == -1){
        perror("Error closing file 1");
        exit(EXIT_FAILURE);
    }

    close(op2);

        return 0;
}

