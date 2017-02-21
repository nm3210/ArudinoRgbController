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

// Define buttons
#define BTNCTRL1_1   0xFFA25D
#define BTNCTRL1_2   0xFF629D
#define BTNCTRL1_3   0xFFE21D
#define BTNCTRL1_4   0xFF22DD
#define BTNCTRL1_5   0xFF02FD
#define BTNCTRL1_6   0xFFC23D
#define BTNCTRL1_7   0xFFE01F
#define BTNCTRL1_8   0xFFA857
#define BTNCTRL1_9   0xFF906F
#define BTNCTRL1_0   0xFF9867
#define BTNCTRL1_ASTRX   0xFF6897
#define BTNCTRL1_POUND   0xFFB04F
#define BTNCTRL1_SELECT  0xFF38C7
#define BTNCTRL1_UP      0xFF18E7
#define BTNCTRL1_DOWN    0xFF4AB5
#define BTNCTRL1_LEFT    0xFF10EF
#define BTNCTRL1_RIGHT   0xFF5AA5

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
bool handleInterrupt();
bool decodeIrSignal();
void digitalClockDisplay();
void print2digits(int digits);

//Do not add code below this line
#endif /* _IrRemoteTesting_H_ */
