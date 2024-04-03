#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "threadController.h"
#include "tempSensor.h"
#include "utils.h"

#define TEMP_SENSOR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define STATUS_TEMP_VOLTAGE 2028.00 // 25.8 ℃
#define STATUS_TEMP_DEGREE 25.80
#define TEMP_PER_VOLTAGE 0.08

void* TempSensor_returnTemp(void *arg)
{   
    (void) arg;
    // Open file
    while (running_flag == 1)
    {
        FILE *f = fopen(TEMP_SENSOR_PATH, "r");
        if (!f)
        {
            printf("ERROR: Unable to open voltage 1 input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            //exit(-1);
            return NULL;
        }
        // Get reading
        double temp_voltage_reading = 0;
        double temp_result = 0;
        int itemsRead = fscanf(f, "%le", &temp_voltage_reading);
        if (itemsRead <= 0)
        {
            printf("ERROR: Unable to read values from voltage 1 input file.\n");
            //exit(-1);
            return NULL;

        }
        // Close file
        fclose(f);

        // calculate the temp
        if (temp_voltage_reading > STATUS_TEMP_VOLTAGE)
        {
            double gap = temp_voltage_reading - STATUS_TEMP_VOLTAGE;
            temp_result = STATUS_TEMP_DEGREE + TEMP_PER_VOLTAGE * gap;
        }
        else
        {
            double gap = STATUS_TEMP_VOLTAGE - temp_voltage_reading;
            temp_result = STATUS_TEMP_DEGREE - TEMP_PER_VOLTAGE * gap;
        }

        printf("Current Tempature is %.2f \n", temp_result);
        sleepForMs(1000);
        //return temp_result;
    }
    return NULL;
}