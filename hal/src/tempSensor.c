#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>

#include "tempSensor.h"
#include "threadController.h"
#include "utils.h"

static pthread_t temp_id;
float temp = 30;

static void *tempThread(void *args);

//init
void temp_init()
{
    pthread_create(&temp_id, NULL, tempThread, NULL);
}

//join thread
void temp_wait()
{
    pthread_join(temp_id, NULL);
}

//clean up malloc variable
void temp_cleanup(void)
{

}

static void *tempThread(void *args)
{
    (void)args; 
    while (running_flag)
    {
        // set i2c bus and open it 
        int i2cFileDescriptor;
        char *i2cBus = "/dev/i2c-2";
        i2cFileDescriptor = open(i2cBus, O_RDWR);
        if (i2cFileDescriptor < 0) {
            printf("Unable to access the bus.\n");
            //exit(1); 
        }

        // Get the address of temperature sensor, and it already set to 0x19
        if (ioctl(i2cFileDescriptor, I2C_SLAVE, 0x19) < 0) {
            printf("Cannot find the device on the bus.\n");
            //exit(1); 
        }

        // Select configuration register
        char sensorConfig[3] = {0x01, 0x00, 0x00}; 
        write(i2cFileDescriptor, sensorConfig, 3);

        // Select resolution rgister
        char resolutionConfig[2] = {0x08, 0x03}; 
        write(i2cFileDescriptor, resolutionConfig, 2);
        
        sleepForMs(5);

        // temp register address
        char tempRegAddr = 0x05; 
        write(i2cFileDescriptor, &tempRegAddr, 1);
        
        char tempData[2] = {0};
        int temp_flag = 1;
        if (read(i2cFileDescriptor, tempData, 2) != 2) {
            printf("Temperature read failed.\n");
            temp_flag = 0;
            //exit(1); 
        }

        // Calculate the temperature
        if (temp_flag){
            int tempRaw = (tempData[0] & 0x1F) << 8 | tempData[1];
            if (tempRaw > 4095)
                tempRaw -= 8192;

            if (tempRaw > 30)
            {
                //printf("tempRaw is %d\n", tempRaw);
                temp = tempRaw * 0.0625;
            }    
            
        }
        //printf("temperature is %f\n", temp);
    }
    pthread_exit(NULL);
    return NULL;
}

float getTemp()
{
    return temp;
}