#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define SERVER_PORT 12345
#define BUF_SIZE 1024

int main(){
    // UDP 不需要知道client的socket
    int server_sock_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    socklen_t client_addr_len = sizeof(client_addr);

    // 创建server套接字
    server_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_sock_fd == -1){
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 设置服务器端的address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有可用的网络接口上的客户端活动

    // bind fd and address
    if(bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on port %d...\n", SERVER_PORT);

    while(1){
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        ssize_t received_bytes = recvfrom(server_sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (received_bytes == -1) {
            perror("Receive error");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Received packet from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        if (strcmp(buffer, "bye") == 0) {
            printf("Exit the client!\n");
            continue;
        }

        if (buffer[0] == 'i') {
            long n = atol(buffer + 2);
            n = n * n;
            sprintf(buffer, "%ld", n);
        }
        else if (buffer[0] == 'f') {
            float n = atof(buffer + 2);
            n = n * n;
            sprintf(buffer, "%f", n);
        }
        else {
            sprintf(buffer, "Sorry, please input the data like: i 1 or f 1");
        }

        // 向server_sock_fd中写入buffer
        if (sendto(server_sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
            perror("Send error");
        }
        // TODO();
    }

    close(server_sock_fd);

    return 0;
}
