#ifndef _MATRIX_H_
#define _MATRIX_H_


#define I2C_MATRIX_ADDRESS 0x70 

#define REG_STARTUP 0x21
#define REG_NOFLASH 0x81

#define REG_ROW8 0x0E
#define REG_ROW7 0x0C
#define REG_ROW6 0x0A
#define REG_ROW5 0x08
#define REG_ROW4 0x06
#define REG_ROW3 0x04
#define REG_ROW2 0x02
#define REG_ROW1 0x00

typedef enum {
    PEOPLE_MODE,
    TEMP_MODE,
} ShowMode;

//configure and init
void matrix_init();

//join thread
void matrix_wait();

//clean up malloc variable
void matrix_cleanup(void);

//thread function
void *matrixThread(void *args);

ShowMode getCurrentMode();

// set matrix to people count mode
void setPeopleMode();

// set matrix to temperature mode
void setTempMode();

#endif