// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _IrRemoteTesting_H_
#define _IrRemoteTesting_H_
#include "Arduino.h"
//add your includes for the project IrRemoteTesting here
#include "IRremote.h"
#include "Time.h"
//end of add your includes here

// Constant variables
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;

// RAM-specific variables to keep track while handling the interrupts
volatile unsigned long interruptCurTime = 0;
volatile unsigned long interruptLastTime = 0;
volatile bool changeMode = false;

// Miscellaneous
long displayClockTime = 0;
uint16_t count = 0;

// Button/mode variables
int curMode = 0;

// Function declarations
void interruptIr();
bool decodeIrSignal();
void digitalClockDisplay();
void print2digits(int digits);

//Do not add code below this line
#endif /* _IrRemoteTesting_H_ */
