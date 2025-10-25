#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Not enough arguments");
        exit(EXIT_FAILURE);
    }
        int opt;
        int count=1;
        int new_line = 1;
        int n;
        int size=1;
        char *buffer = NULL;
        char *buf = NULL;
        int buf_mode = _IOFBF;
        size_t buf_size= 1024; 


        while((opt = getopt(argc, argv, "b:l:unr:")) != -1){        //we use getopt() to parse command line arguments
            switch (opt){
                case 'n':                                          //-n to eliminate the newline character something like echo -n
                    new_line = 0;                             
                    break;
                case 'r':                             //repeat the output count times
                count = atoi(optarg);             //we set the count to the integer value of the argument
                     if(count < 0){
                        exit(EXIT_SUCCESS);
                    }else if(count == 0){
                        exit(EXIT_FAILURE);
                    }
                    break;
                
                case 'u':                       
                buf_mode = _IONBF;             //we disable the output buffering
                    break;

                case 'l':                           //we set line buffering the program allocates a buffer of size bytes malloc(buf_size)
               buf_mode = _IOLBF;        
               buf_size = atoi(optarg);
                break;

                case 'b':                          //we set full buffering mode the program allocates a buffer of size bytes
                buf_mode = _IOFBF;
                buf_size = atoi(optarg);
                break;

            default:
                exit(EXIT_FAILURE);
            }
            
        }


        if(buf_mode!=_IONBF){
            buffer = (char *)malloc(buf_size);
            if(!buffer){
                perror("malloc");
                exit(EXIT_FAILURE);
            }
        }

        setvbuf(stdout, buffer, buf_mode, buf_size);   //we set the output buffer
        


for(int i = 0; i < count; i++){                                        //outer loop used for repeating the printing times
    for(int j = optind; j < argc; j++){                                //inner loop to iterate iver remaining arguments
        for(int h = 0; h < strlen(argv[j]); h++){                      //outer loop to iterate over each character of the argument
            putchar(argv[j][h]);

            if(j < argc -1){                                          //if not the last argument we print a space
                putchar(' ');
            }

}
}
    if(new_line){                                         //after the last argument we print a new line if new_line is set to 1. so if we dont have -n option
        putchar('\n');
    }
}

free(buffer);
free(buf);

return 0;

}


//why do we need buffers?
//when we write putchar or printf or fputs in this case we are not directly talking to the terminal instead we write into a memory buffer first later the buffer is flushed this means is written out in bigger chunks
//the reason for this is performance because system calls are relatively expensive if we use buffering we accumulate many characters in the memory and we write all of them at once using a single system call this approach is more efficient 

//SUMMARY
//The buffer is a temporary storage area for output. Without it, printing would be slower. In your program, you allocate the buffer yourself so you can give the user control over when output flushes and how big the buffer is.
