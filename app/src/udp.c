#include <stdlib.h>
#include <stdio.h>
#include "threadController.h"
#include "trafficControl.h"
#include "utils.h"

void* udpThread(void* _arg){
    (void)_arg;

    while(running_flag){
        printf("People count in the room is %d\n", getCurrentPeopleCount());
        sleepForMs(200);
    }

    return NULL;
}