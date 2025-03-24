#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/des.h>

#define PORT 8081
#define BUF_SIZE 1024

DES_key_schedule keySchedule;
DES_cblock key = "mysecret"; // 8-byte key

void encrypt_decrypt(const char *input, char *output, int encrypt, int *out_len) {
DES_cblock ivec = "12345678"; // 8-byte IV
int len = strlen(input) + 1; // +1 for NULL terminator
int padded_len = (len % 8 == 0) ? len : (len + 8 - (len % 8));
*out_len = padded_len; // Store output length

DES_ncbc_encrypt((unsigned char *)input, (unsigned char *)output, padded_len, &keySchedule,
&ivec, encrypt);
}

void print_hex(const char *label, const char *data, int len) {
printf("%s: ", label);
for (int i = 0; i < len; i++) {
printf("%02x ", (unsigned char)data[i]);
}
printf("\n");
}

int main() {
DES_set_key_checked(&key, &keySchedule);

int server_fd, new_socket;
struct sockaddr_in address;
int addrlen = sizeof(address);
char buffer[BUF_SIZE] = {0}, encrypted[BUF_SIZE];

if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
perror("Socket failed");
exit(EXIT_FAILURE);
}

address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);

if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
perror("Bind failed");
exit(EXIT_FAILURE);
}

if (listen(server_fd, 3) < 0) {
perror("Listen failed");
exit(EXIT_FAILURE);
}

printf("Server listening on port %d...\n", PORT);

if ((new_socket = accept(server_fd, (struct sockaddr )&address, (socklen_t)&addrlen)) < 0) {
perror("Accept failed");
exit(EXIT_FAILURE);
}

while (1) {
memset(buffer, 0, BUF_SIZE);
read(new_socket, buffer, BUF_SIZE);

printf("\n--- Received Data ---\n");
print_hex("Encrypted", buffer, 16); // Print first 16 bytes

char decrypted[BUF_SIZE];
int out_len;
encrypt_decrypt(buffer, decrypted, DES_DECRYPT, &out_len);
printf("Client (Decrypted): %s\n", decrypted);

if (strcmp(decrypted, "exit") == 0) {
printf("Closing connection...\n");
break;
}

printf("Server: ");
fgets(buffer, BUF_SIZE, stdin);
buffer[strcspn(buffer, "\n")] = 0;

encrypt_decrypt(buffer, encrypted, DES_ENCRYPT, &out_len);
printf("\n--- Sending Data ---\n");
print_hex("Encrypted", encrypted, out_len);

send(new_socket, encrypted, out_len, 0);
}

close(new_socket);
close(server_fd);
return 0;
}