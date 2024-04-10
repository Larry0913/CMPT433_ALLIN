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
#include "musicThread.h"
#include "utils.h"
#include "udp.h"
#include "matrix.h"
#include "digitDisplay.h"


int sockfd;
struct sockaddr_in servaddr, cliaddr;
static pthread_t UDP_id;

static float get_uptime();
static int getTime();

static void setupUDPSocket(void);

static int receivePacket(char *packet);
static int sendPacket(char *packet);

static void runUDPCommand(char *command);

static void printHelp(void);
static void printStatus(void);
static void printCurrentTime();
static void printPeopleCount(void);
static void printTemp(void);
static void ppl(void);
static void temp(void);
static void smile(void);
static void volumeIncrease(void);
static void volumeDecrease(void);

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

static float get_uptime()
{
	float upTime;
	FILE *upTimeFile = fopen("/proc/uptime", "r");
	fscanf(upTimeFile, "%f", &upTime);
	fclose(upTimeFile);
	return upTime;
}

static int getTime()
{
    time_t rawtime = time(NULL);
	struct tm * timeinfo;
	int hour = 0;
	int minute = 0;

    time(&rawtime);
	timeinfo = localtime(&rawtime);

	hour = (*timeinfo).tm_hour;
	minute = (*timeinfo).tm_min;

    int ret = hour * 100 + minute;
    return ret;
}

static void setupUDPSocket(void)
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

static int receivePacket(char *packet)
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

static int sendPacket(char *packet)
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

static void runUDPCommand(char *command)
{
    if (strcmp(command, "help\n") == 0 ||strcmp(command, "?\n") == 0)
    {
        printHelp();
    }
    else if (strcmp(command, "status\n") == 0)
    {
        printStatus();
    }
    else if (strcmp(command, "time\n") == 0)
    {
        printCurrentTime();
    }
    else if (strcmp(command, "peoplecount\n") == 0)
    {
        printPeopleCount();
    }
    else if (strcmp(command, "temperature\n") == 0)
    {
        printTemp();
    }
    else if (strcmp(command, "ppl\n") == 0)
    {
        ppl();
    }
    else if (strcmp(command, "temp\n") == 0)
    {
        temp();
    }
    else if (strcmp(command, "smile\n") == 0)
    {
        smile();
    }
    else if (strcmp(command, "volume increase\n") == 0)
    {
        volumeIncrease();
    }
    else if (strcmp(command, "volume decrease\n") == 0)
    {
        volumeDecrease();
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

static void printHelp(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    strcat(buffer, "BBG_ALLIN Accepted command examples:\n");
    strcat(buffer, "status          -- Get current status.\n");
    strcat(buffer, "time            -- Get current time.\n");
    strcat(buffer, "peoplecount     -- Get the number of people in the room.\n");
    strcat(buffer, "temperature     -- Get the temperature of the room.\n");
    strcat(buffer, "ppl             -- Go to peoplecount mode.\n");
    strcat(buffer, "temp            -- Go to temperature mode.\n");
    strcat(buffer, "smile           -- Go to smile mode.\n");
    strcat(buffer, "volume increase -- Increase the volume by 5.\n");
    strcat(buffer, "volume decrease -- Decrease the volume by 5.\n");
    strcat(buffer, "shutdown        -- Cause the server program to end.\n");
    strcat(buffer, "<enter>         -- repeat last command.\n\n");
    sendPacket(buffer);
}

static void printStatus(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    //sprintf(buffer, "BBG_ALLIN status uptime=%f, time=%d, mode=%d, ppl=%d, volume=%d, temp=%d\n", get_uptime(), getTime(), getModebyEnum(getCurrentMode()), getCurrentPeopleCount(), AudioMixer_getVolume(), getTemp());
    sprintf(buffer, "BBG_ALLIN status uptime=%f, time=%d, mode=%d, ppl=%d, volume=%d\n", get_uptime(), getTime(), getModebyEnum(getCurrentMode()), getCurrentPeopleCount(), AudioMixer_getVolume());
    sendPacket(buffer);
}

static void printCurrentTime()
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    int time = getTime();
    int hour = time / 100;
    int minute = time % 100;
    if (hour < 10) {
		if (minute < 10) {
			sprintf(buffer, "BBG_ALLIN current time: 0%d : 0%d\n", hour, minute);
		} else {
			sprintf(buffer, "BBG_ALLIN current time: 0%d : %d\n", hour, minute);
		}
	} else {
		if (minute < 10) {
			sprintf(buffer, "BBG_ALLIN current time: %d0 : %d\n", hour, minute);
		} else {
			sprintf(buffer, "BBG_ALLIN current time: %d : %d\n", hour, minute);
		}
	}
    sendPacket(buffer);
}

static void printPeopleCount(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN people count: %d\n", getCurrentPeopleCount());
    sendPacket(buffer);
}

static void printTemp(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    //sprintf(buffer, "BBG_ALLIN temperature: %d\n", get());
    sprintf(buffer, "BBG_ALLIN temperature: 25\n");
    sendPacket(buffer);
}

static void ppl(void)
{
    setPeopleMode();
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN Mode: People count mode\n");
    sendPacket(buffer);
}

static void temp(void)
{
    setTempMode();
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN Mode: Temperature mode\n");
    sendPacket(buffer);
}

static void smile()
{
    setSmileMode();
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN Mode: Smile mode\n");
    sendPacket(buffer);
}

static void volumeIncrease(void)
{
    AudioMixer_setVolume(AudioMixer_getVolume() + 5);
    printf("Current volume is %d\n", AudioMixer_getVolume());
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN: volume increases by 5, now is %d\n", AudioMixer_getVolume());
    sendPacket(buffer);
}

static void volumeDecrease(void)
{
    AudioMixer_setVolume(AudioMixer_getVolume() - 5);
    printf("Current volume is %d\n", AudioMixer_getVolume());
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN: volume decreases by 5, now is%d\n", AudioMixer_getVolume());
    sendPacket(buffer);
}

void stop(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "BBG_ALLIN Program terminating.\n");
    sendPacket(buffer);
    stopProgram();
    exit(1);
}