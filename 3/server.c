#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<netinet/in.h>


#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS] = {0};

void handle_message(int sender, char *message){

    if(strncmp(message, "ALL ", 4) == 0){
        char broadcast_msg[1024] = {0};
        snprintf(broadcast_msg, sizeof(broadcast_msg), "Client %d :: %s", sender, message + 4);
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(client_sockets[i] != 0 && client_sockets[i] != sender){
                send(client_sockets[i], message, strlen(message), 0);
                printf("\n\nMessage Broadcasted\n\n");
            }
        }
    }
    else if(message[0] == '@'){
        int target_id = atoi(message + 1);
        char broadcast_msg[1024] = {0};
        snprintf(broadcast_msg, sizeof(broadcast_msg), "Client %d :: %s", sender, message + 3);
        if(target_id >= 0 && target_id < MAX_CLIENTS && client_sockets[target_id] != 0){
            send(client_sockets[target_id], message, strlen(message), 0);
            printf("\n\nMessage Sent to Client :: %d", target_id);
        }

    }

}


int main(){
    int server_fd, client_fd, max_sd, activity, valread, sd;
    struct sockaddr_in address;
    int add_len = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, add_len) < 0){
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_CLIENTS) < 0){
        perror("Listen Failed");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        max_sd = server_fd;

        for(int i = 0; i < MAX_CLIENTS; i++){
            sd = client_sockets[i];
            if(sd > 0) FD_SET(sd, &readfds);
            if(sd > max_sd) max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if(activity < 0){
            perror("Failed");
            exit(EXIT_FAILURE);
        }

        if(FD_ISSET(server_fd, &readfds)){
            client_fd = accept(server_fd, (struct sockaddr *)&address, &add_len);
            if(client_fd < 0){
                perror("Accept Failed");
                exit(EXIT_FAILURE);
            }


            for(int i = 0; i < MAX_CLIENTS; i++){
                sd = client_sockets[i];
                if(sd == 0){
                    client_sockets[i] = client_fd;
                    printf("\nNew Client Connect\n");
                    break;
                }
            }
        }

        for(int i = 0; i < MAX_CLIENTS; i++){
            sd = client_sockets[i];
            if(FD_ISSET(sd, &readfds)){
                valread = read(sd, buffer, sizeof(buffer));
                if(valread == 0){
                    printf("\nClient Disconnected\n");
                    close(sd);
                    client_sockets[i] = 0;
                }
                else{
                    buffer[valread] = '\0';
                    handle_message(sd, buffer);
                }
            }
        }
    }
    close(server_fd);
    close(client_fd);

}