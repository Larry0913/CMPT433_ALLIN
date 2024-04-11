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
#include "neoPixel.h"

int peopleCount = 0;

bool in_flag = false;
bool out_flag = true;

static pthread_t trafficIn_id, trafficOut_id;

void trafficIn_init()
{
    pthread_create(&trafficIn_id, NULL, trafficInThread, NULL);
}
    
void trafficIn_wait()
{
    pthread_join(trafficIn_id, NULL);
}
    
void trafficIn_cleanup(void)
{

}

void* trafficInThread(void* _arg){
    (void)_arg;

    while(running_flag){
        if(getMotionSensorStatus() == 1){ // motion sensor for IN
            peopleCount += 1;
            AudioMixer_queueSound(&enterWave);
            flashTeal(); // neopixel turns Teal for 500ms when detect people in
            sleepForMs(5500);
            while(getMotionSensorStatus() == 1){ sleepForMs(50); } // busy wait until motion sensor back to 0
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void trafficOut_init()
{
    pthread_create(&trafficOut_id, NULL, trafficOutThread, NULL);
}

void trafficOut_wait()
{
    pthread_join(trafficOut_id, NULL);
}
void trafficOut_cleanup(void)
{

}

void* trafficOutThread(void* _arg){
    (void)_arg;

    sleepForMs(50); // prevent premature error

    while(running_flag){
        if(getLightIntensity() <= 3800){ 
            if (peopleCount > 0){ // prevent getting nagative count
                peopleCount -= 1;
                AudioMixer_queueSound(&outWave);
                flashBlue(); //neopixel turns blue for 500ms when detect people out
                sleepForMs(6500);
            }
            sleepForMs(1000); // prevent repeated count
        }
    }
    pthread_exit(NULL);
    return NULL;
}


int getCurrentPeopleCount(){
    return peopleCount;
}