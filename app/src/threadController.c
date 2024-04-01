#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "threadController.h"
#include "joystick.h"
#include "trafficControl.h"
#include "udp.h"
#include "motionSensor.h"
#include "musicThread.h"
#include "matrix.h"
#include "I2C.h"

int running_flag = 1;

void startProgram()
{
    pthread_t trafficInID, trafficOutID, udpThreadID;

    pthread_create(&trafficInID, NULL, trafficInThread, NULL);
    pthread_create(&trafficOutID, NULL, trafficOutThread, NULL);
    pthread_create(&udpThreadID, NULL, udpThread, NULL);
    joystick_init();
    AudioMixer_init();
    motionSensor_init();
    I2C_init();

    matrix_init();

    pthread_join(trafficInID, NULL);
    pthread_join(trafficOutID, NULL);
    pthread_join(udpThreadID, NULL);
    joystick_wait();
    matrix_wait();
    I2C_wait();

}

void stopProgram()
{
    running_flag = 0;
    matrix_cleanup();
    AudioMixer_cleanup();

}