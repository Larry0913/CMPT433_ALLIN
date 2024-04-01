#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "motionSensor.h"
#include "utils.h"

#define MOTION_SENSOR_PATH "/sys/class/gpio/gpio49/value"


void motionSensor_init(){

    runCommand("config-pin p9.15 gpio");
    runCommand("config-pin p9.23 gpio");

    runCommand("echo in > /sys/class/gpio/gpio48/direction"); // p9.15
    runCommand("echo in > /sys/class/gpio/gpio49/direction"); // p9.23

}

int getMotionSensorStatus(){
    return readFromFileToScreen(MOTION_SENSOR_PATH);
}


void motionSensor_cleanup(){

}