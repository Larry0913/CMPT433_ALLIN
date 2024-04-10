#include <stdlib.h>
#include <stdio.h>
#include "Leds.h"
#include "utils.h"

//static void ledOn(enum ledIdx usrIdx);
static void ledOff(enum ledIdx usrIdx);

void leds_init()
{
    ledTriggerNone(NUM_LED);
    for (int i = 0; i < NUM_LED; i++)
    {
        ledOff(i);
    }
}

void leds_cleanup()
{
    for (int i = 0; i < NUM_LED; i++)
    {
        ledOff(i);
    }
}

void ledTriggerNone(int numLeds)
{
    for(int i = 0 ; i < numLeds; i++){
        char filePath[128];
        snprintf(filePath, 128, "/sys/class/leds/beaglebone:green:usr%d/trigger", i);
        FILE *pLedTriggerFile = fopen(filePath, "w");
        if (pLedTriggerFile == NULL) {
            printf("ERROR OPENING %s.\n", filePath);
            exit(1);
        }
        int charWritten = fprintf(pLedTriggerFile, "none");
        if (charWritten <= 0) {
            printf("ERROR WRITING DATA");
            exit(1);
        }
        fclose(pLedTriggerFile);
    }
}

void ledBlinkBase()
{
    int gapTime = 50;
    int times = 2; 
    char command[128];

    for(int j = 0; j < times; j ++){

        for (int j = 0; j < NUM_LED; j++)//Set bink variable: light up/off for gapTime ms,
        {
            snprintf(command, 128, "/sys/class/leds/beaglebone:green:usr%d/brightness", j);
            FILE *pLedOnFile = fopen(command, "w");
            if (pLedOnFile == NULL) {
                printf("ERROR OPENING %s.\n", command);
                exit(1);
            }
            int charWritten = fprintf(pLedOnFile, "%d", 1);
            if (charWritten <= 0) {
                printf("ERROR WRITING DATA");
                exit(1);
            }
            fclose(pLedOnFile);  
        }

        sleepForMs(gapTime);//Total amount of time for blink LEDs:

        for (int j = 0; j < NUM_LED; j++)//Set bink variable: light up/off for gapTime ms,
        {
            snprintf(command, 128, "/sys/class/leds/beaglebone:green:usr%d/brightness", j);
            FILE *pLedOffFile = fopen(command, "w");
            if (pLedOffFile == NULL) {
                printf("ERROR OPENING %s.\n", command);
                exit(1);
            }
            int charWritten = fprintf(pLedOffFile, "%d", 0);
            if (charWritten <= 0) {
                printf("ERROR WRITING DATA");
                exit(1);
            }
            fclose(pLedOffFile);  
        }
        sleepForMs(gapTime);
    }
}

// static void ledOn(enum ledIdx usrIdx){
//     char filePath[128];
//     snprintf(filePath,128,"/sys/class/leds/beaglebone:green:usr%d/brightness", usrIdx);
//     //read file
//     FILE *pFile = fopen(filePath, "w");
//     if (pFile == NULL) {
//         printf("ERROR OPENING %s.\n", filePath);
//         exit(-1);
//     }
//     // Write to data to the file using fprintf():
//     fprintf(pFile, "%d", 1);
//     // Close the file using fclose():
//     fclose(pFile);

// }

static void ledOff(enum ledIdx usrIdx){
    char filePath[128];
    snprintf(filePath,128,"/sys/class/leds/beaglebone:green:usr%d/brightness", usrIdx);
    //read file
    FILE *pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        printf("ERROR OPENING %s.\n", filePath);
        exit(-1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "%d", 0);
    // Close the file using fclose():
    fclose(pFile);

}