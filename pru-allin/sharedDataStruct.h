#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>


#define STR_LEN         8       // # LEDs in our string

#define OFF_LED     0x00000000 // Off

#define Green   0x0f000000 // Green
#define Red     0x000f0000 // Red
#define Blue    0x00000f00 // Blue
#define White   0x0000000f // White
#define Yellow  0x0f0f0000 // Yellow
#define Purple  0x000f0f00 // Purple
#define Teal    0x0f000f00 // Teal

// Try these; they are birght! 
// (You'll need to comment out some of the above)

#define GreenBright 0xff000000 // Green Bright
#define RedBright   0x00ff0000 // Red Bright
#define BlueBright  0x0000ff00 // Blue Bright
#define WhiteBright 0xffffff00 // White

#define BrightGreenWhite    0xff0000ff // Green Bright w/ Bright White
#define BrightRedWhite      0x00ff00ff // Red Bright w/ Bright White
#define BrightBlueWhite     0x0000ffff // Blue Bright w/ Bright White
#define BrightWhiteWhite    0xffffffff // White w/ Bright White

// WARNING:
// Fields in the struct must be aligned to match ARM's alignment
// bool/char, uint8_t: byte aligned
// int/long, uint32_t: word (4 byte) aligned
// double, uint64_t: dword (8 byte) aligned
// Add padding fields (char _p1) to pad out to alignment.

// My Shared Memory Structure
// ----------------------------------------------------------------
typedef struct {
    uint32_t neoPixels[STR_LEN];
} sharedMemStruct_t;

#endif
