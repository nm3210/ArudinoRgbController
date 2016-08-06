// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _RTC_ReadTime_H_
#define _RTC_ReadTime_H_
#include "Arduino.h"
//add your includes for the project RTC_ReadTime here
//#include "Wire.h"
#include "Time.h"
#include "DS1307RTC.h"

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project RTC_ReadTime here
void btnInterrupt();
void digitalClockDisplay();
void print2digits(int number);


//Do not add code below this line
#endif /* _RTC_ReadTime_H_ */
