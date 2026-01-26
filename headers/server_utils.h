#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#define PORT "2250"
#define BACKLOG 1
#define MAXSIZEDATA 1024
#define DB_FILE_PATH "output/db/db.txt"
#define CONNECTIONS_LOG_FILE_PATH "output/connections_log/client_connections.log"
#define MESSAGES_LOG_FILE_PATH "output/connections_log/client_messages.log"
#define SERVER_LOG_FILE_PATH "output/server_log/server.log"
#define MAX_LEN_USERNAME 64
#define BUFFER_LEN_MAX_SIZE 64
#define MAX_USER_NAME 9
#define MIN_USER_NAME 0
#define MIN_PASSWORD 1000
#define MAX_PASSWORD 9999
#define NUMERIC_BASE_STRTOL 10
#define LINE_MAX_SIZE 128
#define USER_MAX_SIZE 64
#define PASSWORD_MAX_SIZE 64

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>

// DB UTILITIES
typedef struct {

    char username[MAX_LEN_USERNAME];
    char password[PASSWORD_MAX_SIZE];

} User;

void generate_username(
    User *var 
);

void generate_password(
    User *var
);

int read_number_of_users(
    int *var
);

int check_number_of_users(
    int *var
);

void check_malloc(
    User *var
);

int authenticate_user(
    const char *file_name, const char *input_user, const char *input_pass
);

void insert_users_into_db(
    void  
);

int read_choice(
    int *var
);

// SERVER UTILITIES
int recv_line(
    int sockfd, char *buffer, size_t maxlen
);

void *get_in_addr(
    struct sockaddr *sa
);

void printtime(
    time_t t, char *buffer, size_t size
);

int handle_client(
    int newsockfd, FILE *file_message, FILE *file_connection, char *IPaddr 
);

#endif