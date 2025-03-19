#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 8080

int main(){

    int sock;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    char buffer[1024] = {0};


    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        perror("Socket connection failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port  = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;
    
    if(connect(sock, (struct sockaddr *)&address, addr_len) < 0){
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    printf("\nServer Connected\n");

   while(1){
        // send message to server
        printf("\nEnter message to send server\n");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        send(sock, buffer, strlen(buffer), 0);

        if(strcmp(buffer, "exit") == 0){
            printf("\nClient exiting\n");
            break;
        }

        // Receive message from server
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer));

        if(valread <= 0){
            printf("Server Disconnected\n");
            break;
        }

        printf("Server: %s", buffer);

   }
   close(sock);
   return 0;


}