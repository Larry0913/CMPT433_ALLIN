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
#include "neoPixel.h"
#include "digitDisplay.h"
#include "Leds.h"
#include "tempSensor.h"

int running_flag = 1;
volatile void *pPruBase;
volatile sharedMemStruct_t *pSharedPru0;

void startProgram()
{

    // Get access to shared memory for my uses
    pPruBase = getPruMmapAddr();
    pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    UDP_init();
    trafficIn_init();
    trafficOut_init();
    joystick_init();
    AudioMixer_init();
    motionSensor_init();
    I2C_init();
    matrix_init();
    timeDisplay_init();
    neoPixel_init(pSharedPru0);
    temp_init();
    leds_init();


    UDP_wait();
    trafficIn_wait();
    trafficOut_wait();
    joystick_wait();
    matrix_wait();
    I2C_wait();
    timeDisplay_wait();
    temp_wait();
    freePruMmapAddr(pPruBase);

}

void stopProgram()
{
    running_flag = 0;

    printf("Cleaning up...54321\nDone!\n");
    printf("Thank you!\nBy: CMPT433_ALLIN!\n");

    UDP_cleanup();
    matrix_cleanup();
    AudioMixer_cleanup();
    I2C_cleanup();
    timeDisplay_cleanup();
    clearStrip();
    leds_cleanup();

    printf("   ________  _______  ________ __ __________      ___    __    __    _____   __\n");     
    printf("  / ____/  |/  / __ \\/_  __/ // /|__  /__  /     /   |  / /   / /   /  _/ | / /\n");       
    printf(" / /   / /|_/ / /_/ / / / / // /_ /_ < /_ <     / /| | / /   / /    / //  |/ / \n"); 
    printf("/ /___/ /  / / ____/ / / /__  __/__/ /__/ /    / ___ |/ /___/ /____/ // /|  /  \n"); 
    printf("\\____/_/  /_/_/     /_/    /_/ /____/____/____/_/  |_/_____/_____/___/_/ |_/   \n"); 
    printf("                                        /_____/                                \n"); 


}