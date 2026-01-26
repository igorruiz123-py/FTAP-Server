#include "server_utils.h"

int authenticate_user(const char *file_name, const char *input_user, const char *input_pass){

    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        return 0;
    }

    char line[LINE_MAX_SIZE];
    char user[USER_MAX_SIZE];
    char pass[PASSWORD_MAX_SIZE];

    while (fgets(line, sizeof(line), file)) {

        if (sscanf(line, "User: %31s Password: %31s", user, pass) == 2) {

            if (strcmp(user, input_user) == 0 && strcmp(pass, input_pass) == 0) {

                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

void generate_user_name(User *var){

    int space_index = -1;
    char buffer[BUFFER_LEN_MAX_SIZE];
    char aux[BUFFER_LEN_MAX_SIZE];

    printf("Enter first and last name: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if (strlen(buffer) < 8){
        printf("Name and last name must be at least 8 characters! \n");
        exit (EXIT_FAILURE);
    }

    for (int i = 0; buffer[i] != '\0'; i++){

        if (buffer[i] <= '9' && buffer[i] >= '0'){
            printf("Error, name and last name must be all characters! \n");
            exit (EXIT_FAILURE);
        }

        if (buffer[i] == ' '){
            space_index = i;
            break;
        }
    }

    if (space_index == -1){
        printf("It is required name and last name! \n");
        exit (EXIT_FAILURE);
    }

    aux[0] = buffer[0];

    strcpy(&aux[1], &buffer[space_index + 1]);

    strcpy(var->username, aux);
}

void generate_password(User *var){

    int password = rand() % (MAX_PASSWORD - MIN_PASSWORD + 1) + MIN_PASSWORD;

    snprintf(var->password, PASSWORD_MAX_SIZE, "%d", password);
}

int read_number_of_users(int *var){

    char buffer[BUFFER_LEN_MAX_SIZE];
    char *end;

    if (!fgets(buffer, sizeof(buffer), stdin)){
        printf("Error to read Number of users! \n");
        exit (EXIT_FAILURE);
    }

    *var = (int) strtol(buffer, &end, NUMERIC_BASE_STRTOL);

    if (end == buffer || (*end != '\n' && *end != '\0')){
        printf("Invalid entry for Number of users! \n");
        exit (EXIT_FAILURE);
    }

    return *var;
}

int check_number_of_users(int *var){

    if (*var <= 0){
        printf("Invalid number of users! \n");
        exit (EXIT_FAILURE);
    }

    return 1;
}

void check_malloc(User *var){

    if (var == NULL){
        perror("malloc");
        exit (EXIT_FAILURE);
    }
}

void printtime(time_t t, char *buffer, size_t size){

    struct tm *tme = localtime(&t);

    strftime(buffer, size, "%d/%m/%Y - %H:%M:%S", tme);
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int recv_line(int sockfd, char *buffer, size_t maxlen) {
    size_t i = 0;
    char c;

    while (i < maxlen - 1) {
        ssize_t n = recv(sockfd, &c, 1, 0);
        if (n <= 0) return n;
        if (c == '\n') break;
        buffer[i++] = c;
    }

    buffer[i] = '\0';
    return i;
}

int handle_client(int newsockfd, FILE *file_message, FILE *file_connection, char *IPaddr) {

    struct sockaddr_storage their_addr;

    char buffer[MAXSIZEDATA];
    char username[USER_MAX_SIZE];
    char password[PASSWORD_MAX_SIZE];

    time_t t = time(NULL);
    char buff_time[BUFFER_LEN_MAX_SIZE];

    if (recv_line(newsockfd, buffer, MAXSIZEDATA) <= 0)
        return -1;

    if (strcmp(buffer, "100") != 0) {
        send(newsockfd, "server: 620 unknown command\n", 28, 0);
        return -1;
    }

    send(newsockfd, "server: 120 OK Send username\n", 30, 0);
    printf("server: sent 120 \n");

    if (recv_line(newsockfd, buffer, MAXSIZEDATA) <= 0){
        return -1;
    }

    if (strcmp(buffer, "200") != 0){
        send(newsockfd, "server: 620 unknown command\n", 28, 0);
        return -1;
    }

    if (recv_line(newsockfd, username, USER_MAX_SIZE) <= 0){
        return -1;
    }

    send(newsockfd, "server: 220 OK Send password\n", 29, 0);
    printf("server: sent 220 \n");

    if (recv_line(newsockfd, buffer, MAXSIZEDATA) <= 0){
        return -1;
    }

    if (strcmp(buffer, "300") != 0){
        send(newsockfd, "server: 620 unknown command\n", 28, 0);
        return -1;
    }

    if (recv_line(newsockfd, password, PASSWORD_MAX_SIZE) <= 0)
        return -1;

    if (!authenticate_user(DB_FILE_PATH, username, password)) {
        send(newsockfd, "server: 610 login failed\n", 26, 0);
        return -1;
    }

    send(newsockfd, "server: 320 login OK\n", 21, 0);

    printf("server: '%s' logged in \n", username);

    printtime(t, buff_time, sizeof(buff_time));
    fprintf(file_connection, "client: '%s' logged in from %s at %s \n", username, IPaddr, buff_time);

    printf("server: sent 320 \n");

    while (true) {

        if (recv_line(newsockfd, buffer, MAXSIZEDATA) <= 0)
            break;

        if (strcmp(buffer, "400") == 0) {

            if (recv_line(newsockfd, buffer, MAXSIZEDATA) <= 0)
                break;

            printf("server: '%s' message '%s'\n",username, buffer);
            printtime(t, buff_time, sizeof(buff_time));
            fprintf(file_message, "at %s client: '%s' from '%s' sent message: '%s'\n", buff_time, username, IPaddr, buffer);
            fflush(file_message);
            fprintf(file_message, "----------------------------------------------\n");
            fflush(file_message);

            send(newsockfd, "server: 420 OK\n", 15, 0);
            printf("server: sent 420 \n");
        }
        else if (strcmp(buffer, "500") == 0) {

            send(newsockfd, "server: 520 OK\n", 15, 0);
            printf("server: sent 520 \n");
            printf("server: client '%s' disconnected from server \n", username);
            break;
        }

        else {
            send(newsockfd, "server: 620 unknown command\n", 28, 0);
            printf("server: sent 620 \n");
        }
    }

    return 1;
}

void insert_users_into_db(void){

    FILE *file;
    User *varloc;

    int number_of_users;

    printf("Enter number of users: ");
    read_number_of_users(&number_of_users);
    check_number_of_users(&number_of_users);

    printf("\n");

    varloc = malloc(number_of_users * sizeof(User));
    check_malloc(varloc);

    file = fopen(DB_FILE_PATH, "a");

    if (file == NULL){
        perror("fopen");
        exit (EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_users; i++){

        printf("-------------- User Data Entry ---------------\n");

        generate_user_name(&varloc[i]);
        generate_password(&varloc[i]);
    }


    for (int j = 0; j < number_of_users; j++){

        fprintf(file, "User: %s Password: %s\n", varloc[j].username, varloc[j].password);
        fprintf(file, "-------------------------\n");
    }

    printf("Users inserted into data base! \n");

    fclose(file);
    free(varloc);
}

int read_choice(int *var){

    char buffer[BUFFER_LEN_MAX_SIZE];
    char *end;

    if (!fgets(buffer, sizeof(buffer), stdin)){
        printf("Error to read choice! \n");
        exit (EXIT_FAILURE);
    }

    *var = (int) strtol(buffer, &end, NUMERIC_BASE_STRTOL);

    if (end == buffer || (*end != '\n' && *end != '\0')){
        printf("Invalid entry for choice! \n");
        exit (EXIT_FAILURE);
    }

    return *var;
}