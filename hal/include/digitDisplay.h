#ifndef _DIGIT_DISPLAY_H_
#define _DIGIT_DISPLAY_H_

/******************************************************
 * Grove Display Definitions
 ******************************************************/
#define CMD_AUTO_ADDR 0x40
#define START_ADDR 0xc0
#define NUM_DIGITS 4
#define DISPLAY_ON 0x88

#define COLON_ON 1
#define COLON_OFF 0
#define COLON_FLAG 0x80
#define ASCII_0 48
#define ASCII_9 57

#define HIGH 1
#define LOW 0
#define IN "in"
#define OUT "out"
#define MAX_BUFFER_LENGTH 1024

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"

#define CLK 2
#define DIO 3
#define CLK_VAL "/sys/class/gpio/gpio2/value"
#define DIO_VAL "/sys/class/gpio/gpio3/value"
#define CLK_DIR "/sys/class/gpio/gpio2/direction"
#define DIO_DIR "/sys/class/gpio/gpio3/direction"

void timeDisplay_init(void);
void timeDisplay_wait(void);
void timeDisplay_cleanup(void);

#endif