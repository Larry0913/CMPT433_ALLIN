#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "threadController.h"
#include "joystick.h"

int running_flag = 1;

void startProgram()
{
    joystick_init();

    joystick_wait();
}

void stopProgram()
{
    running_flag = 0;
}