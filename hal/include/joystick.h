//Press in (centre) to cycle through the beats (modes). 
//Pressing up increases the volume by 5 points; down decreases by 5 points.
//Pressing right increases the tempo by 5 BPM, left decreases by 5 BPM.

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

enum joystick {UP = 26, DOWN = 46, RIGHT = 47, LEFT = 65, IN = 27};

//configure and init
void joystick_init();

//join thread
void joystick_wait();

//clean up malloc variable
void joystick_cleanup(void);

//thread function
void *joystickThread(void *args);

#endif