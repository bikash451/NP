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
    int sockfd, commfd;
    char str[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ERR_LOG("socket");
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to any IP address
    servaddr.sin_port = htons(PORT); // Server port

    // Bind the socket to the server address and port
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        ERR_LOG("bind");
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        ERR_LOG("listen");
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept a client connection
    commfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
    if (commfd < 0) {
        ERR_LOG("accept");
    }

    printf("Client connected!\n");

    while (1) {
        // Clear buffer and receive message from the client
        memset(str, 0, BUFFER_SIZE);
        ssize_t recv_len = recv(commfd, str, BUFFER_SIZE, 0);
        if (recv_len < 0) {
            ERR_LOG("recv");
        }
        str[recv_len] = '\0'; // Null-terminate the received string

        printf("Client: %s\n", str);

        // Check if the client wants to exit
        if (strncmp(str, "exit", 4) == 0) {
            printf("Client exited the chat.\n");
            break;
        }

        // Clear buffer, input message from the server, and send to client
        memset(str, 0, BUFFER_SIZE);
        printf("Server: ");
        fgets(str, BUFFER_SIZE, stdin);
        str[strcspn(str, "\n")] = '\0'; // Remove newline character from input

        send(commfd, str, strlen(str), 0);

        // Check if the server wants to exit
        if (strncmp(str, "exit", 4) == 0) {
            printf("Server exited the chat.\n");
            break;
        }
    }

    // Close the sockets
    close(commfd);
    close(sockfd);

    return 0;
}

