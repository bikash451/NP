#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX 1024 // Buffer size
#define SERVER_PORT 8080 // Server port
#define ERR_LOG(log) do { perror(log); exit(1); } while (0)

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX];
    socklen_t addr_len;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        ERR_LOG("socket");
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        ERR_LOG("inet_pton");
    }

    addr_len = sizeof(servaddr);

    printf("UDP Client ready. Type your messages below:\n");

    while (1) {
        // Clear the buffer and get user input
        memset(buffer, 0, MAX);
        printf("Client: ");
        fgets(buffer, MAX, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        // Send the message to the server
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&servaddr, addr_len);

        // Check if the client wants to exit
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client exited the chat.\n");
            break;
        }

        // Clear the buffer and receive the server's response
        memset(buffer, 0, MAX);
        ssize_t recv_len = recvfrom(sockfd, buffer, MAX, 0, (struct sockaddr *)&servaddr, &addr_len);
        if (recv_len < 0) {
            ERR_LOG("recvfrom");
        }
        buffer[recv_len] = '\0'; // Null-terminate the received string

        printf("Server: %s\n", buffer);

        // Check if the server wants to exit
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Server exited the chat.\n");
            break;
        }
    }

    // Close the socket
    close(sockfd);

    return 0;
}

