#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    fd_set readfds;
    int sockfd1, newsockfd1, newsockfd2, portno4, sockfd2, portno6;
    socklen_t clilen4, clilen6;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    struct sockaddr_in6 serv_addr6, cli_addr6;
    int n, m;
    int opt =1;

    if(argc < 3){
        fprintf(stderr, "Error: no port provided\n");
        exit(1);   
    }

    sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
    sockfd2 = socket(AF_INET6, SOCK_STREAM, 0);

    

    if(sockfd1 < 0 || sockfd2 < 0){
        error("Error: couldnt open the sockets");
    }

    //ipv4
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno4 = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno4);

    setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //ipv6
    bzero((char *) &serv_addr6, sizeof(serv_addr6));
    portno6 = atoi(argv[2]);
    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_addr = in6addr_any;
    serv_addr6.sin6_port = htons(portno6);

    setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //ipv4 socket
    if(bind(sockfd1, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
        error("Error: couldnt bind socket ipv4");
    }

    listen(sockfd1, 5);


    //ipv6 socket
    if(bind(sockfd2, (struct sockaddr *) &serv_addr6, sizeof(serv_addr6))<0){
        error("Error: couldnt bind socket ipv6");
    }

    listen(sockfd2, 5);


    while(1){
        FD_ZERO(&readfds);
        FD_SET(sockfd1, &readfds);
        FD_SET(sockfd2, &readfds);

        int max_sd;

        if(sockfd1 > sockfd2){
            max_sd = sockfd1;
        }else{
            max_sd = sockfd2;
        }

        int activity = select(max_sd+1, &readfds, NULL, NULL, NULL);

        if(activity < 0){
            error("Error: error encountered why selecting");
        }

        if(FD_ISSET(sockfd1, &readfds)){
            clilen4 = sizeof(cli_addr);
            newsockfd1 = accept(sockfd1, (struct sockaddr *) &cli_addr, &clilen4);
        
         if(newsockfd1 < 0){
        error("Error: could not accept");
        }

        bzero(buffer, 256);
        n = read(newsockfd1, buffer, 255);

        if(n < 0){
            error("Error: couldnt read from socket1(ipv4)");
        }

        printf("The message from ipv4 %s\n", buffer);
        n = write(newsockfd1, "Message received\n", 18);

        if(n < 0){
            error("Error: couldnt write to socket1(ipv4)");
        }
        close(newsockfd1);

    }
        if(FD_ISSET(sockfd2, &readfds)){
            clilen6 = sizeof(cli_addr6);
            newsockfd2 = accept(sockfd2, (struct sockaddr *) &cli_addr6, &clilen6);
          
        if(newsockfd2 < 0){
            error("Error: could not accept(ipv6)");
        }
        bzero(buffer, 256);
        m = read(newsockfd2, buffer, 255);

        if(m < 0){
            error("Error: could not read from socket 2");
        }

        printf("The message from ipv6 %s \n", buffer);
        m = write(newsockfd2, "Message received\n", 18);

        if(m < 0){
            error("Error: could not write to socket2");
        }
         close(newsockfd2);
    }
    }

    

    close(sockfd1);
    close(sockfd2);

    return 0;
}

