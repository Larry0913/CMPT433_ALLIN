#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "matrix.h"
#include "utils.h"
#include "threadController.h"
#include "I2C.h"

static pthread_t matrix_id;
static int i2cFileDesc;
ShowMode currentMode = PEOPLE_MODE;

static void showPeopleMode();
static void showTempMode();

//configure and init
void matrix_init()
{
    runCommand(" config-pin P9_18 i2c");
    runCommand(" config-pin P9_17 i2c");
    
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_MATRIX_ADDRESS);
    
    writeI2cReg(i2cFileDesc, REG_STARTUP, 0x00);
    writeI2cReg(i2cFileDesc, REG_NOFLASH, 0x00);

    pthread_create(&matrix_id, NULL, matrixThread, NULL);
}

//join thread
void matrix_wait()
{
    pthread_join(matrix_id, NULL);
}

//clean up malloc variable
void matrix_cleanup(void)
{
    writeI2cReg(i2cFileDesc, REG_ROW1, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW2, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW3, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW4, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW5, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW6, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW7, OFF_I2C);
    writeI2cReg(i2cFileDesc, REG_ROW8, OFF_I2C);
}

//thread function
void *matrixThread(void *args)
{
    (void)args;
    while(running_flag){
        switch(currentMode)
        {
            case PEOPLE_MODE:
                showPeopleMode();
                break;
            case TEMP_MODE:
                showTempMode();
                break;
            default:
                break;
        }


    }
    pthread_exit(NULL);
    return NULL;
}

ShowMode getCurrentMode()
{
    return currentMode;
}

static void showPeopleMode()
{
    writeI2cReg(i2cFileDesc, REG_ROW1, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW2, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW3, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW4, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW5, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW6, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW7, 0x55);
    writeI2cReg(i2cFileDesc, REG_ROW8, 0x55);
} 

static void showTempMode()
{
    writeI2cReg(i2cFileDesc, REG_ROW1, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW2, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW3, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW4, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW5, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW6, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW7, 0x44);
    writeI2cReg(i2cFileDesc, REG_ROW8, 0x44);
} 

void setPeopleMode() {
    printf("Current mode is people count\n");
    currentMode = PEOPLE_MODE;
}

void setTempMode() {
    printf("Current mode is temperature!\n");
    currentMode = TEMP_MODE;
}

