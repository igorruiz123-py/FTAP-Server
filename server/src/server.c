#include "server_utils.h"

int main(void){

    srand(time(NULL));

    int choice;

    while(true){
        
        printf("1 -> add user into data base \n");
        printf("2 -> turn server on \n");
        printf("3 -> stop service \n");
        printf("Enter choice: ");
        read_choice(&choice);
        printf("\n");

        if (choice == 1){

            system("clear");

            insert_users_into_db();
            printf("\n");
        }

        else if (choice == 2){

            system("clear");

            FILE *file_connections;
            FILE *file_messages;
            FILE *file_server;

            file_connections = fopen(CONNECTIONS_LOG_FILE_PATH, "a");
            if (file_connections == NULL){
                perror("fopen");
                exit (EXIT_FAILURE);
            }

            file_messages = fopen(MESSAGES_LOG_FILE_PATH, "a");
            if (file_messages == NULL){
                perror("fopen");
                exit (EXIT_FAILURE);
            }

            file_server = fopen(SERVER_LOG_FILE_PATH, "a");
            if (file_server == NULL){
                perror("fopen");
                exit (EXIT_FAILURE);
            }

            time_t start;
            char buf_start[BUFFER_LEN_MAX_SIZE];
            printf("SERVER ONLINE \n");

            time(&start);
            printtime(start, buf_start, sizeof(buf_start));

            fprintf(file_server, "server online at %s \n", buf_start);
            fflush(file_server);
            fprintf(file_server, "--------------------------------------------\n");
            fflush(file_server);

            int sockfd;

            int status;

            int binding;

            int listening;

            int newsockfd;

            struct addrinfo hints, *serverinfo, *p;

            struct sockaddr_storage their_addr;

            char buffer[BUFFER_LEN_MAX_SIZE];

            socklen_t sin_size;

            char IPaddr[INET6_ADDRSTRLEN];


            memset(&hints, 0, sizeof hints);

            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;

            if ((status = getaddrinfo(NULL, PORT, &hints, &serverinfo)) != 0){
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
                return 1;
            }

            for (p = serverinfo; p != NULL; p = p->ai_next){

                sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

                if (sockfd == -1){
                    perror("server: socket");
                    continue;
                }

                binding = bind(sockfd, p->ai_addr, p->ai_addrlen);

                if (binding == -1){
                    perror("server: bind");
                    close(sockfd);
                    continue;
                }

                break;
            }

            freeaddrinfo(serverinfo);

            if (p == NULL){
                fprintf(stderr, "server: failed to bind\n");
                exit(1);
            }

            listening = listen(sockfd, BACKLOG);

            if (listening == -1){
                perror("listen");
                exit(1);
            }

            printf("server: listening for connections... \n");


        while(true){

            time_t t = time(NULL);

            sin_size = sizeof their_addr;

            newsockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

            if (newsockfd == -1){
                perror("accept");
                continue;
            }

            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), IPaddr, sizeof IPaddr);

            printtime(t, buffer, sizeof(buffer));

            printf("server: got connection from %s on port %s at %s \n", IPaddr, PORT, buffer);

            fprintf(file_connections, "client connected from %s on port %s at %s \n", IPaddr, PORT, buffer);
            fflush(file_connections);

            handle_client(newsockfd, file_messages, file_connections, IPaddr);

            fprintf(file_connections, "----------------------------------------------------\n");
            fflush(file_connections);

            close(newsockfd);
        }

        fclose(file_connections);
        fclose(file_messages);
        fclose(file_server);
        close(sockfd);
        
        }

        else {
            break;
        }
    }
}