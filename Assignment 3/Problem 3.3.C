#define _POSIX_C_SOURCE 199309L 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>

//Problem 3.3 a)                                                                                    


static int is_perfect(uint64_t num){
    uint64_t i, sum;

    if(num < 2){
        return 0;
    }

    for(i=2, sum =1; i*i<=num; i++){
        if (num % i == 0) {
             sum += (i*i == num) ? i : i + num / i;
}
}

    return (sum == num);
}




int main(int argc, char* argv[]){
    int opt;
    int lower_bound, upper_bound;
    int num;

    if(argc < 2){
        exit(EXIT_FAILURE);
    }

    while((opt = getopt(argc, argv, "s:e:")) != -1){
        switch (opt){
            case 's':
                lower_bound = atoi(optarg);
                break;
            case 'e':
                upper_bound = atoi(optarg);
                break;
    }
}       
    for(uint64_t j=lower_bound; j<=upper_bound; j++){
        if(is_perfect(j)){
            printf("%llu\n", j);
        }
    }
    
        return 0;
}