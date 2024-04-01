#ifndef _TRAFFIC_CONTROL_H_
#define _TRAFFIC_CONTROL_H_

#include <stdbool.h>

// record people walk in or walk out
void* trafficInThread(void*);

void* trafficOutThread(void*);

int getCurrentPeopleCount(void);

#endif