#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "send_icmp.h"

unsigned char sendbuff[BUFFSIZE] = {'\0'};
unsigned char recvbuff[BUFFSIZE] = {'\0'};

tunnel *tnel;
int server_socket;
struct sockaddr_in dest_addr;
unsigned short total_len;

int main(){
    server_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    // server_socket = socket(AF_PACKET, SOCK_RAW, IPPROTO_ICMP);

    if (server_socket == -1) {
        perror("Create Socket Error");
        exit(EXIT_FAILURE);
    }

    char myname[NAMESIZE];
    printf("Enter your name: ");
    scanf("%s", myname);

    char whoname[NAMESIZE];
    printf("Who you want to communicate: ");
    scanf("%s", whoname);

    char dst_ip[16];
    printf("Enter the dest IP: ");
    scanf("%s", dst_ip);
    
    init_icmp_packet(myname, whoname, dst_ip);

    pid_t pid = fork();
    // 子进程处理发送来的数据包
    if(pid == 0){
        while(1){
            int n = recvfrom(server_socket, recvbuff, BUFFSIZE, 0, NULL, NULL);
            if (n == -1) {
                perror("Receive Error");
                exit(EXIT_FAILURE);
            }
            // 设置recvbuff的头部为iphdr 
            struct iphdr *iph = (struct iphdr*)(recvbuff);
            // recvbuff的ip报头之后设置icmphdr
            struct icmphdr *icmph = (struct icmphdr*)
                                    (recvbuff + sizeof(struct iphdr));
            // recvbuff剩余部分设置成tunnel类型，用来提取数据
            tunnel *recv = (tunnel *)(recvbuff + sizeof(struct iphdr)
                                      + sizeof(struct icmphdr)); 
            // 解码
            decrypt(recv->data, 7);
            // 过滤
            if(filter(icmph)) {
                printf("\t%s : ", recv->sname);
                printf("%s\n", recv->data); 
            }

            memset(recvbuff, 0, BUFFSIZE); // Clear the recvbuff
        }
    }
    
    // 主进程处理发送数据包
    while (1){
        struct icmphdr *icmph = (struct icmphdr *)(sendbuff);
        char *databuff = tnel->data;
        fgets(databuff, sizeof(tnel->data), stdin);
        databuff[strcspn(databuff, "\n")] = '\0';
        if (strlen(databuff) == 0)
            continue;
        senddata(databuff);
    }
    
    close(server_socket);
    return 0;
}