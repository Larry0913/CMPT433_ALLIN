#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h> 
#include <unistd.h>

void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

int readFromFileToScreen(char *fileName) { // From Brian
    FILE *pFile = fopen(fileName, "r"); 
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        exit(-1); 
    }
    // Read string (line)
    const int MAX_LENGTH = 1024; 
    char buff[MAX_LENGTH]; 
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    int ret = atoi(buff);
    return ret;
}

void sleepForMs(long long delayInMs) { // from Brian
    const long long NS_PER_MS = 1000 * 1000; 
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND; 
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
        nanosleep(&reqDelay, (struct timespec *) NULL);
}


void writeToFile(char* fileName, int value) {
	FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("ERROR OPENING %s.\n", fileName);
        exit(-1);
    }
	int charWritten = fprintf(file, "%d", value);
	if (charWritten <= 0) printf("ERROR WRITING DATA");
	fclose(file);
}

int initI2cBus(char* bus, int address) // from Brian
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl (i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value) // from Brian
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2); 
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1); 
    }   
}