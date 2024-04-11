#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_


void temp_init();

//join thread
void temp_wait();

//clean up malloc variable
void temp_cleanup(void);

float getTemp();


#endif