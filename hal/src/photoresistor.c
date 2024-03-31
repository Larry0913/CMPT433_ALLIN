#include "photoresistor.h"
#include "utils.h"

#define LIGHT_INTENSITY_PATH "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"



int getLightIntensity(){
    int ret = readFromFileToScreen(LIGHT_INTENSITY_PATH);
    return ret;
}