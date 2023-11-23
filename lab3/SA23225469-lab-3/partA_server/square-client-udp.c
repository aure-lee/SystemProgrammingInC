#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUF_SIZE 1024
#define MAX_RETRY 5 // 最大重试次数

int main(){
    int client_sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    socklen_t server_addr_len = sizeof(server_addr);

    // client_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    client_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_sock_fd == -1){
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0){
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server by UDP. Enter 'bye' to exit.\n");

    struct timeval tv; // 设置超时重传时间
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (setsockopt(client_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        perror("Setsocket Error!");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("Enter a message: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove trailing newline character 查找字符串中第一个换行符，并用'\0'替换它

        int flag_exit = 0; // 0 默认 1 发送成功 2 bye
        for (int retry_count = 0; retry_count < MAX_RETRY; ++ retry_count) {
            if (sendto(client_sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send Error!");
                continue;
            }

            if (strcmp(buffer, "bye") == 0) {
                // printf("Exiting...\n");
                flag_exit = 2;
                break;
            }

            ssize_t received_bytes = recvfrom(client_sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &server_addr_len);
            if (received_bytes > 0) {
                buffer[received_bytes] = '\0';
                printf("Server response: %s\n", buffer);
                flag_exit = 1;
                break;
            }
            else {
                perror("Receive error or timeout");
                printf("Retrying...\n");
            }
        }

        if (flag_exit == 0) {
            printf("Sorry, failed to receive response from server!\n");
        }
        else if (flag_exit == 2) {
            printf("Exiting...\n");
            break;
        }
    }

    close(client_sock_fd);

    return 0;
}
