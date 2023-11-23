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
    int server_sock_fd, client_sock_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];

    // 创建server套接字
    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock_fd == -1){
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Enable address reuse option
    int reuseaddr = 1;
    // 设置套接字并启用 SO_REUSEADDR常用于TCP服务器
    if(setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1){
        perror("Setsockopt error");
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

    // listen(socket, backlog) backlog表示最多监听的连接数量?
    if(listen(server_sock_fd, 5) == -1){
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", SERVER_PORT);

    while(1){
        socklen_t client_addr_len = sizeof(client_addr);

        client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if(client_sock_fd == -1){
            perror("Accept error");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Accepted connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        while(1){
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            ssize_t received_bytes = read(client_sock_fd, buffer, sizeof(buffer));
            if(received_bytes <= 0){
                perror("Receive error or client disconnected");
                break;
            }
            // Exercise 3. Add your code:
            // Add your code here:
            if (strcmp(buffer, "bye") == 0) {
                printf("Client Disconnected!\n");
                break;
            }

            if (buffer[0] == 'i' || buffer[0] == 'I') {
                long n = atol(buffer + 2);
                n = n * n;
                sprintf(buffer, "%ld", n);
            }
            else if (buffer[0] == 'f' || buffer[0] == 'F') {
                float n = atof(buffer + 2);
                n = n * n;
                sprintf(buffer, "%f", n);
            }
            else {
                sprintf(buffer, "Sorry, please input the data like: i 1 or f 1");
            }

            if (write(client_sock_fd, buffer, sizeof(buffer)) == -1) {
                perror("Write error");
            }
            // TODO();
        }

        close(client_sock_fd);
    }

    close(server_sock_fd);

    return 0;
}
