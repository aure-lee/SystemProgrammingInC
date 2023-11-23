#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TODO()\
do{\
    printf("Add your code here: file %s, line %d\n", __FILE__, __LINE__);\
}while(0)



// socket
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
// ETH_P_ALL
#include <net/ethernet.h>
// ioctl
#include <sys/ioctl.h>
// struct ifeq
#include <net/if.h>
#include "packet-analyze.h"

// if you set the default port to 80, then you are
// required to gain the root privilege to bind this port.
#define HOST_PORT 12345
#define DEBUG 1
#define BUF_SIZE 65536
#define MAX_ROUND 1000
static char buf[BUF_SIZE];

// create the socket (set up the required flag for mode):
int raw_init (const char *device){
    // Exercise 1: Create a raw socket and enable promiscuous mode
    // Add your code here:
    /* Open A Raw Socket */
    int raw_socket = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
    if (raw_socket < 1){
        printf ("ERROR: Could not open socket, Got #?\n");
        exit (1);
    }

    /* Set the device to use */
    struct ifreq ifr; // ifreq允许获取和设置网络接口的参数，如interface name
    memset (&ifr, 0, sizeof (struct ifreq));
    strcpy (ifr.ifr_name, device);
    
    /* Get the current flags that the device might have */
    // 获取指定网络接口raw_socket的标志 Socket IO Control Get InterFace FLAGS
    if (ioctl (raw_socket, SIOCGIFFLAGS, &ifr) == -1){
        perror ("Error: Could not retrieve the flags from the device.\n");
        exit (1);
    }
    /* Set the old flags plus the IFF_PROMISC flag */
    // 将ifr_flags设置为混杂模式，并且设置raw_socket接口的标志
    ifr.ifr_flags |= IFF_PROMISC;
    if (ioctl (raw_socket, SIOCSIFFLAGS, &ifr) == -1){
        perror ("Error: Could not set flag IFF_PROMISC");
        exit (1);
    }
    printf ("Set up the promiscuous mode\n");

    /* Configure the device */
    // if (ioctl (raw_socket, SIOCGIFINDEX, &ifr) < 0){
    //     perror ("Error: Error getting the device index.\n");
    //     exit (1);
    // }
    return raw_socket;
    // TODO();
}

// clear the flag:
void clear_flag(char *device, int sock_fd){
    // Exercise 1: turn off promiscuous mode
    // Add your code here:
    /* Get the current flags that the device might have */
    // Get device name
    struct ifreq ifr;
    memset (&ifr, 0, sizeof (struct ifreq));
    strcpy (ifr.ifr_name, device);
    // Get sock_fd's Interface Flags
    if (ioctl (sock_fd, SIOCGIFFLAGS, &ifr) == -1){
        perror ("Error: Could not retrieve the flags from the device.\n");
        exit (1);
    }
    /* Clear IFF_PROMISC flag */
    // Cancel Promiscuous mode and Set sock_fd's Interface Flags
    ifr.ifr_flags &= ~IFF_PROMISC;
    if (ioctl (sock_fd, SIOCSIFFLAGS, &ifr) == -1){
        perror ("Error: Could not set flag IFF_PROMISC");
        exit (1);
    }
    printf ("Cleared promiscuous mode\n");
    // TODO();
}

int main (int argc, char **argv){
    int max_rounds = MAX_ROUND;
    if(argc < 2){
        printf("Usage: ./main.out <device>\n");
        exit(1);
    }
    char *device = argv[1];

    // create a new raw socket
    int sock_fd = raw_init(device);
    printf("Start sniffing\n");

    int loops = 0;
    while(1){
        if (loops > max_rounds)
            break;
        
        printf("server %d: sniffing %d\n", getpid(), loops);

        // the specification requires to initialize the sock_addr_len:
        struct sockaddr sock_addr;
        memset(&sock_addr, 0, sizeof(sock_addr));
        unsigned int sock_addr_len = sizeof(sock_addr);
        ssize_t n = recvfrom(sock_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&sock_addr, &sock_addr_len);
        if (n == -1)
            printf("Error: socket recv\n");

        // analyze the received packet:
        packet_analyze(buf, n);
        loops ++;
    }
    // a summary:
    printf("arps = %d, ips = %d, ipv6 = %d\n",
           eth_count.arps, eth_count.ips, eth_count.ipv6s);
    printf("tcps = %d, udps = %d\n", ip_count.tcps, ip_count.udps);

    clear_flag(device, sock_fd);
    close(sock_fd);
    return 0;
}





