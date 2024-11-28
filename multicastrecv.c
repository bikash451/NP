#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXBUF 1024 // Maximum buffer size
#define PORT 5555   // Multicast port

struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
char databuf[MAXBUF];

int main(int argc, char *argv[]) {
    /* Create a datagram socket on which to receive */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        perror("opening datagram socket");
        exit(1);
    }

    /* Enable SO_REUSEADDR to allow multiple instances of this application to receive copies of the multicast datagrams */
    {
        int reuse = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
            perror("setting SO_REUSEADDR");
            close(sd);
            exit(1);
        }
    }

    /* Bind to the proper port number with the IP address specified as INADDR_ANY */
    memset((char *)&localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(PORT);
    localSock.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&localSock, sizeof(localSock)) < 0) {
        perror("binding datagram socket");
        close(sd);
        exit(1);
    }

    /* Join the multicast group 225.1.1.1 on the local 9.5.1.1 interface */
    group.imr_multiaddr.s_addr = inet_addr("225.1.1.1"); // Multicast group
    group.imr_interface.s_addr = inet_addr("9.5.1.1");   // Local interface

    if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0) {
        perror("adding multicast group");
        close(sd);
        exit(1);
    }

    /* Read from the socket */
    while (1) {
        memset(databuf, 0, MAXBUF);
        ssize_t recv_len = read(sd, databuf, MAXBUF - 1);
        if (recv_len < 0) {
            perror("reading datagram message");
            close(sd);
            exit(1);
        }

        databuf[recv_len] = '\0'; // Null-terminate the received string
        printf("Received: %s\n", databuf);

        /* Check for exit condition */
        if (strncmp(databuf, "exit", 4) == 0) {
            printf("Exiting multicast receiver.\n");
            break;
        }
    }

    /* Leave the multicast group */
    if (setsockopt(sd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&group, sizeof(group)) < 0) {
        perror("dropping multicast group");
    }

    /* Close the socket */
    close(sd);

    return 0;
}

