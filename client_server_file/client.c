#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main(){
    int sock;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};
    char filename[256];

    // Create client socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // change to server IP if needed
    address.sin_port = htons(PORT);

    if(connect(sock, (struct sockaddr *) &address, addr_len) < 0){
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    int max_sd;
    printf("Connected to server on port %d\n", PORT);

    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);
        max_sd = sock;

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        // Check for user input (filename)
        if(FD_ISSET(STDIN_FILENO, &readfds)){
            printf("Enter filename to upload (or 'exit' to disconnect): ");
            if(fgets(filename, sizeof(filename), stdin) == NULL)
                continue;
            filename[strcspn(filename, "\n")] = '\0';

            if(strcmp(filename, "exit") == 0){
                printf("Disconnecting...\n");
                break;
            }

            // Open file
            FILE *fp = fopen(filename, "r");
            if(fp == NULL){
                perror("Failed to open file");
                continue;
            }
            // Read file content into buffer
            size_t n = fread(buffer, 1, MAX_BUFFER_SIZE - 1, fp);
            buffer[n] = '\0';
            fclose(fp);

            // Send file content to server
            send(sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, MAX_BUFFER_SIZE);
        }

        // Check for incoming messages (processed file content)
        if(FD_ISSET(sock, &readfds)){
            int valread = read(sock, buffer, MAX_BUFFER_SIZE - 1);
            if(valread <= 0){
                printf("Server disconnected\n");
                break;
            }
            buffer[valread] = '\0';
            printf("Modified file content received:\n%s\n", buffer);
            memset(buffer, 0, MAX_BUFFER_SIZE);
        }
    }
    close(sock);
    return 0;
}
