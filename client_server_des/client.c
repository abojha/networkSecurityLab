#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<openssl/des.h>

#define PORT 8081

DES_key_schedule keySechedule;
DES_cblock key = "mysecret";

void encrypt_decrypte(char *input, char *output, int encrypt, int *out_len){
    DES_cblock ivec = "12345678";
    int len = strlen(input) + 1;
    int pad_len = (len % 8 == 0) ? len : ((len + 8) - (len % 8));
    *out_len = pad_len;

    DES_cbc_encrypt((unsigned char *)input, (unsigned char *)output, pad_len, &keySechedule, &ivec, encrypt);
}

void printHex(const char *label, const char *data, int len){
    printf("%s: ", label);
    for(int i = 0; i < len; i++){
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");
}

int main(){

    DES_set_key_checked(&key, &keySechedule);
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
        char decrypted[1024];
        int out_len;
        char encrypted[1024];

        // send message to server
        printf("\nEnter message to send server\n");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        encrypt_decrypte(buffer, encrypted, DES_ENCRYPT, &out_len);
        printHex("Encrypted Message = %s", encrypted, out_len);
        send(sock, encrypted, out_len, 0);


        
        // Receive message from server
        
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer));

        if(valread <= 0){
            printf("Server Disconnected\n");
            break;
        }

        printHex("Encrypted from Server: %s", buffer, 16);

        
        encrypt_decrypte(buffer, decrypted, DES_DECRYPT, &out_len);
        printf("Server (Decrypted) : %s\n", decrypted);

   }
   close(sock);
   return 0;


}