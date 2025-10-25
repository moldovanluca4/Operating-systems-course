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
       
        while((opt = getopt(argc, argv, "nr:")) != -1){         //getopt() is a function that helps to parse argv and argc
            switch (opt){
                case 'n':                                       //case n disables the newline character at the end
                    new_line = 0;                             
                    break;
                case 'r':                                      //case r sets var count to the values passed
                count = atoi(optarg);                      //we set the count to the integer value of the argument
                     if(count < 0){
                        exit(EXIT_SUCCESS);
                    }else if(count == 0){
                        exit(EXIT_FAILURE);
                    
                    break;
            }
            
        }

        
}

for(int i = 0; i < count; i++){                                        //outer loop used for repeating the printing times
    for(int j = optind; j < argc; j++){                                //inner loop to iterate over remaining arguments
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

return 0;

}
