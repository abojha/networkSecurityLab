#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

int client_sockets[MAX_CLIENTS] = {0};

// Process file content by keeping only alphabetic characters
void process_file(char *input, char *output) {
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (isalpha((unsigned char)input[i])) {
            output[j++] = input[i];
        }
    }
    output[j] = '\0';
}

int main(){
    int server_fd, client_fd, max_sd, activity, valread, sd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};
    char processed[MAX_BUFFER_SIZE] = {0};

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, addr_len) < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_CLIENTS) < 0){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    printf("Server listening on port %d\n", PORT);

    while(1){
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets to set
        for(int i = 0; i < MAX_CLIENTS; i++){
            sd = client_sockets[i];
            if(sd > 0)
                FD_SET(sd, &readfds);
            if(sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if(activity < 0){
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // New connection
        if(FD_ISSET(server_fd, &readfds)){
            client_fd = accept(server_fd, (struct sockaddr *)&address, &addr_len);
            if(client_fd < 0){
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }
            for(int i = 0; i < MAX_CLIENTS; i++){
                if(client_sockets[i] == 0){
                    client_sockets[i] = client_fd;
                    printf("New client connected: socket fd %d\n", client_fd);
                    break;
                }
            }
        }

        // Check for IO on each client socket
        for(int i = 0; i < MAX_CLIENTS; i++){
            sd = client_sockets[i];
            if(sd > 0 && FD_ISSET(sd, &readfds)){
                valread = read(sd, buffer, MAX_BUFFER_SIZE - 1);
                if(valread <= 0){
                    printf("Client disconnected: socket fd %d\n", sd);
                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("Received file content from client (socket fd %d):\n%s\n", sd, buffer);
                    // Process file content
                    process_file(buffer, processed);
                    printf("Processed file content:\n%s\n", processed);
                    // Broadcast the processed file content to all connected clients
                    for(int j = 0; j < MAX_CLIENTS; j++){
                        if(client_sockets[j] != 0){
                            send(client_sockets[j], processed, strlen(processed), 0);
                        }
                    }
                    memset(buffer, 0, MAX_BUFFER_SIZE);
                    memset(processed, 0, MAX_BUFFER_SIZE);
                }
            }
        }
    }
    close(server_fd);
    return 0;
}
