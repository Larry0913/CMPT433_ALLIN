#ifndef _MOTION_SENSOR_H_
#define _MOTION_SENSOR_H_

//configure and init
void motionSensor_init(void);

int getMotionSensorStatus(void);

//clean up malloc variable
void motionSensor_cleanup(void);


#endif