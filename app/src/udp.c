#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "threadController.h"
#include "trafficControl.h"
#include "utils.h"
#include "udp.h"


int sockfd;
struct sockaddr_in servaddr, cliaddr;
static pthread_t UDP_id;

void UDP_init()
{
    setupUDPSocket();
    //printf("set up socket successsfully!\n");
    pthread_create(&UDP_id, NULL, UDPServerThread, NULL);
}

void UDP_wait()
{
    pthread_join(UDP_id, NULL);
}

void UDP_cleanup(void)
{
    close(sockfd);
}

void *UDPServerThread(void *args)
{
    (void)args;
    char buffer[BUFFER_MAX_SIZE];
    char lastCommand[BUFFER_MAX_SIZE] = {0};

    while (running_flag)
    {
        memset(buffer, 0, BUFFER_MAX_SIZE);  // Clear the buffer
        int size = receivePacket(buffer);

        if(size > 0)
        {   
            buffer[size] = '\0';

            size_t length = strlen(lastCommand);
            //if only receive enter
            if (strcmp(buffer, "\n") == 0)
            {
                strncpy(buffer, lastCommand, length);
                buffer[length] = '\0';
            }
            else //updade last command
            {
                size_t length = strlen(buffer);
                strncpy(lastCommand, buffer, length);
                lastCommand[length] = '\0';
            }
            
            runUDPCommand(buffer);
        }
        
    }
    pthread_exit(NULL);
    return NULL;
    
}

void setupUDPSocket(void)
{
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    //create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Failed to create socket!\n");
        exit(1);
    }

    //bind
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Binding failed!\n");
        exit(1);
    }
}

int receivePacket(char *packet)
{
    unsigned int sin_len = sizeof(cliaddr);
    int bytesRx = recvfrom(sockfd, packet, BUFFER_MAX_SIZE - 1, 0, (struct sockaddr *) &cliaddr, &sin_len);
    
    if (bytesRx < 0)
    {
        perror("Failed to receive packet.\n");
        exit(1);
    }
    return bytesRx;
}

int sendPacket(char *packet)
{
    unsigned int sin_len = sizeof(cliaddr);
    int byteTx = sendto(sockfd, (const char *)packet, BUFFER_MAX_SIZE - 1, 0, (const struct sockaddr *) &cliaddr, sin_len); 

    if (byteTx < 0)
    {
        perror("Failed to send packet.\n");
        exit(1);
    }
    return byteTx;
}

void runUDPCommand(char *command)
{
    if (strcmp(command, "help\n") == 0 ||strcmp(command, "?\n") == 0)
    {
        printHelp();
    }
    else if (strcmp(command, "peoplecount\n") == 0)
    {
        printPeopleCount();
    }
    else if (strcmp(command, "temperature\n") == 0)
    {
        printTemp();
    }
    else if (strcmp(command, "shutdown\n") == 0)
    {
        stop();
    }
    else
    {
        char buffer[BUFFER_MAX_SIZE] = {0};
        sprintf(buffer, "Unknown command. Type 'help' for command list.\n");
        sendPacket(buffer);
        printHelp();
    }
}

void printHelp(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    strcat(buffer, "BBG_ALLIN Accepted command examples:\n");
    strcat(buffer, "peoplecount     -- get the number of people in the room.\n");
    strcat(buffer, "stop            -- cause the server program to end.\n");
    strcat(buffer, "<enter>         -- repeat last command.\n\n");
    sendPacket(buffer);
}

void printPeopleCount(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN count: %d\n", getCurrentPeopleCount());
    sendPacket(buffer);
}

void printTemp(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    //sprintf(buffer, "BBG_ALLIN temperature: %d\n", get());
    sprintf(buffer, "BBG_ALLIN temperature: 25\n");
    sendPacket(buffer);
}

void stop(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN Program terminating.\n");
    sendPacket(buffer);
    stopProgram();
}