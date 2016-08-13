// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _RTC_AlarmTesting_H_
#define _RTC_AlarmTesting_H_
#include "Arduino.h"
#include "Time.h"
#include "TimeAlarms.h"
#include "DS1307RTC.h"
//add your includes for the project RTC_AlarmTesting here

// Constant variables
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;
const uint16_t ALARMCHECK_TIMEOUT = 5000;
const int NUMALARMS = 10;


// Miscellaneous
long timeAlarms[NUMALARMS];
int alarmState = 0;
long displayClockTime = 0;
long alarmCheck = 0;


// Function definitions
void digitalClockDisplay();
void print2digits(int digits);
long calcTOD(int hr, int min, int sec);


// Functions
void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(year());
    Serial.print("-");
    print2digits(month());
    Serial.print("-");
    print2digits(day());
    Serial.print(" ");
    print2digits(hour());
    Serial.print(":");
    print2digits(minute());
    Serial.print(":");
    print2digits(second());
    Serial.println();
}

void print2digits(int number) {
    if (number >= 0 && number < 10) {
        Serial.write('0');
  }
  Serial.print(number);
}

long calcTOD(int hr, int min, int sec){
    // Calculate the Time Of Day, in seconds, since midnight
    return hr * SECS_PER_HOUR + min * SECS_PER_MIN + sec;
}

//Do not add code below this line
#endif /* _RTC_AlarmTesting_H_ */
