#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)


#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "send_icmp.h"

extern unsigned char sendbuff[BUFFSIZE];
extern unsigned char recvbuff[BUFFSIZE];

extern tunnel *tnel;
extern int server_socket;
extern struct sockaddr_in dest_addr;
extern unsigned short total_len;

static unsigned short icmp_checksum(
                unsigned short *ptr, int nbytes) {
    long sum;
    unsigned short oddbyte;
    unsigned short answer;
    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((uint8_t *) &oddbyte) = *(uint8_t *)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

// 封装ICMP报文，设置icmp报头，同时设置目的ip地址
void init_icmp_packet(const char *myname,
                     const char *whoname, const char *dst_ip) {

    tnel = (tunnel *)malloc(sizeof(tunnel));
    memcpy(tnel->sname, myname, strlen(myname));
    memcpy(tnel->dname, whoname, strlen(whoname));

    struct icmphdr *icmph = (struct icmphdr *)(sendbuff);
    icmph->type = ICMP_ECHO;
    icmph->code = 0;
    icmph->un.echo.id = htons(1);
    icmph->un.echo.sequence = htons(1);
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dst_ip, &dest_addr.sin_addr);
}

int senddata(char *data){
    // Exercise 2.
    // Add your code here:
    encrypt(data, 7);
    struct icmphdr *icmph = (struct icmphdr *)(sendbuff);
    tunnel *send = (tunnel *)(sendbuff + sizeof(struct icmphdr));
    memcpy(send, tnel, sizeof(tunnel));

    icmph->checksum = 0;
    icmph->checksum = icmp_checksum((unsigned short *)sendbuff, sizeof(struct icmphdr));

    int ret = sendto(server_socket, sendbuff, sizeof(struct icmphdr) + sizeof(tunnel),
                0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (ret == -1) {
        perror("Send Error");
        return -1;
    }
    // TODO();
    return 0;
}

// 过滤 非回声ICMP报文 和 发送源name与接收目的name不同 的报文
int filter(struct icmphdr *icmph){
    // 从ip报头和icmp报头之后开始读取信息
    tunnel *recv = (tunnel *)(recvbuff + sizeof(struct iphdr)
                                      + sizeof(struct icmphdr));
    if (icmph->type != ICMP_ECHO || (strcmp(recv->dname, tnel->sname) != 0)) {
        // printf("%s, %s\n", recv->dname, tnel->sname);
        // char *s1 = recv->dname, *s2 = tnel->sname;
        // while (*s1) {
        //     printf("%02x ", (unsigned char) *s1);
        //     s1++;
        // }
        // printf("\n");
        // while (*s2) {
        //     printf("%02x ", (unsigned char) *s2);
        //     s2++;
        // }
        // printf("\n");
        // printf("%d\n", t);
        // printf("%s, %s\n", recv->dname, tnel->sname);
        return 0;
    }
    return 1;
}

// 凯撒加密
void encrypt(char *text, int shift){
    char *ptr = text;
    shift = shift % 26;

    while (*ptr) {
        if (isalpha((unsigned char)*ptr)) {
            char offset = isupper((unsigned char)*ptr) ? 'A' : 'a'; // 如果当前字母是大写的，偏移量基准在'A'，否则在'a'
            *ptr = (char)((*ptr - offset + shift) % 26 + offset);
        }
        ptr++;
    }
}

// 凯撒解密
void decrypt(char *text, int shift){
    char *ptr = text;
    shift = shift % 26;

    while (*ptr) {
        if (isalpha((unsigned char)*ptr)) {
            char offset = isupper((unsigned char)*ptr) ? 'A' : 'a'; // 如果当前字母是大写的，偏移量基准在'A'，否则在'a'
            *ptr = (char)((*ptr - offset - shift + 26) % 26 + offset);
        }
        ptr++;
    }
}