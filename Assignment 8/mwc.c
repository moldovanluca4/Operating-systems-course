#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>

#define IN_WORD 1
#define OUT_WORD 0

typedef struct {
    long lines;
    long words;
    long bytes;
} count;

count totalc = {0, 0, 0};
pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    char *filename;
} thread_data_t;

count count_stream(FILE *stream) {
    count c = {0, 0, 0};
    int ch;
    int state = OUT_WORD;

    while ((ch = fgetc(stream)) != EOF) {
        c.bytes++;
        if (ch == '\n') {
            c.lines++;
        }
        if (isspace(ch)) {
            state = OUT_WORD;
        } else if (state == OUT_WORD) {
            state = IN_WORD;
            c.words++;
        }
    }
    return c;
}

count count_mmap(char *data, long size) {
    count c = {0, 0, 0};
    int state = OUT_WORD;

    for (long i = 0; i < size; i++) {
        char ch = data[i];
        c.bytes++;
        if (ch == '\n') {
            c.lines++;
        }
        if (isspace(ch)) {
            state = OUT_WORD;
        } else if (state == OUT_WORD) {
            state = IN_WORD;
            c.words++;
        }
    }
    return c;
}

void print_count(count c, const char *filename) {
    printf("%7ld %7ld %7ld %s\n", c.lines, c.words, c.bytes, filename);
}

void* thread_file(void* arg) {
    thread_data_t* data = (thread_data_t*) arg;
    const char* filename = data->filename;
    
    count local_c = {0, 0, 0};
    int thread_status = EXIT_SUCCESS;
    int fd = -1;
    struct stat stat_buf;
    int fd_opened = 0;

    if (strcmp(filename, "-") == 0) {
        local_c = count_stream(stdin);
    } else {
        fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror(filename);
            thread_status = EXIT_FAILURE;
        } else {
            fd_opened = 1;
            if (fstat(fd, &stat_buf) == -1) {
                perror("fstat");
                thread_status = EXIT_FAILURE;
            } else {
                if (S_ISREG(stat_buf.st_mode)) {
                    if (stat_buf.st_size == 0) {
                        local_c = (count){0, 0, 0};
                    } else {
                        char *mmap_data = mmap(NULL, stat_buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                        if (mmap_data == MAP_FAILED) {
                            perror("mmap");
                            thread_status = EXIT_FAILURE;
                        } else {
                            local_c = count_mmap(mmap_data, stat_buf.st_size);
                            munmap(mmap_data, stat_buf.st_size);
                        }
                    }
                } else {
                    FILE *file = fdopen(fd, "r");
                    if (file == NULL) {
                        perror("fdopen");
                        thread_status = EXIT_FAILURE;
                    } else {
                        local_c = count_stream(file);
                        fclose(file); 
                        fd_opened = 0; 
                    }
                }
            }
            if (fd_opened) {
                close(fd);
            }
        }
    }

    if (thread_status == EXIT_SUCCESS) {
        print_count(local_c, filename);
    }

    pthread_mutex_lock(&total_mutex);
    totalc.lines += local_c.lines;
    totalc.words += local_c.words;
    totalc.bytes += local_c.bytes;
    pthread_mutex_unlock(&total_mutex);

    free(data);
    return (void*)(long)thread_status;
}

int main(int argc, char *argv[]) {
    int status = EXIT_SUCCESS;

    if (argc == 1) {
        count c = count_stream(stdin);
        print_count(c, "");
    } else {
        int num_files = argc - 1;
        
        pthread_t* threads = malloc(sizeof(pthread_t) * num_files);
        if (threads == NULL) {
            perror("malloc for threads array");
            return EXIT_FAILURE;
        }

        for (int i = 0; i < num_files; i++) {
            const char* filename = argv[i + 1];

            thread_data_t* data = malloc(sizeof(thread_data_t));
            if (data == NULL) {
                perror("malloc for thread data");
                status = EXIT_FAILURE;
                continue; 
            }
            data->filename = (char*)filename;

            if (pthread_create(&threads[i], NULL, thread_file, data) != 0) {
                perror("pthread_create");
                free(data);
                status = EXIT_FAILURE;
            }
        }

        for (int i = 0; i < num_files; i++) {
            void* thread_status;
            pthread_join(threads[i], &thread_status);
            
            if ((long)thread_status == EXIT_FAILURE) {
                status = EXIT_FAILURE;
            }
        }

        free(threads);

        if (argc > 2) {
            print_count(totalc, "total");
        }
    }

    return status;
}