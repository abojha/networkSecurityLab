#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<openssl/des.h>


#define PORT 8081
#define BUFFER_SIZE 1024

// Make schedule and key
DES_key_schedule keySchedule;
DES_cblock key = "mysecret";


void encrypt_decrypt(char *input, char *output, int encrypt, int *out_len){
    DES_cblock ivec = "12345678";
    int len = strlen(input) + 1;
    int padded_len = (len % 8 == 0) ? len : ((len + 8) - (len % 8));
    *out_len = padded_len;

    DES_ncbc_encrypt((unsigned char *)input, (unsigned char *)output, padded_len, &keySchedule, &ivec, encrypt);
}

void printHex(const char *label, const char *data, int len){
    printf("%s: ", label);
    for(int i = 0; i < len; i++){
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");
}

int main(){
    DES_set_key_checked(&key, &keySchedule);

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

        printHex("Client Encrypted Message", buffer, 16);

        char decrypted[BUFFER_SIZE];
        int out_len;

        encrypt_decrypt(buffer, decrypted, DES_DECRYPT, &out_len);
        printf("Client (Decrypted) : %s\n", decrypted);

        // Send response to client
        printf("\nEnter Server Message\n");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
            
        char encrypted[BUFFER_SIZE];
        encrypt_decrypt(buffer, encrypted, DES_ENCRYPT, &out_len);

        printHex("Encrypted Message send by server\n", encrypted, out_len);

        send(client_fd, encrypted, out_len, 0);


    }

    close(client_fd);
    close(server_fd);

}