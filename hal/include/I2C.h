//I2C.h 14-Seg Display
#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_LEFT_DIGIT_PATH "/sys/class/gpio/gpio61/value"
#define I2C_RIGHT_DIGIT_PATH "/sys/class/gpio/gpio44/value"

#define LEFT_DIRECTION "/sys/class/gpio/gpio61/direction"
#define RIGHT_DIRECTION "/sys/class/gpio/gpio44/direction"

#define I2C_DEVICE_ADDRESS 0x20

#define ON 1
#define OFF_I2C 0

#define REG_DIRA 0x02 // Zen Red uses: 0x02
#define REG_DIRB 0x03 // Zen Red uses: 0x03
#define REG_OUTA 0x00 // Zen Red uses: 0x00
#define REG_OUTB 0x01 // Zen Red uses: 0x01

#ifndef _I2C_H_
#define _I2C_H_

void I2C_init();
void I2C_wait();
void I2C_cleanup(void);

void *I2CDisplayThread(void *args);

#endif