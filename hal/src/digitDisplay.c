// This C file of Grove 4-Digit Display hardware made by CMPT433_ALLIN Lingjie Li, reference 
//from Grove 4-Digit Display GUide https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2016-student-howtos/Grove4DigitDisplay.pdf
//GPIO guide from Brian Fraser https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/GPIOGuide.pdf
//Github source from id:djtung https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/links/files/2016-student-howtos/Grove4DigitDisplay.pdf

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>

#include "utils.h"
#include "digitDisplay.h"
#include "threadController.h"

char digits[5];
static pthread_t grove_id;
static char displayDigits[10] = {
    0x3f,
    0x06,
    0x5b,
    0x4f,
    0x66,
    0x6d,
    0x7d,
    0x07,
    0x7f,
    0x67,
};

static char OFFDigits[10] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

static void* groveDisplayThread(void* arg); 

static void setClk(int value);
static void setDio(int value);
static char convertChar(char ch, _Bool colon); 

static void timeDisplay_start(void);
static void timeDisplay_stop(void);
static void timeDisplay_write(char data);

static int getValue(int GPIOpin);

static void DisplayPin2(void);
static void DisplayPin3(void);

static void setDirection(int GPIOpin, char *direction);
static void getCurrentTime(char* result) ;

void timeDisplay_init(void)
{ 
    int ex2 = -1;
	int ex3 = -1;

    runCommand("config-pin p9.22 gpio");    //2CLK
    runCommand("config-pin p9.21 gpio");    //3DIO

    runCommand("echo 2 > /sys/class/gpio/export");
    runCommand("echo 3 > /sys/class/gpio/export");

    while (ex2 != 0 && ex3 != 0)
	{
		ex2 = runCommandDisplay("echo out > /sys/class/gpio/gpio2/direction");
		if (ex2 != 0)
        {
            DisplayPin2();
        }
			
		ex3 = runCommandDisplay("echo out > /sys/class/gpio/gpio3/direction");
		if (ex3 != 0)
        {
            DisplayPin3();
        }		
	}

    setClk(1);
    setDio(1);
    
    pthread_create(&grove_id, NULL, *groveDisplayThread, NULL);
}

void timeDisplay_wait(void)
{
    pthread_join(grove_id, NULL);
}

void timeDisplay_cleanup(void)
{
    timeDisplay_start();
    timeDisplay_write(START_ADDR);
    for (int i = 0; i < NUM_DIGITS; i++) {
        timeDisplay_write(convertChar(digits[i], COLON_OFF));
    }
    timeDisplay_stop();

    setClk(0);
    setDio(0);

    runCommand("echo in > /sys/class/gpio/gpio2/direction");
    runCommand("echo in > /sys/class/gpio/gpio3/direction");

    runCommand("echo 2 > /sys/class/gpio/unexport");
    runCommand("echo 3 > /sys/class/gpio/unexport");
}

static void* groveDisplayThread(void* args) 
{
    (void)args;
    while(running_flag){
        getCurrentTime(digits);
        
        assert(strlen(digits) == NUM_DIGITS);

        timeDisplay_start();
        timeDisplay_write(CMD_AUTO_ADDR);
        timeDisplay_stop();

        timeDisplay_start();
        timeDisplay_write(START_ADDR);
        for (int i = 0; i < NUM_DIGITS; i++) {
            timeDisplay_write(convertChar(digits[i], COLON_ON));
        }
        timeDisplay_stop();

        timeDisplay_start();
        
        timeDisplay_write(DISPLAY_ON | 0x07);
        timeDisplay_stop();
        sleepForMs(3000);
    }

    pthread_exit(NULL);
    return NULL;
}

static void timeDisplay_start(void)
{
    /*
    * When CLK is high, and DIO goes from high to low, input begins
    */

    setClk(HIGH);
    setDio(HIGH);
    wait1();

    setDio(LOW);
    wait1();

    setClk(LOW);
    wait1();
}

static void timeDisplay_stop(void)
{
    /*
    * When CLK is high, and DIO goes from low to high, input ends
    */
    setClk(LOW);
    setDio(LOW);
    wait1();

    setClk(HIGH);
    wait1();

    setDio(HIGH);
    wait1();
}

void timeDisplay_write(char data)
{
    /*
    *Send each bit of data
    */
    for(int i = 0; i < 8; i++) {
        //transfer data when clock is low, from low bit to high bit
        setClk(LOW);
        setDio(data & 0x01);
        data >>= 1;
        wait1();

        setClk(HIGH);
        wait1();
    }
    setClk(LOW);
    //runCommand("echo in > /sys/class/gpio/gpio3/direction");
    setDirection(3, IN);
    wait1();

    //Check that we are getting the ACK from the device
    //printf("get value is %d\n", getValue(DIO));
    assert(getValue(DIO) == 0);

    setClk(HIGH);
    wait1();

    setClk(LOW);
    //runCommand("echo out > /sys/class/gpio/gpio3/direction");
    setDirection(3, OUT);
}

static char convertChar(char ch, _Bool colon) 
{
    char val = 0;
    if ((ASCII_0 <= ch) && (ch <= ASCII_9)) {
        if(running_flag)
        {
            val = displayDigits[ch - ASCII_0];
        }
        else
        {
            val = OFFDigits[ch - ASCII_0];
        }
        
    }

    if (colon) {
        return val | COLON_FLAG;
    }

    return val;
}

static void setClk(int value)
{
    FILE *file = fopen(CLK_VAL, "w");
    if (!file) {
        printf("ERROR: Unable to open file (%s) for write\n", CLK_VAL);
        exit(-1);
    }
    if(fprintf(file, "%d", value) < 0) {
        printf("ERROR: Unable to write (%d) into (%s)", value, CLK_VAL);
    }
    fclose(file);
}

static void setDio(int value)
{
    FILE *file = fopen(DIO_VAL, "w");
    if (!file) {
    printf("ERROR: Unable to open file (%s) for write\n", DIO_VAL);
    exit(-1);
    }
    if(fprintf(file, "%d", value) < 0) {
    printf("ERROR: Unable to write (%d) into (%s)", value, DIO_VAL);
    }
    fclose(file);
}

static int getValue(int GPIOpin)
{
    char value[MAX_BUFFER_LENGTH] = {'\0'};

    FILE *file;
    if (GPIOpin == 2) {
        file = fopen(CLK_VAL, "r");
    }
    else {
        file = fopen(DIO_VAL, "r");
    }
    if (!file) {
        printf("ERROR: Unable to open file GPIO(%d) for read\n", GPIOpin);
        exit(-1);
    }
    if(!(fgets(value, MAX_BUFFER_LENGTH, file))) {
        printf("ERROR: Unable to get value from GPIO(%d)", GPIOpin);
    }
    fclose(file);
    return atoi(value);

}

static void DisplayPin2(void)
{
	int exportPin2 = -1;
	while (exportPin2 != 0)
	{
		exportPin2 = runCommandDisplay("echo 2 > /sys/class/gpio/export");
	}
}

static void DisplayPin3(void)
{
	int exportPin3 = -1;
	while (exportPin3 != 0)
	{
		exportPin3 = runCommandDisplay("echo 3 > /sys/class/gpio/export");
	}
}

static void setDirection(int GPIOpin, char *direction)
{
    FILE *file = NULL;
    if (GPIOpin == 2) {
        file = fopen(CLK_DIR, "w");
    }
    else {
        file = fopen(DIO_DIR, "w");
    }
    if (!file) {
        printf("ERROR: Unable to open file GPIO(%d) for write\n", GPIOpin);
        exit(-1);
    }
    if(fprintf(file, "%s", direction) < 0) {
        printf("ERROR: Unable to write (%s) into GPIO(%d)", direction, GPIOpin);
    }
    fclose(file);
}

static void getCurrentTime(char* result) {
	time_t rawtime = time(NULL);
	struct tm * timeinfo;
	int hour = 0;
	int minute = 0;

    time(&rawtime);
	timeinfo = localtime(&rawtime);

	hour = (*timeinfo).tm_hour;
	minute = (*timeinfo).tm_min;

	// format the string
	if (hour < 10) {
		if (minute < 10) {
			sprintf(result, "0%d0%d", hour, minute);
		} else {
			sprintf(result, "0%d%d", hour, minute);
		}
	} else {
		if (minute < 10) {
			sprintf(result, "%d0%d", hour, minute);
		} else {
			sprintf(result, "%d%d", hour, minute);
		}
	}

}


