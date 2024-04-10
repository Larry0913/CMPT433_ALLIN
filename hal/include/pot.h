#ifndef _POT_H_
#define _POT_H_

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

int getPotValue(); // get voltage from pot

int getDelayTimeForNeoPixel(); //use pot value computing neo pixel delay time

#endif