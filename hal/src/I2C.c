#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "threadController.h"
#include "utils.h"
#include "I2C.h"
#include "trafficControl.h"
#include "matrix.h"

static pthread_t I2C_id;

//0x00
int bottomDigits[10] = {0xd0, 0xc0, 0x98, 0xd8, 0xc8, 0x58, 0x58, 0x02, 0xd8, 0xd8};
//0x01
int topDigits[10] = {0xa1, 0x00, 0x83, 0x03, 0x22, 0x23, 0xa3, 0x05, 0xa3, 0x23};

static void exportLeftDisplayPin61(void);
static void exportRightDisplayPin44(void);

static int I2CrunCommand(char* command);

void I2C_init()
{
    //printf("14-Seg Diaplay starting!\n");
    int output61 = -1;
	int output44 = -1;

    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    
	while (output61 != 0 && output44 != 0)
	{
		output61 = I2CrunCommand("echo out > /sys/class/gpio/gpio61/direction");
		if (output61 != 0)
        {
            exportLeftDisplayPin61();
        }
			
		output44 = I2CrunCommand("echo out > /sys/class/gpio/gpio44/direction");
		if (output44 != 0)
        {
            exportRightDisplayPin44();
        }		
	}
    
    pthread_create(&I2C_id, NULL, I2CDisplayThread, NULL);
}

void I2C_wait()
{
    pthread_join(I2C_id, NULL);
}

void I2C_cleanup(void)
{
    writeToFile(I2C_LEFT_DIGIT_PATH, OFF_I2C);
    writeToFile(I2C_RIGHT_DIGIT_PATH, OFF_I2C);
}

void *I2CDisplayThread(void *args)
{
    (void)args;
    int showNum = 0;
    int leftDigit;
    int rightDigit;
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    
    writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
    writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);

    while(running_flag)
    {
        ShowMode curMode = getCurrentMode();
        if(curMode == PEOPLE_MODE)
        {
            showNum = getCurrentPeopleCount();
        }
        else if(curMode == TEMP_MODE)
        {
            showNum = 0;
        }

        if(showNum > 99)
        {
            showNum = 99;
        }

        leftDigit = showNum / 10;
        rightDigit = showNum % 10;

        if(leftDigit > 0)
        {
            // Set both pins off
            writeToFile(I2C_LEFT_DIGIT_PATH, OFF_I2C);
            writeToFile(I2C_RIGHT_DIGIT_PATH, OFF_I2C);

            // Display left digit
            writeI2cReg(i2cFileDesc, REG_OUTA, bottomDigits[leftDigit]);
            writeI2cReg(i2cFileDesc, REG_OUTB, topDigits[leftDigit]);

            // Turn on left digit
            writeToFile(I2C_LEFT_DIGIT_PATH, ON);

            sleepForMs(5);
        }

        // Set both pins off
		writeToFile(I2C_LEFT_DIGIT_PATH, OFF_I2C);
        writeToFile(I2C_RIGHT_DIGIT_PATH, OFF_I2C);

		// Display right digit
		writeI2cReg(i2cFileDesc, REG_OUTA, bottomDigits[rightDigit]);
		writeI2cReg(i2cFileDesc, REG_OUTB, topDigits[rightDigit]);

		// Turn on right digit
		writeToFile(I2C_RIGHT_DIGIT_PATH, ON);

		sleepForMs(5);


    }

    writeToFile(I2C_LEFT_DIGIT_PATH, OFF_I2C);
    writeToFile(I2C_RIGHT_DIGIT_PATH, OFF_I2C);

    pthread_exit(NULL);
    return NULL;
}

static void exportLeftDisplayPin61(void)
{
	int exportPin61 = -1;
	while (exportPin61 != 0)
	{
		exportPin61 = I2CrunCommand("echo 61 > /sys/class/gpio/export");
	}
}

static void exportRightDisplayPin44(void)
{
	int exportPin44 = -1;
	while (exportPin44 != 0)
	{
		exportPin44 = I2CrunCommand("echo 44 > /sys/class/gpio/export");
	}
}

static int I2CrunCommand(char* command)
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
    // if (exitCode != 0) {
    //     perror("Unable to execute command:");
    //     printf(" command: %s\n", command);
    //     printf(" exit code: %d\n", exitCode);
    // }
    return exitCode;
}
