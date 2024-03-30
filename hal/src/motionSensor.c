#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "motionSensor.h"
#include "utils.h"


void motionSensor_init(){

    runCommand("config-pin p9.15 gpio");
    runCommand("config-pin p9.23 gpio");

    runCommand("echo in > /sys/class/gpio/gpio48/direction"); // p9.15
    runCommand("echo in > /sys/class/gpio/gpio49/direction"); // p9.23
}


void motionSensor_cleanup(){

}