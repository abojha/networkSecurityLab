#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<openssl/aes.h>

#define PORT 8083
void printHex(const char *label, const char *data){
    printf("%s: ", label);
    for(int i = 0; i < AES_BLOCK_SIZE; i++){
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");
}



int main(){
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addr_len = sizeof(address);


    unsigned char buffer[AES_BLOCK_SIZE] = {0};
    unsigned char encrypted[AES_BLOCK_SIZE];
    unsigned char decrypted[AES_BLOCK_SIZE];

    unsigned char aes_key[AES_BLOCK_SIZE] = "examplekey123456";
    AES_KEY enc_key, dec_key;
    AES_set_encrypt_key(aes_key, 128, &enc_key);
    AES_set_decrypt_key(aes_key, 128,  &dec_key);


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
        memset(buffer, 0, AES_BLOCK_SIZE);

        int valread = read(client_fd, buffer, AES_BLOCK_SIZE);
        if(valread <= 0){
            printf("\n Client Disconnected\n");
            break;
        }
        printHex("Encrypted from Client", buffer);

        // decrypt the message
        AES_decrypt(buffer, decrypted, &dec_key);
        decrypted[AES_BLOCK_SIZE] = '\0';

        printf("\nMessage from Client :: %s\n", decrypted);



        // Send response to client
        printf("\nEnter Server Message\n");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        AES_encrypt(buffer, encrypted, &enc_key);


// Manually add a null terminator at the end of the block
        

        send(client_fd, encrypted, AES_BLOCK_SIZE, 0);


    }

    close(client_fd);
    close(server_fd);


}