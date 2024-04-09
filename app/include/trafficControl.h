#ifndef _TRAFFIC_CONTROL_H_
#define _TRAFFIC_CONTROL_H_

#include <stdbool.h>

void trafficIn_init();
void trafficIn_wait();
void trafficIn_cleanup(void);

void trafficOut_init();
void trafficOut_wait();
void trafficOut_cleanup(void);

// record people walk in or walk out
void* trafficInThread(void*);

void* trafficOutThread(void*);

int getCurrentPeopleCount(void);

#endif