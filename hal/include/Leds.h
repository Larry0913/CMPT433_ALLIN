#ifndef _LEDS_H_
#define _LEDS_H_

#define NUM_LED 4
enum ledIdx {USR0, USR1, USR2, USR3};

void leds_init();
void leds_cleanup();

void ledTriggerNone(int numLeds);

void ledBlinkBase();

void leds_init();

#endif