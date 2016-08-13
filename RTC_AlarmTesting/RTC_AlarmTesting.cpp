// Do not remove the include below
#include "RTC_AlarmTesting.h"

//The setup function is called once at startup of the sketch
void setup() {
    pinMode(A2,OUTPUT);
    pinMode(A3,OUTPUT);
    digitalWrite(A2,LOW);  // Turn on the ground for the DS1307
    digitalWrite(A3,HIGH); // Turn on the +5vcc for the DS1307

    // Turn on serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");

    // Get the current time from the DS1307
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
        Serial.println("RTC: Unable to sync with the RTC, setting time to: ");
        setTime(00,00,00,01,01,2015);
    } else {
        Serial.print("RTC: Successfully set Arduino time to: ");
    }
    digitalClockDisplay();


    // Set up alarms
    breakTime(newTime( 5, 45, 00),timeAlarms[0]); // this first assignment doesn't work for some reason
    breakTime(newTime( 5, 45, 00),timeAlarms[0]);
    breakTime(newTime( 6, 15, 00),timeAlarms[1]);
    breakTime(newTime( 6, 30, 00),timeAlarms[2]);
    breakTime(newTime( 7, 30, 00),timeAlarms[3]);
    breakTime(newTime( 9, 30, 00),timeAlarms[4]);
    breakTime(newTime(10, 30, 00),timeAlarms[5]);
    breakTime(newTime(11, 30, 00),timeAlarms[6]);
    breakTime(newTime(12, 30, 00),timeAlarms[7]);
    breakTime(newTime(14, 30, 00),timeAlarms[8]);
    breakTime(newTime(15, 30, 00),timeAlarms[9]);

}

// The loop function is called in an endless loop
void loop() {
    // Run commands every so often
    if(abs(millis() - timeCheck) > TIMECHECK_TIMEOUT){
        timeCheck = millis();
        // Check current time
        time_t curTime = now();
        tmElements_t curTimeElements;
        breakTime(curTime,curTimeElements);

        int index = 0;
        for(index = 0; index<NUMALARMS-1; index++){
            Serial.print("Time Check #"); print2digits(index); Serial.print(": ");
            Serial.print("CurTime=");
                print2digits(curTimeElements.Hour);
                Serial.print(":");
                print2digits(curTimeElements.Minute);
                Serial.print(":");
                print2digits(curTimeElements.Second);
                Serial.print("   >  ");
            Serial.print("timeAlarm=");
                print2digits(timeAlarms[index].Hour);
                Serial.print(":");
                print2digits(timeAlarms[index].Minute);
                Serial.print(":");
                print2digits(timeAlarms[index].Second);
            if(timeAlarms[index].Hour >= curTimeElements.Hour &&
                    timeAlarms[index].Minute >= curTimeElements.Minute &&
                    timeAlarms[index].Second >= curTimeElements.Second){
                Serial.print(" --> Check passed");
            } else {
                Serial.print(" --> Check FAILED");
            }
            Serial.println();
        }


//        Serial.print("Time checks: ");
//        Serial.print(index);


//        Serial.println();
    }

    // Print out current time (once every second)
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
        digitalClockDisplay();
    }
}
