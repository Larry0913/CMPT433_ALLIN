#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "threadController.h"
#include "trafficControl.h"
#include "utils.h"

#define SERVER_PORT 12345  // The port the server will listen on

void* udpThread(void* _arg){
    (void)_arg;

    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Any incoming interface
    serverAddr.sin_port = htons(SERVER_PORT); // Server port

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d\n", SERVER_PORT);

    while(true){
        char buffer[1024];
        unsigned int len = sizeof(clientAddr); // Length of client's address

        // Receive message from client
        int n = recvfrom(sockfd, (char *)buffer, 1024, 0, ( struct sockaddr *) &clientAddr, &len);
        buffer[n] = '\0';

        printf("People count in the room is %d\n", getCurrentPeopleCount());
        sleepForMs(200);

        if ((strcmp(buffer, "peoplecount\n") == 0) || (strcmp(buffer, "peoplecount") == 0)) {
            // Buffer to hold the acknowledgement message, adjust size as needed
            char ack[50]; 
            snprintf(ack, sizeof(ack), "BBG count %d", getCurrentPeopleCount()); 
            sendto(sockfd, ack, strlen(ack), 0, (const struct sockaddr *) &clientAddr, len);
        }

        if ((strcmp(buffer, "shutdown\n") == 0) || (strcmp(buffer, "shutdown") == 0)) {
            const char *ack = "BBG shutdown";
            sendto(sockfd, ack, strlen(ack), 0, (const struct sockaddr *) &clientAddr, len);
            printf("Shutting down server.\n");
            stopProgram();
        }

        if(!running_flag){
            // Close the socket
            close(sockfd);
            break;
        }
    }

    return NULL;
}