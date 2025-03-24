#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 8080

int main(){
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    char buffer[1024] = {0};


    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd <= 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, addr_len) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is Listening");

    if(listen(server_fd, 3) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }


    client_fd = accept(server_fd, (struct sockaddr *)&address, &addr_len);
    

    if(client_fd < 0){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    while(1){
        memset(buffer, 0, sizeof(buffer));

        int valread = read(client_fd, buffer, sizeof(buffer));
        if(valread <= 0){
            printf("\n Client Disconnected\n");
            break;
        }

        printf("\nClient :: %s\n", buffer);

        // Send response to client
        printf("\nEnter Server Message\n");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        send(client_fd, buffer, sizeof(buffer), 0);


    }

    close(client_fd);
    close(server_fd);


    


}