#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "tempSensor.h"
#include "utils.h"

#define TEMP_SENSOR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define STATUS_TEMP_VOLTAGE 2028.00 // 25.8 ℃
#define STATUS_TEMP_DEGREE 25.80
#define TEMP_PER_VOLTAGE 0.08

// void* TempSensor_thread(void *arg)
// {
//     (void) arg;
//     // Open file
//     while (running_flag == 1)
//     {
//         FILE *f = fopen(TEMP_SENSOR_PATH, "r");
//         if (!f)
//         {
//             printf("ERROR: Unable to open in_voltage1_raw file\n");
//             //exit(-1);
//             return NULL;
//         }
//         // Get reading
//         double temp_voltage_reading = 0;
//         double temp_result = 0;
//         int itemsRead = fscanf(f, "%le", &temp_voltage_reading);
//         if (itemsRead <= 0)
//         {
//             printf("ERROR: Unable to read values from in_voltage1_raw file.\n");
//             //exit(-1);
//             return NULL;

//         }
//         // Close file
//         fclose(f);

//         // calculate the temp
//         if (temp_voltage_reading > STATUS_TEMP_VOLTAGE)
//         {
//             double gap = temp_voltage_reading - STATUS_TEMP_VOLTAGE;
//             temp_result = STATUS_TEMP_DEGREE + TEMP_PER_VOLTAGE * gap;
//         }
//         else
//         {
//             double gap = STATUS_TEMP_VOLTAGE - temp_voltage_reading;
//             temp_result = STATUS_TEMP_DEGREE - TEMP_PER_VOLTAGE * gap;
//         }

//         printf("Current Tempature is %.2f \n", temp_result);
//         sleepForMs(1000);
//         //return temp_result;
//     }
//     return NULL;
// }

double TempSenor_returnTemp()
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
       
    sleep(1); 

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
    float temperature = 24;
	if (temp_flag){
		int tempRaw = (tempData[0] & 0x1F) << 8 | tempData[1];
		if (tempRaw > 4095)
			tempRaw -= 8192;
	    temperature = tempRaw * 0.0625;
	}
    sleepForMs(1000);
    return temperature;
}