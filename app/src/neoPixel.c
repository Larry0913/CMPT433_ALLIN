#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>

#include "sharedDataStruct.h"
#include "threadController.h"
#include "neoPixel.h"
#include "utils.h"
#include "trafficControl.h"
#include "pot.h"

static volatile sharedMemStruct_t* pSharedPru0 = NULL;
static pthread_t neoPixel_id;
int color_flag = 0; //0: green/yellow/red, 1: Teal, 2: blue

void neoPixel_init(volatile sharedMemStruct_t* pSharedPru0_temp)
{
    //init pixel pruout
    runCommand("config-pin P8.11 pruout");

    //get pru value
    pSharedPru0 = pSharedPru0_temp;

    pthread_create(&neoPixel_id, NULL,neoPixelThread, NULL);
}

// Return the address of the PRU's base memory
volatile void* getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, 
        MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return pPruBase;
}

void freePruMmapAddr(volatile void* pPruBase){
    if (pPruBase != NULL) {
        if (munmap((void*) pPruBase, PRU_LEN)) {
            perror("PRU munmap failed");
            exit(EXIT_FAILURE);
        }
        pPruBase = NULL; 
    }
}

void *neoPixelThread(void *args)
{
    (void)args;
    while (running_flag)
    {
        if(color_flag == 0)
        {
            setPixelColor(color_flag); //green/yellow/red
        }
        else if(color_flag == 1)
        {
            setPixelColor(color_flag); //Teal
            sleepForMs(500);
            color_flag = 0;
        }
        else if(color_flag == 2)
        {
            setPixelColor(color_flag); //blue
            sleepForMs(500);
            color_flag = 0;
        }
        else
        {
            setPixelColor(3); //off
        }
    }
    clearStrip();
    pthread_exit(NULL);
    return NULL;
}

void setPixelColor(int color)
{
    if (color == 0)
    {
        // Forward direction
        //int peopleCount = 0;
        int peopleCount = getCurrentPeopleCount();
        uint32_t showColor;
        uint32_t showColor_temp;
        int delayTime = getDelayTimeForNeoPixel();

        if(peopleCount < 4)
        {
            showColor = Green;
            showColor_temp = GreenBright;
        }
        else if (peopleCount >=4 && peopleCount < 8)
        {
            showColor = Yellow;
            showColor_temp = Yellow;
        }
        else 
        {
            showColor = Red;
            showColor_temp = RedBright;
        }

        for(int i = 0; i < STR_LEN - 1; i++) // Stop one before the last to light up two pixels
        {
            pSharedPru0->neoPixels[i] = showColor; // Set current pixel to blue
            if(i + 1 < STR_LEN)
            {
                pSharedPru0->neoPixels[i + 1] = showColor_temp; // Also set the next pixel to blue
            }
            if(i > 0)
            {
                pSharedPru0->neoPixels[i - 1] = 0; // Turn off the pixel two behind
            }
            sleepForMs(delayTime);
        }

        // Backward direction
        for(int i = STR_LEN - 2; i >= 0; i--) // Start from second-to-last pixel
        {
            pSharedPru0->neoPixels[i] = showColor; // Set current pixel to blue
            if(i + 1 < STR_LEN)
            {
                pSharedPru0->neoPixels[i + 1] = showColor_temp; // Also set the next pixel to blue
            }
            
            if(i + 2 < STR_LEN)
                pSharedPru0->neoPixels[i + 2] = 0; // Turn off the pixel just after the pair
            
            // Edge case for when i is 0 to turn off the LED at position 1
            if(i == 0)
            {
                pSharedPru0->neoPixels[1] = 0;
            }
            sleepForMs(delayTime);
        }

    }
    else if(color == 1)
    {
        for(int i = 0; i < STR_LEN; i++)
        {
            pSharedPru0->neoPixels[i] = Teal;
        }
    }
    else if(color == 2)
    {
        for(int i = 0; i < STR_LEN; i++)
        {
            if(i % 2 == 0)
            {
                pSharedPru0->neoPixels[i] = BlueBright;
            }
            else 
            {
                pSharedPru0->neoPixels[i] = Blue;
            }
        }
    }
    else
    {
        for(int i = 0; i < STR_LEN; i++)
        {
            pSharedPru0->neoPixels[i] = OFF_LED;
        }
    }
    
}

void clearStrip()
{
    for (int i = 0; i < STR_LEN; i++)
    {
        pSharedPru0->neoPixels[i] = OFF_LED;
    }
}

void flashTeal()
{
    color_flag = 1;
}

void flashBlue()
{
    color_flag = 2;
}
