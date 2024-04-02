#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "utils.h"
#include "trafficControl.h"
#include "threadController.h" // getting running_flag
#include "motionSensor.h"
#include "photoresistor.h"
#include "musicThread.h"

int peopleCount = 0;

bool in_flag = false;
bool out_flag = true;

void* trafficInThread(void* _arg){
    (void)_arg;

    while(running_flag){
        if(getMotionSensorStatus() == 1){ // motion sensor for IN
            peopleCount += 1;
            AudioMixer_queueSound(&enterWave);
            while(getMotionSensorStatus() == 1){ sleepForMs(50); } // busy wait until motion sensor back to 0
        }
    }
    return NULL;
}


void* trafficOutThread(void* _arg){
    (void)_arg;

    sleepForMs(50); // prevent premature error

    while(running_flag){
        if(getLightIntensity() <= 3800){ 
            if (peopleCount > 0){ // prevent getting nagative count
                peopleCount -= 1;
                AudioMixer_queueSound(&outWave);
            }
            sleepForMs(1000); // prevent repeated count
        }
    }
    return NULL;
}


int getCurrentPeopleCount(){
    return peopleCount;
}