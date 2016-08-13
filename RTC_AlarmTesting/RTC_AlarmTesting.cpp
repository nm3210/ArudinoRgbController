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


    // Set up alarms (must be monotonic)
    timeAlarms[0] = calcTOD( 0, 00, 00); // The first alarm should be the earlier time
    timeAlarms[1] = calcTOD( 5, 45, 00);
    timeAlarms[2] = calcTOD( 6, 15, 00);
    timeAlarms[3] = calcTOD( 6, 30, 00);
    timeAlarms[4] = calcTOD( 7, 30, 00);
    timeAlarms[5] = calcTOD(10, 00, 00);
    timeAlarms[6] = calcTOD(14, 00, 00);
    timeAlarms[7] = calcTOD(16, 00, 00);
    timeAlarms[8] = calcTOD(22, 00, 00);
    timeAlarms[9] = calcTOD(23, 59, 59); // The last alarm should be the latest time

}

// The loop function is called in an endless loop
void loop() {
    // DO STUFF HERE FOR THE ALARMS
    switch(alarmState){
    case 0:
    case 1:
    case 2:
    default:
        break;
    }

    // Check alarm times versus the current time
    if(abs(millis() - alarmCheck) > ALARMCHECK_TIMEOUT){
        alarmCheck = millis();

        // Grab current time of day
        long curTime = calcTOD(hour(),minute(),second());

        // Loop over all alarms (minus one, for the comparison)
        for(int i = 0; i<NUMALARMS-1; i++){
            // Check if our current time is between a set of alarms
            if(timeAlarms[i] < curTime && curTime < timeAlarms[i+1] && alarmState!=i){
                alarmState = i;
            }
        }
    }

    // Print out current time (once every second)
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
        digitalClockDisplay();

        Serial.print("AlarmState = ");
        Serial.print(alarmState);
        Serial.print(" > CurTime:");
    }

    Alarm.delay(0);
}
