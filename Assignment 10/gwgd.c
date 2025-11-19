#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include "player.h"
#include <pthread.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>


void play_game(int fd) {
    char buffer[256];
    char *msg = NULL;
    int n;
    
    
    player_t *p = player_new();
    if (!p) return; 

  
    if (player_get_greeting(p, &msg) == 0) {
        write(fd, msg, strlen(msg));
        free(msg);
    }

   
    while (!p->finished) {
        
        if (player_get_challange(p, &msg) == 0) {
            write(fd, msg, strlen(msg));
            free(msg);
        }

       
        memset(buffer, 0, 256);
        n = read(fd, buffer, 255);
        
        if (n <= 0) break; 

       
        if (player_post_challange(p, buffer, &msg) == 0) {
            write(fd, msg, strlen(msg));
            free(msg);
        }
    }

   
    player_del(p);
    close(fd);
    printf("Game over. Client disconnected.\n");
}

void *thread_play_game(void *arg) {
    int fd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());
    play_game(fd);

    return NULL;
}


int main(int argc, char *argv[]){
    signal(SIGCHLD, SIG_IGN);
    int opt;
    bool thread_mode = false;
    bool forking_mode = false;
    char* port_str = "1234"; 

    while((opt = getopt(argc, argv, "tfp:")) != -1){
        switch(opt){
            case 't':
                thread_mode = true;
                break;
            case 'f':
                forking_mode = true;
                break;
            case 'p':
                port_str = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-t] [-f] [-p port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(thread_mode && forking_mode){
        fprintf(stderr, "Error: -t and -f options are mutually exclusive\n");
        return EXIT_FAILURE;
    }

    int portno = atoi(port_str);
    printf("Server starting on port %d...\n", portno);

   
    int sockfd1, sockfd2;
    struct sockaddr_in serv_addr;
    struct sockaddr_in6 serv_addr6;
    int option = 1;

    
    sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    sockfd2 = socket(AF_INET6, SOCK_STREAM, 0);

    if (sockfd1 < 0 || sockfd2 < 0) {
        perror("Error opening sockets");
        exit(1);
    }

    
    setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

   
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd1, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding IPv4");
        exit(1);
    }

    
    memset(&serv_addr6, 0, sizeof(serv_addr6));
    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_addr = in6addr_any;
    serv_addr6.sin6_port = htons(portno);

    if (bind(sockfd2, (struct sockaddr *) &serv_addr6, sizeof(serv_addr6)) < 0) {
        perror("Error binding IPv6");
        exit(1);
    }

  
    listen(sockfd1, 5);
    listen(sockfd2, 5);

    
    fd_set readfds;
    int max_sd, activity;
    int newsockfd;
    struct sockaddr_storage cli_addr; 
    socklen_t clilen;

    while(1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd1, &readfds);
        FD_SET(sockfd2, &readfds);

        max_sd = (sockfd1 > sockfd2) ? sockfd1 : sockfd2;

        
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0)) {
            perror("Select error");
            continue;
        }

        
        if (FD_ISSET(sockfd1, &readfds)) {
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd1, (struct sockaddr *)&cli_addr, &clilen);
            
            if (newsockfd >= 0) {
                if (thread_mode) {
                    pthread_t tid;
                    int *sock_ptr = malloc(sizeof(int));

                    if (sock_ptr == NULL) {
                        perror("Failed to allocate memory for socket descriptor");
                        close(newsockfd);
                        continue;
                    }
                    *sock_ptr = newsockfd;

                    if(pthread_create(&tid, NULL, thread_play_game, sock_ptr) != 0){
                        perror("Failed to create thread");
                        free(sock_ptr);
                        close(newsockfd);
                        continue;
                    }

                } else if (forking_mode) {
                    pid_t kiddo = fork();

                    if(kiddo < 0){
                        perror("Fork failed");
                        continue;
                    }

                    if(kiddo == 0){
                        close(sockfd1);
                        close(sockfd2);
                        play_game(newsockfd);
                        exit(0);
                    }

                    if(kiddo > 0){
                        close(newsockfd);
                    }

                    
                } else {
                    printf("New client connected (IPv4). Playing game...\n");
                    play_game(newsockfd);
                }
            }
        }

       
        if (FD_ISSET(sockfd2, &readfds)) {
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd2, (struct sockaddr *)&cli_addr, &clilen);
            
            if (newsockfd >= 0) {
                if (thread_mode) {
                    pthread_t tid;
                    int *sock_ptr = malloc(sizeof(int));

                    if (sock_ptr == NULL) {
                        perror("Failed to allocate memory for socket descriptor");
                        close(newsockfd);
                        continue;
                    }
                    *sock_ptr = newsockfd;

                    if(pthread_create(&tid, NULL, thread_play_game, sock_ptr) != 0){
                        perror("Failed to create thread");
                        free(sock_ptr);
                        close(newsockfd);
                        continue;
                    }

                } else if (forking_mode) {
                    pid_t child = fork();
                    if(child < 0){
                        perror("Fork failed");
                        continue;
                    }

                    if(child == 0){
                        close(sockfd1);
                        close(sockfd2);
                        play_game(newsockfd);
                        exit(0);
                    }

                    if(child > 0){
                        close(newsockfd);
                    }
                } else {
                    printf("New client connected (IPv6). Playing game...\n");
                    play_game(newsockfd);
                }
            }
        }
    }

    close(sockfd1);
    close(sockfd2);
    return 0;
}


//to avoid code duplication we can create a function like the following to avoid having the same lengthy code inside the main function for ipv6 and ipv4

/*
void dispatch_client(int newsockfd, bool thread_mode, bool forking_mode, int sock_v4, int sock_v6) {
    if (thread_mode) {
        pthread_t tid;
        int *sock_ptr = malloc(sizeof(int));
        if (!sock_ptr) {
            perror("Malloc failed");
            close(newsockfd);
            return;
        }
        *sock_ptr = newsockfd;
        if (pthread_create(&tid, NULL, thread_play_game, sock_ptr) != 0) {
            perror("Thread creation failed");
            free(sock_ptr);
            close(newsockfd);
        }
    } else if (forking_mode) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(newsockfd);
        } else if (pid == 0) {
            
            close(sock_v4);
            close(sock_v6);
            play_game(newsockfd);
            exit(0);
        } else {
         
            close(newsockfd);
        }
    } else {
        
        printf("New client connected. Playing game...\n");
        play_game(newsockfd);
    }
}*/


//after adding this the main function becomes really clean the only thing we have to add is the followinf

/*
if (FD_ISSET(sockfd1, &readfds)) {
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd1, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd >= 0) {
        dispatch_client(newsockfd, thread_mode, forking_mode, sockfd1, sockfd2);
    }
}

if (FD_ISSET(sockfd2, &readfds)) {
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd2, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd >= 0) {
        dispatch_client(newsockfd, thread_mode, forking_mode, sockfd1, sockfd2);
    }
}

*/


//this is much better than repeating the same logic twice 