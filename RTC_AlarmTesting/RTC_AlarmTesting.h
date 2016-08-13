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
const uint16_t TIMECHECK_TIMEOUT = 5000;
const int NUMALARMS = 10;

// Miscellaneous
tmElements_t timeAlarms[NUMALARMS];
long displayClockTime = 0;
long timeCheck = 0;


// Function definitions
void digitalClockDisplay();
void print2digits(int digits);
time_t newTime(int hr,int min,int sec,int dy, int mnth, int yr);
time_t newTime(int hr,int min,int sec);

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

time_t newTime(int hr, int min, int sec, int dy, int mnth, int yr) {
    tmElements_t tempTime;
    // year can be given as full four digit year or two digits (2010 or 10 for 2010);
    //it is converted to years since 1970
    if (yr > 99)
        yr = yr - 1970;
    else
        yr += 30;
    tempTime.Year = yr;
    tempTime.Month = mnth;
    tempTime.Day = dy;
    tempTime.Hour = hr;
    tempTime.Minute = min;
    tempTime.Second = sec;
    return makeTime(tempTime);
}
time_t newTime(int hr, int min, int sec) {
    tmElements_t tempTime;
    tempTime.Hour = hr;
    tempTime.Minute = min;
    tempTime.Second = sec;
    return makeTime(tempTime);
}

//Do not add code below this line
#endif /* _RTC_AlarmTesting_H_ */
