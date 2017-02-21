// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _NeoPixelTesting_H_
#define _NeoPixelTesting_H_
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "Time.h"

// Constant variables
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;

// RAM-specific variables to keep track while handling the interrupts
volatile unsigned long interruptCurTime = 0;
volatile unsigned long interruptLastTime = 0;
volatile bool changeMode = false;

// Button/mode variables
int curMode = 0;
int countMode = 0;
bool doModeOnceFlag = true;

// Miscellaneous
long displayClockTime = 0;
long alarmCheckTime = 0;
uint16_t count = 0;

// Function declarations
void digitalClockDisplay();
void print2digits(int digits);
void fullColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

//Do not add code below this line
#endif /* _NeoPixelTesting_H_ */
