#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ERR_LOG(log) do { perror(log); exit(1); } while (0)
#define N 128

int main(int argc, const char *argv[]) {
    int sockfd;
    char buff[N] = {0};
    struct sockaddr_in broadcast_addr;
    int optval = 1; // Option for enabling broadcast

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        ERR_LOG("socket");
    }

    // Set socket options to enable broadcast
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0) {
        ERR_LOG("setsockopt");
    }

    // Configure broadcast address
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Broadcast address
    broadcast_addr.sin_port = htons(8888); // Broadcast port

    printf("Broadcast sender ready. Type messages to send:\n");

    while (1) {
        printf("Input: ");
        if (fgets(buff, N, stdin) == NULL) {
            break; // Exit loop if input fails
        }

        // Remove newline character from input
        buff[strcspn(buff, "\n")] = '\0';

        // Send broadcast message
        if (sendto(sockfd, buff, strlen(buff), 0, 
                   (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
            ERR_LOG("sendto");
        }

        printf("Message sent: %s\n", buff);
    }

    close(sockfd);
    return 0;
}

