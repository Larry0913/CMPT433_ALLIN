#ifndef _NEO_PIXEL_H_
#define _NEO_PIXEL_H_


// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000 // Length of PRU memory
#define PRU0_DRAM 0x00000 // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000 // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)

#include "sharedDataStruct.h"

//init pixel configure
void neoPixel_init(volatile sharedMemStruct_t* pSharedPru0_temp);

// Return the address of the PRU's base memory
volatile void* getPruMmapAddr(void); 
void freePruMmapAddr(volatile void* pPruBase);

void *neoPixelThread(void *args);

void setPixelColor(int color);

//turn off all pixels
void clearStrip();

void flashTeal();
void flashBlue();



#endif