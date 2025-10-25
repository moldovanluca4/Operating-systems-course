#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    uint64_t start;
    uint64_t end;
    int id;
    int verbose;
} ThreadArgs;

static int is_perfect(uint64_t num) {
    uint64_t i, sum;
    if (num < 2) return 0;

    for (i = 2, sum = 1; i * i <= num; i++) {
        if (num % i == 0) {
            sum += (i * i == num) ? i : i + num / i;
        }
    }
    return sum == num;
}

void* search_perfect(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;

    if (args->verbose)
        fprintf(stderr, "perfect: t%d searching [%lu,%lu]\n", args->id, args->start, args->end);

    for (uint64_t n = args->start; n <= args->end; n++) {
        if (is_perfect(n))
            printf("%lu\n", n);
    }

    if (args->verbose)
        fprintf(stderr, "perfect: t%d finishing\n", args->id);

    return NULL;
}

int main(int argc, char* argv[]) {
    uint64_t start = 1, end = 10000;
    int num_threads = 1;
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "s:e:t:v")) != -1) {
        switch(opt) {
            case 's': start = strtoull(optarg, NULL, 10); break;
            case 'e': end = strtoull(optarg, NULL, 10); break;
            case 't': num_threads = atoi(optarg); break;
            case 'v': verbose = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-s start] [-e end] [-t threads] [-v]\n", argv[0]);
                return 1;
        }
    }

    pthread_t threads[num_threads];
    ThreadArgs args[num_threads];

    uint64_t range = end - start + 1;
    uint64_t chunk = range / num_threads;
    uint64_t extra = range % num_threads;

    uint64_t current_start = start;
    for (int i = 0; i < num_threads; i++) {
        args[i].start = current_start;
        args[i].end = current_start + chunk - 1;
        if (extra > 0) {
            args[i].end++;
            extra--;
        }
        args[i].id = i;
        args[i].verbose = verbose;
        current_start = args[i].end + 1;

        pthread_create(&threads[i], NULL, search_perfect, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
