#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "utils.h"
#include "trafficControl.h"
#include "threadController.h" // getting running_flag
#include "motionSensor.h"
#include "photoresistor.h"

int peopleCount = 0;


void* trafficInThread(void* _arg){
    (void)_arg;

    while(running_flag){
        if(getMotionSensorStatus() == 1){ // motion sensor for IN
            peopleCount += 1;

            while(getMotionSensorStatus() == 1){ sleepForMs(50); } // busy wait until motion sensor back to 0
        }
    }
    return NULL;
}


void* trafficOutThread(void* _arg){
    (void)_arg;

    sleepForMs(50); // prevent premature error
    int previous = getLightIntensity();

    while(running_flag){
        if(previous - getLightIntensity() >= 500){ // laser for OUT
            if (peopleCount >= 1){ // prevent getting nagative count
                peopleCount -= 1;
            }
            previous = getLightIntensity();
            
            sleepForMs(1000); // prevent repeated count
        }
    }
    return NULL;
}


int getCurrentPeopleCount(){
    return peopleCount;
}