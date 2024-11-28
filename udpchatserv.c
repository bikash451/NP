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
    struct sockaddr_in servaddr, cliaddr;
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all network interfaces
    servaddr.sin_port = htons(SERVER_PORT); // Set server port

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        ERR_LOG("bind");
    }

    printf("UDP Server is running on port %d. Waiting for messages...\n", SERVER_PORT);

    addr_len = sizeof(cliaddr);

    while (1) {
        // Clear buffer and receive message from the client
        memset(buffer, 0, MAX);
        ssize_t recv_len = recvfrom(sockfd, buffer, MAX, 0, (struct sockaddr *)&cliaddr, &addr_len);
        if (recv_len < 0) {
            ERR_LOG("recvfrom");
        }
        buffer[recv_len] = '\0'; // Null-terminate the received string

        printf("Client: %s\n", buffer);

        // Check if the client wants to exit
        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client exited the chat.\n");
            break;
        }

        // Clear buffer, input message from the server, and send it back
        memset(buffer, 0, MAX);
        printf("Server: ");
        fgets(buffer, MAX, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&cliaddr, addr_len);

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

