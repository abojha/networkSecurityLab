#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>

#define PORT 8080

int main(){
    int sock;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    char buffer[1024] = {0};


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(connect(sock, (struct sockaddr *) &address, addr_len) < 0){
        perror("Connect Failed");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        select(sock + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(STDIN_FILENO, &readfds)){
            printf("\n Enter Client Message :: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            send(sock, buffer, strlen(buffer), 0);
            if(strcpy(buffer, "exit") == 0){
                printf("Client Disconnected\n");
                close(sock);
                break;
            }

        }

        if(FD_ISSET(sock, &readfds)){
            memset(buffer, 0, sizeof(buffer));

            int valread = read(sock, buffer, sizeof(buffer));
            if(valread == 0){
                printf("Server Disconnected\n");
                break;
            }
            buffer[valread] = '\0';
            printf("Message Received %s\n", buffer);
        }
    }
    close(sock);
}