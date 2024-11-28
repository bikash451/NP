#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ERR_LOG(log) do { perror(log); exit(1); } while (0)
#define N 128

int main() {
    int sockfd;
    char buff[N];
    struct sockaddr_in broadcast_addr, src_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        ERR_LOG("socket");
    }

    // Configure the broadcast address
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen on all network interfaces
    broadcast_addr.sin_port = htons(8888); // Same port as the sender

    // Bind the socket to the broadcast address
    if (bind(sockfd, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
        ERR_LOG("bind");
    }

    printf("Broadcast receiver is ready and listening on port 8888...\n");

    while (1) {
        // Receive broadcast message
        ssize_t received = recvfrom(sockfd, buff, N - 1, 0, (struct sockaddr *)&src_addr, &addrlen);
        if (received < 0) {
            ERR_LOG("recvfrom");
        }

        // Null-terminate the received message
        buff[received] = '\0';

        // Print the received message and sender details
        printf("Received: '%s' from %s:%d\n",
               buff,
               inet_ntoa(src_addr.sin_addr), // Convert IP address to string
               ntohs(src_addr.sin_port));   // Convert port number to host byte order
    }

    close(sockfd);
    return 0;
}

