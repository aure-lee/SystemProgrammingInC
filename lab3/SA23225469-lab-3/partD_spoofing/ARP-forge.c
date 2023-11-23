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
#include <net/if_arp.h>
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

#define DESTMAC0 0xff
#define DESTMAC1 0xff
#define DESTMAC2 0xff
#define DESTMAC3 0xff
#define DESTMAC4 0xff
#define DESTMAC5 0xff

#define destination_ip "192.168.117.138"

int total_len = 0, send_len;

void get_eth_index() {
	memset(&ifreq_i, 0, sizeof(ifreq_i));

	// Replace ens33 with your network interface
	strncpy(ifreq_i.ifr_name, "eth0", IFNAMSIZ - 1);
	if ((ioctl(sock_raw, SIOCGIFINDEX, &ifreq_i)) < 0) {
		perror("Error in Index Ioctl Reading");
		exit(EXIT_FAILURE);
	}

	printf("Index = %d\n", ifreq_i.ifr_ifindex);
}

void get_mac() {
	memset(&ifreq_c, 0, sizeof(ifreq_c));

	// Replace ens33 with your network interface
	strncpy(ifreq_c.ifr_name, "eth0", IFNAMSIZ - 1);

	// 获取sock_raw的MAC地址
	if ((ioctl(sock_raw, SIOCGIFHWADDR, &ifreq_c)) < 0) {
		perror("Error in SIOCGIFHWADDR Ioctl Reading");
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

	// 设置协议为ARP
	eth->h_proto = htons(ETH_P_ARP);

	printf("Ethernet Packaging Done.\n");

	total_len += sizeof(struct ethhdr);
}

void get_arp() {
	struct ether_arp *arp = (struct ether_arp *)(sendbuff + sizeof(struct ethhdr));
	arp->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
	arp->ea_hdr.ar_pro = htons(ETH_P_IP);
	arp->ea_hdr.ar_hln = '6';
	arp->ea_hdr.ar_pln = '4';
	arp->ea_hdr.ar_op = htons(ARPOP_REQUEST); // ARP request
	/* sender hardware address */
	// Exercise 4: Complete ARP_forge.c in your project to achieve the forgery of ARP protocol packets:
	// Add your code here:
	struct ethhdr *eth = (struct ethhdr *)(sendbuff);
	// arp->arp_sha = eth->h_source;
	memcpy(arp->arp_sha, eth->h_source, 6);
	printf("ARP Sender Hardware Address = %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", 
		(unsigned char)(arp->arp_sha[0]), 
		(unsigned char)(arp->arp_sha[1]), 
		(unsigned char)(arp->arp_sha[2]), 
		(unsigned char)(arp->arp_sha[3]), 
		(unsigned char)(arp->arp_sha[4]), 
		(unsigned char)(arp->arp_sha[5]));
	// TODO();

	/* sender protocol address */
	// Exercise 4: Complete ARP_forge.c in your project to achieve the forgery of ARP protocol packets:
 	// Add your code here:
	memset(&ifreq_ip, 0, sizeof(ifreq_ip));
	strncpy(ifreq_ip.ifr_name, "eth0", IFNAMSIZ - 1);

	if (ioctl(sock_raw, SIOCGIFADDR, &ifreq_ip) < 0) {
		perror("error in SIOCGIFADDR");
		exit(EXIT_FAILURE);		
	}

	struct sockaddr_in *source_ip;
	source_ip = (struct sockaddr_in*)&ifreq_ip.ifr_addr;
	memcpy(arp->arp_spa, &source_ip->sin_addr, 4);
	printf("ARP Sender Protocol Address = %d.%d.%d.%d\n", 
		(unsigned char)(arp->arp_spa[0]), 
		(unsigned char)(arp->arp_spa[1]), 
		(unsigned char)(arp->arp_spa[2]), 
		(unsigned char)(arp->arp_spa[3]));
 	// TODO();

	/* target hardware address */
	// Exercise 4: Complete ARP_forge.c in your project to achieve the forgery of ARP protocol packets:
	// Add your code here:

	memcpy(arp->arp_tha, eth->h_dest, 6);
	printf("ARP Target Hardware Address = %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", 
		(unsigned char)(arp->arp_tha[0]), 
		(unsigned char)(arp->arp_tha[1]), 
		(unsigned char)(arp->arp_tha[2]), 
		(unsigned char)(arp->arp_tha[3]), 
		(unsigned char)(arp->arp_tha[4]), 
		(unsigned char)(arp->arp_tha[5]));
	// TODO();

	/* target protocol address */
	// Exercise 4: Complete ARP_forge.c in your project to achieve the forgery of ARP protocol packets:
	// Add your code here:
	struct in_addr target_ip;
	inet_aton(destination_ip, &target_ip);
	memcpy(arp->arp_spa, &target_ip, 4);
	printf("ARP Target Protocol Address = %d.%d.%d.%d\n", 
		(unsigned char)(arp->arp_spa[0]), 
		(unsigned char)(arp->arp_spa[1]), 
		(unsigned char)(arp->arp_spa[2]), 
		(unsigned char)(arp->arp_spa[3]));
	// TODO();
}

int main() {
	sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sock_raw == -1) {
		perror("Socket Create Error");
		exit(EXIT_FAILURE);
	}

	sendbuff = (unsigned char *)malloc(64); // increase in case of large data.
	memset(sendbuff, 0, 64);

	get_eth_index(); // interface number
	get_mac();
	get_arp();

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
		// send_len = sendto(sock_raw, sendbuff, 64, 0, 
		// 				(const struct sockaddr *)&sadr_ll, sizeof(struct sockaddr_ll));
		// if (send_len < 0) {
		// 	printf("Error in Sending....\nSendlen = %d....\tErrno = %d\n", 
		// 			send_len, errno);
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
