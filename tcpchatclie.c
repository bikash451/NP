#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080 // Port number
#define BUFFER_SIZE 100
#define ERR_LOG(log) do { perror(log); exit(1); } while (0)

int main() {
    int sockfd;
    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ERR_LOG("socket");
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT); // Server port

    // Convert and set server IP address (localhost for testing)
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        ERR_LOG("inet_pton");
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        ERR_LOG("connect");
    }

    printf("Connected to the server.\n");

    while (1) {
        // Clear buffer, input message from the client, and send to the server
        memset(sendline, 0, BUFFER_SIZE);
        printf("Client: ");
        fgets(sendline, BUFFER_SIZE, stdin);
        sendline[strcspn(sendline, "\n")] = '\0'; // Remove newline character from input

        send(sockfd, sendline, strlen(sendline), 0);

        // Check if the client wants to exit
        if (strncmp(sendline, "exit", 4) == 0) {
            printf("Client exited the chat.\n");
            break;
        }

        // Clear buffer and receive the server's response
        memset(recvline, 0, BUFFER_SIZE);
        ssize_t recv_len = recv(sockfd, recvline, BUFFER_SIZE, 0);
        if (recv_len < 0) {
            ERR_LOG("recv");
        }
        recvline[recv_len] = '\0'; // Null-terminate the received string

        printf("Server: %s\n", recvline);

        // Check if the server wants to exit
        if (strncmp(recvline, "exit", 4) == 0) {
            printf("Server exited the chat.\n");
            break;
        }
    }

    // Close the socket
    close(sockfd);

    return 0;
}

