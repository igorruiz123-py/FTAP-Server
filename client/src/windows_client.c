#define _WIN32_WINNT 0x0601

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "2250"
#define MAXSIZEDATA 1024
#define IPv4_SERVER_ADDRESS "192.168.100.249"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

SOCKET open_connection(void)
{
    SOCKET sockfd = INVALID_SOCKET;
    int status;
    char IPaddr[INET6_ADDRSTRLEN];

    struct addrinfo hints, *serverinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(IPv4_SERVER_ADDRESS, PORT, &hints, &serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %d\n", status);
        return INVALID_SOCKET;
    }

    for (p = serverinfo; p != NULL; p = p->ai_next) {

        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == INVALID_SOCKET) {
            perror("socket");
            continue;
        }

        inet_ntop(p->ai_family,
                  get_in_addr((struct sockaddr *)p->ai_addr),
                  IPaddr, sizeof IPaddr);

        printf("client: attempting connection to %s:%s\n", IPaddr, PORT);

        if (connect(sockfd, p->ai_addr, (int)p->ai_addrlen) == SOCKET_ERROR) {
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(serverinfo);

    if (p == NULL) {
        printf("client: failed to connect\n");
        return INVALID_SOCKET;
    }

    printf("client: connected to %s\n", IPaddr);
    return sockfd;
}

int main(void)
{
    WSADATA wsa;
    SOCKET sockfd = INVALID_SOCKET;
    int bytes;

    char buffer[MAXSIZEDATA];
    char cmd[64];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    printf("TCP client running (Windows)...\n");

    while (true) {

        printf("> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strcmp(cmd, "open") == 0) {

            sockfd = open_connection();
            if (sockfd == INVALID_SOCKET)
                continue;

            strcpy(buffer, "100\n");
            send(sockfd, buffer, strlen(buffer), 0);

            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }

            strcpy(buffer, "200\n");
            send(sockfd, buffer, strlen(buffer), 0);

            printf("Enter username: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer), 0);

            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }

            strcpy(buffer, "300\n");
            send(sockfd, buffer, strlen(buffer), 0);

            printf("Enter password: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer), 0);

            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }
        }

        else if (strcmp(cmd, "send") == 0) {

            if (sockfd == INVALID_SOCKET) {
                printf("No connection.\n");
                continue;
            }

            strcpy(buffer, "400\n");
            send(sockfd, buffer, strlen(buffer), 0);

            printf("Type message: ");
            fgets(buffer, sizeof(buffer), stdin);
            send(sockfd, buffer, strlen(buffer), 0);

            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }
        }

        else if (strcmp(cmd, "close") == 0) {

            if (sockfd == INVALID_SOCKET)
                continue;

            strcpy(buffer, "500\n");
            send(sockfd, buffer, strlen(buffer), 0);

            bytes = recv(sockfd, buffer, MAXSIZEDATA - 1, 0);
            if (bytes > 0) {
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }

            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
        }

        else if (strcmp(cmd, "exit") == 0) {

            if (sockfd != INVALID_SOCKET)
                closesocket(sockfd);

            break;
        }

        else {
            printf("Unknown command\n");
        }
    }

    WSACleanup();
    return 0;
}
