#ifndef _UTILS_H_
#define _UTILS_H_

void __attribute__((optimize("O0"))) runCommand(char*);

int readFromFileToScreen(char*);

void sleepForMs(long long);

void writeToFile(char*, int);

int initI2cBus(char* , int );

void writeI2cReg(int , unsigned char , unsigned char );

#endif