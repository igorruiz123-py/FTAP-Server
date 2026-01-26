#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>

#define PORT "2250"
#define MAXSIZEDATA 1024
#define IPv4_SERVER_ADDRESS "192.168.100.249"


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int open_connection(void){

    int sockfd, status;

    char IPaddr[INET6_ADDRSTRLEN];

    struct addrinfo hints, *serverinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(IPv4_SERVER_ADDRESS, PORT, &hints, &serverinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next){

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sockfd == -1) {
            perror("client: socket");
            continue;
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), IPaddr, sizeof IPaddr);
        printf("client: attempting connection to: %s on port %s \n", IPaddr, PORT);

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL){
        fprintf(stderr, "client: failed to connect\n");
        return -1;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), IPaddr, sizeof IPaddr);
    printf("client: connected to %s \n", IPaddr);

    freeaddrinfo(serverinfo);

    return sockfd;
}

int main(void){

    int sockfd = -1;

    ssize_t bytes;

    char buffer[MAXSIZEDATA];

    char cmd[64];

    printf("TCP client running... \n\n");

    printf("'open': open connection with server \n");
    printf("'send': send message to server \n");
    printf("'close': close connection with server \n");
    printf("'exit': quit program \n\n");

    while(true){

        printf("> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strcmp(cmd, "open") == 0){

            sockfd = open_connection();

            if (sockfd != -1){

                // abertura do socket | enviando código 100
                printf("client: sent 100 \n");
                strcpy(buffer, "100\n");
                send(sockfd, buffer, strlen(buffer), 0);
                bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);

                if (bytes > 0){
                    buffer[bytes] = '\0';
                    printf("%s", buffer);
                }

                // enviando código 200
                printf("client: sent 200 \n");
                strcpy(buffer, "200\n");
                send(sockfd, buffer, strlen(buffer), 0);


                // enviando username
                printf("Enter username: ");
                fgets(buffer, sizeof(buffer), stdin);
                send(sockfd, buffer, strlen(buffer), 0);
                bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);

                if (bytes > 0){
                    buffer[bytes] = '\0';
                    printf("%s", buffer);
                }

                // enviando código 300
                printf("client: sent 300 \n");
                strcpy(buffer, "300\n");
                send(sockfd, buffer, strlen(buffer), 0);

                // enviando password
                printf("Enter password: ");
                fgets(buffer, sizeof(buffer), stdin);
                send(sockfd, buffer, strlen(buffer), 0);
                bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);

                if (bytes > 0){
                    buffer[bytes] = '\0';
                    printf("%s", buffer);
                }


            }

            continue;
        }

        else if (strcmp(cmd, "send") == 0){

            if (sockfd == -1){
                printf("No connection established. \n");
                continue;
            }

            // enviando código 400
            printf("client: sent 400 \n");
            strcpy(buffer, "400\n");
            send(sockfd, buffer, strlen(buffer), 0);

            // enviando mensagem
            printf("Type message: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer), 0);
            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);

            if (bytes > 0){
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }

            continue;
        }

        else if (strcmp(cmd, "close") == 0){

            if (sockfd == -1){
                printf("No connection established. \n");
                continue;
            }

            // enviando código 500
            printf("client: sent 500 \n");
            strcpy(buffer, "500\n");
            send(sockfd, buffer, strlen(buffer), 0);
            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);

            if (bytes > 0){
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }

            close(sockfd);
            sockfd = -1;
            continue;
        }

        // saindo do programa cliente TCP
        else if (strcmp(cmd, "exit") == 0){

            if (sockfd != -1){
                close(sockfd);
                printf("goodbye! \n");
                break;
            }

            printf("goodbye! \n");

            break;
        }

        else {

            if (sockfd == -1){
                printf("No connection established.\n");
                continue;
            }

            /* envia comando inválido COMO LINHA */
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%s\n", cmd);

            if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
                perror("send");
                close(sockfd);
                sockfd = -1;
                continue;
            }

            /* recebe resposta do servidor */
            memset(buffer, 0, sizeof(buffer));
            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);

            if (bytes <= 0) {
                printf("server disconnected.\n");
                close(sockfd);
                sockfd = -1;
                continue;
            }

            buffer[bytes] = '\0';
            printf("%s", buffer);
        }


    }


}