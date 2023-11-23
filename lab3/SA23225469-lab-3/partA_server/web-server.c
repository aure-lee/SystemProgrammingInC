#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)




#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 1024
#define HTML_404 "<h1>404 Not Found</h1>"
#define DEL_HTML_202 "<h1>File is deleted!</h1>"
#define DEL_HTML_204 "<h1>File does not exist</h1>"
#define MAX_URL_LENGTH 1000 // 比BUF_SIZE要小

struct arg_t {
    int client_sock;
    char web_root[256];
};

void serve_client(int client_sock, const char* web_root){
    char request[BUF_SIZE];
    ssize_t bytes_received = read(client_sock, request, sizeof(request) - 1);
    if(bytes_received < 0){
        perror("read");
        return;
    }
    request[bytes_received] = '\0';

    if (strlen(request) > MAX_URL_LENGTH) {
        char response[] = "HTTP/1.1 414 URL Too Long\r\nContent-Length: 0\r\n\r\n";
        write(client_sock, response, strlen(response));
        close(client_sock);
        return;
    }

    // Parse the HTTP request, extract the requested file path, and handle the request.
    printf("Received request:\n%s\n", request);

    char method[16], path[128];
    int scanResult = sscanf(request, "%s %s", method, path);
    if(scanResult != 2){
        char response[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        write(client_sock, response, strlen(response));
        close(client_sock);
        return;  
    }

    char fullpath[256];
    snprintf(fullpath, sizeof(fullpath), "%s%s", web_root, path);  

    // Handle GET requests for serving files.
    if(strcasecmp(method, "GET") == 0){

        // Exercise 5.
        // Add your code here:
        char response[BUF_SIZE] = {'\0'};
        int file_fd = open(fullpath, O_RDONLY);
        if (file_fd < 0) {
            sprintf(response,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n\r\n"
            HTML_404, strlen(HTML_404));
            write(client_sock, response, strlen(response));
            return;
        }
        else {
            // Prepare header and send the file content
            struct stat st;
            fstat(file_fd, &st); // 检索文件属性，获得文件大小
            sprintf(response, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n\r\n", st.st_size);
            write(client_sock, response, strlen(response));
            
            // Sending file content
            while ((bytes_received = read(file_fd, response, BUF_SIZE)) > 0) {
                write(client_sock, response, bytes_received);
            }
            close(file_fd);
        }
        // TODO();

    }
    // Handle DELETE requests for deleting files.
    // 发送DELETE请求：curl -X DELETE http://localhost:8080/index.html
    // nc 127.0.0.1 8080
    // DELETE /index.html HTTP /1.1
    else if(strcasecmp(method, "DELETE") == 0){
        // Exercise 6: Implement DELETE here
        // Add your code here:
        char response[BUF_SIZE] = {'\0'};
        int ret = unlink(fullpath);
        if (ret == 0) {
            sprintf(response,
            "HTTP/1.1 202 Accepted\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n\r\n"
            DEL_HTML_202, strlen(DEL_HTML_202));
            write(client_sock, response, strlen(response));
            return;
        }
        else {
            perror("Error in unlink");
            sprintf(response,
            "HTTP/1.1 204 No Content\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n\r\n"
            DEL_HTML_204, strlen(DEL_HTML_204));
            write(client_sock, response, strlen(response));
            return;
        }
        // TODO();

    }else{
        char response[] = "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n";
        write(client_sock, response, strlen(response));
    }

    close(client_sock);
}

void *start(void *a) {
    struct arg_t *arg = a;
    int client_sock = arg->client_sock;
    char *web_root = arg->web_root;
    serve_client(client_sock, web_root);
    pthread_exit(a);
}

int main(){
    char web_root[256];
    strcpy(web_root, getenv("HOME"));
    strcat(web_root, "/mywebsite");

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int reuseaddr = 1;
    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1){
        perror("Setsockopt error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if(bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(server_sock, 5) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    while(1){
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        
        // serve_client(client_sock, web_root);

        // Challenge: multiple process
        // Add your code here:
        // pid_t pid = fork();
        // if (pid < 0) {
        //     perror("fork failed");
        //     exit(EXIT_FAILURE);
        // } else if (pid == 0) {
        //     // close(server_fd);  // child doesn't need the listener
        //     serve_client(client_sock, web_root);
        //     exit(EXIT_SUCCESS);
        // } else {
        //     close(client_sock);  // parent doesn't need this client's socket
        // }

        pthread_t tid;
        struct arg_t *arg = malloc(sizeof(*arg));
        arg->client_sock = client_sock;
        strncpy(arg->web_root, web_root, sizeof(arg->web_root));

        if (pthread_create(&tid, 0, start, arg) != 0) {
            perror("Could not create thread");
            close(client_sock);
            continue;
        }
        
        // TODO();

    }

    close(server_sock);
    return 0;
}
