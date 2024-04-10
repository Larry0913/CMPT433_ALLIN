#include "pot.h"
#include "utils.h"

int getPotValue(){
    int ret = readFromFileToScreen(A2D_FILE_VOLTAGE0);
    return ret;
}

int getDelayTimeForNeoPixel()
{
    int ret = readFromFileToScreen(A2D_FILE_VOLTAGE0);
    int delayTime = 30 + ret / 80;
    return delayTime;
}