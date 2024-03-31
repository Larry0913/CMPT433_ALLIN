#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "threadController.h"
#include "joystick.h"
#include "trafficControl.h"
#include "udp.h"

int running_flag = 1;

void startProgram()
{
    pthread_t trafficInID, trafficOutID, udpThreadID;

    joystick_init();
    pthread_create(&trafficInID, NULL, trafficInThread, NULL);
    pthread_create(&trafficOutID, NULL, trafficOutThread, NULL);
    pthread_create(&udpThreadID, NULL, udpThread, NULL);


    joystick_wait();
    pthread_join(trafficInID, NULL);
    pthread_join(trafficOutID, NULL);
    pthread_join(udpThreadID, NULL);
}

void stopProgram()
{
    running_flag = 0;
}