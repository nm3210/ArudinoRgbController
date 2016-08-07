// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _ButtonTesting_H_
#define _ButtonTesting_H_
#include "Arduino.h"
//add your includes for the project ButtonTesting here

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project ButtonTesting here
const uint8_t BTN_DOWN = 0;
const uint8_t BTN_UP = 1;
const uint16_t DEBOUNCE = 100;
const uint16_t LONGPRESS_HOLDTIME = 500;
enum clickState {CLICK_NULL, CLICK_SINGLE, CLICK_LONG};

byte ledPinR = 9;
byte ledPinG = 10;
byte ledPinB = 11;

// RAM-specific variables to keep track within the interrupts
volatile unsigned long buttonCurTime = 0;
volatile unsigned long buttonLastTime = 0;
volatile unsigned long buttonDownTime = 0;
volatile unsigned long buttonUpTime = 0;
//volatile unsigned int currentState = 0;
volatile boolean changeMode = false;
volatile int buttonCurState = BTN_UP; // Button set to high/up
volatile bool buttonClickIgnoreSingle = false;
volatile bool buttonClickIgnoreLong = false;
volatile clickState curClickState = CLICK_NULL;



unsigned int count = 0;

clickState checkBtnState();
int checkBtnState(int b1, int b2, unsigned long t1, unsigned long t2);
void clearBtnState();
void writeRGB(int r, int g, int b);

//Do not add code below this line
#endif /* _ButtonTesting_H_ */
