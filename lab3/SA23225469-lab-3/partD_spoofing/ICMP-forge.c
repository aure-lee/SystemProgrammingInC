#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <linux/icmp.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)


/* for each ioctl keep diffrent ifreq structure 
   otherwise error may come in sending(sendto ) */
struct ifreq ifreq_c, ifreq_i, ifreq_ip;
int sock_raw;
unsigned char *sendbuff;

#define DESTMAC0 0x00
#define DESTMAC1 0x0c
#define DESTMAC2 0x29
#define DESTMAC3 0xe1
#define DESTMAC4 0x8c
#define DESTMAC5 0xaa

#define destination_ip "192.168.117.138"

int total_len = 0, send_len;

// 处理数据链路层索引
void get_eth_index() {
	memset(&ifreq_i, 0, sizeof(ifreq_i));

	// 复制字符串固定字节
	strncpy(ifreq_i.ifr_name, "eth0", IFNAMSIZ - 1);
	if ((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0) {
		perror("error in index ioctl reading");
		exit(EXIT_FAILURE);
	}

	printf("Index = %d\n", ifreq_i.ifr_ifindex);
}

// 处理MAC层地址
void get_mac() {
	memset(&ifreq_c, 0, sizeof(ifreq_c));

	strncpy(ifreq_c.ifr_name, "eth0", IFNAMSIZ - 1);

	// 获取sock_raw的MAC地址
	if ((ioctl(sock_raw, SIOCGIFHWADDR, &ifreq_c)) < 0) {
		perror("error in SIOCGIFHWADDR ioctl reading");
		exit(EXIT_FAILURE);
	}

	printf("Mac = %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", 
			(unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]), 
			(unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]), 
			(unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]), 
			(unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]), 
			(unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]), 
			(unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]));
	printf("Ethernet Packaging Start ... \n");

	struct ethhdr *eth = (struct ethhdr *)(sendbuff);

	// 把sock_raw的硬件地址赋值给sendbuff的源地址
	eth->h_source[0] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]);
	eth->h_source[1] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]);
	eth->h_source[2] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]);
	eth->h_source[3] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]);
	eth->h_source[4] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]);
	eth->h_source[5] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]);

	// 把目的地址赋值给sendbuff的目的地址
	eth->h_dest[0] = DESTMAC0;
	eth->h_dest[1] = DESTMAC1;
	eth->h_dest[2] = DESTMAC2;
	eth->h_dest[3] = DESTMAC3;
	eth->h_dest[4] = DESTMAC4;
	eth->h_dest[5] = DESTMAC5;

	// 设置协议为IP
	eth->h_proto = htons(ETH_P_IP); 

	printf("Ethernet Packaging Done.\n");
	// 总字符长度加上数据链路层头部
	total_len += sizeof(struct ethhdr);
}

unsigned short checksum(unsigned char *buf, int size) {
	unsigned int checkSum = 0;
	for (int i = 0; i < size; i += 2) {
		unsigned short first = (unsigned short)buf[i] << 8;
		unsigned short second = (unsigned short)buf[i + 1] & 0x00ff;
		checkSum += first + second;
	}
	while (1) {
		unsigned short c = (checkSum >> 16);
		if (c > 0) {
			checkSum = (checkSum << 16) >> 16;
			checkSum += c;
		}
		else 
			break;
	}
	return ~checkSum;
}

void get_icmp() {
	struct icmphdr *icmp = (struct icmphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));
	icmp->type = 8; // ICMP type: 8 is request, 0 is reply
	icmp->checksum = 0;
	icmp->checksum = htons(checksum((unsigned char *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr)), sizeof(struct icmphdr)));
	total_len += sizeof(struct icmphdr);
}

void get_ip() {
	memset(&ifreq_ip, 0, sizeof(ifreq_ip));
	strncpy(ifreq_ip.ifr_name, "eth0", IFNAMSIZ - 1);

	if (ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0) {
		perror("error in SIOCGIFADDR");
		exit(EXIT_FAILURE);		
	}

	printf("Source IP Address: %s\n", inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
	
	struct iphdr *iph = (struct iphdr *)(sendbuff + sizeof(struct ethhdr));
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 16;
	iph->id = htons(10201);
	iph->ttl = 64;
	iph->protocol = 1; // ICMP
	iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
	iph->daddr = inet_addr(destination_ip); // put destination IP address
	printf("Destination IP Address: %.2X\n", iph->daddr);
	printf("Destination IP Address: %s\n", destination_ip);

	total_len += sizeof(struct iphdr);

	get_icmp();

	iph->tot_len = htons(total_len - sizeof(struct ethhdr));
	iph->check = htons(checksum((unsigned char *)(sendbuff + sizeof(struct ethhdr)), sizeof(struct iphdr)));
}

int main() {
	sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if (sock_raw == -1) {
		perror("Create Socket Error");
		exit(EXIT_FAILURE);
	}

	sendbuff = (unsigned char *)malloc(64); // increase in case of large data.
	memset(sendbuff, 0, 64);

	get_eth_index(); // interface number
	get_mac();
	get_ip();

	struct sockaddr_ll sadr_ll;
	sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex;
	sadr_ll.sll_halen = ETH_ALEN;
	sadr_ll.sll_addr[0] = DESTMAC0;
	sadr_ll.sll_addr[1] = DESTMAC1;
	sadr_ll.sll_addr[2] = DESTMAC2;
	sadr_ll.sll_addr[3] = DESTMAC3;
	sadr_ll.sll_addr[4] = DESTMAC4;
	sadr_ll.sll_addr[5] = DESTMAC5;

	printf("Sending...\n");
	while (1) {
		// send_len = sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll));
		// if (send_len < 0) {
		// 	printf("Error in Sending....\nSendlen=%d....Errno=%d\n", send_len, errno);
		// 	return -1;
		// }
		
		// 使用 PartC 中完成的 Sniff 检测
		if (sendto(sock_raw, sendbuff, 64, 0, (const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll)) == -1) {
			perror("Send Error");
			exit(EXIT_FAILURE);
		}
		else {
			sleep(1);
		}
	}
}
