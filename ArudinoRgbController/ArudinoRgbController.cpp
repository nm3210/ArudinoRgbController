// Do not remove the include below
#include "ArudinoRgbController.h"

void setup() {
    pinMode(A2,OUTPUT);
    pinMode(A3,OUTPUT);
    digitalWrite(A2,LOW);  // Turn on the ground for the DS1307
    digitalWrite(A3,HIGH); // Turn on the +5vcc for the DS1307

    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD)  |  // Analog Comparator Enabled
           (0 << ACBG) |  // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
           (0 << ACO)  |  // Analog Comparator Output: Off
           (1 << ACI)  |  // Analog Comparator Interrupt Flag: Clear Pending Interrupt
           (1 << ACIE) |  // Analog Comparator Interrupt: Enabled
           (0 << ACIC) |  // Analog Comparator Input Capture: Disabled
           (1 << ACIS1)| (0 << ACIS0); // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

    // Turn on serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");

    // Enable LED pins
    pinMode(ledPinR, OUTPUT);
    pinMode(ledPinG, OUTPUT);
    pinMode(ledPinB, OUTPUT);

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
    timeAlarms[0] = calcTOD( 0, 00, 00); // The first alarm should be the earliest time
    timeAlarms[1] = calcTOD( 5, 45, 00); //   5:45am
    timeAlarms[2] = calcTOD( 6, 15, 00); //   6:15am
    timeAlarms[3] = calcTOD( 6, 30, 00); //   6:30am
    timeAlarms[4] = calcTOD( 7, 30, 00); //   7:30am
    timeAlarms[5] = calcTOD(19, 00, 00); //   7:00pm
    timeAlarms[6] = calcTOD(22, 00, 00); //  10:00pm
    timeAlarms[7] = calcTOD(23, 59, 59); // The last alarm should be the latest time

    Alarm.alarmRepeat(dowWednesday, 12, 00, 00, rtcCorrection); // Correct the clock for a bit

    // Make sure LED is off
    writeRGB(OFF);
}

void loop() {
    // Handle button press
    if(changeMode){
        changeMode = false;
        doModeOnceFlag = true;
        count = 0;
        curMode++;
        Serial.print("curMode = "); Serial.println(curMode);
        blinkRGBnTimes(WHITE,2);
    }

    // Switch through different modes
    switch(curMode){
    case offMode :
        // OFF State (just don't do anything)
        if(doModeOnce()){
            crossFade(WHITE,OFF,1500);
        }
        break;

    case alarmMode :
        // Alarm State
        if(doModeOnce()){
            crossFadeTo(WHITE,200);
            crossFadeTo(RED,200);
            crossFadeTo(YELLOW,200);
            crossFadeTo(GREEN,200);
            crossFadeTo(OFF,200);
            waitForButton(1000);
        }
        if(doAlarmOnce()){
            switch(curAlarm){
            case 1: // timeAlarms[1] = calcTOD( 5, 45, 00);
                crossFadeTo(adjustInt(RED,0.1),1000.0*30.0);
                break;
            case 2: // timeAlarms[2] = calcTOD( 6, 15, 00);
                crossFadeTo(adjustInt(YELLOW,0.2),1000.0*30.0);
                break;
            case 3: // timeAlarms[3] = calcTOD( 6, 30, 00);
                crossFadeTo(adjustInt(GREEN,0.3),1000.0*5.0);
                break;
            case 4: // timeAlarms[4] = calcTOD( 7, 30, 00);
                crossFadeTo(OFF,1000.0*90.0);
                break;
            default:
                break;
            }
        }
        break;

    case dynamicMode :
        // Dynamic State
        if(doModeOnce()){
            crossFadeTo(PINK,250);
            crossFadeTo(PURPLE,250);
            crossFadeTo(BLUE,250);
            crossFadeTo(OFF,250);
            waitForButton(1000);
        }
        switch(curAlarm){
        case 1: // timeAlarms[1] = calcTOD( 5, 45, 00); //   5:45am
        case 2: // timeAlarms[2] = calcTOD( 6, 15, 00); //   6:15am
        case 3: // timeAlarms[3] = calcTOD( 6, 30, 00); //   6:30am
            writeHSI(count++ % 360,1.0,0.1); Alarm.delay(25);
            break;
        case 4: // timeAlarms[4] = calcTOD( 7, 30, 00); //   7:30am
            writeHSI(count++ % 360,1.0,1.0); Alarm.delay(25);
            break;
        case 5: // timeAlarms[5] = calcTOD(19, 00, 00); //   7:00pm
            writeHSI(loopCount(120)+120,1.0,1.0); Alarm.delay(25);
            break;
        case 0: // timeAlarms[0] = calcTOD( 0, 00, 00);
        case 6: // timeAlarms[6] = calcTOD(22, 00, 00); //  10:00pm
        case 7: // timeAlarms[7] = calcTOD(23, 59, 59);
        default:
            writeHSI(loopCount(120)+120,1.0,0.1); Alarm.delay(25);
            break;
        }
        break;

    case 3 :
        writeHSI(count++ % 360,1.0,1.0); Alarm.delay(25);
        break;

    case 4 :
        writeHSI(loopCount(120)+120,1.0,1.0); Alarm.delay(25);
        break;

    case 5 :
        writeHSI(count++ % 360,1.0,0.1); Alarm.delay(25);
        break;

    case 6 :
        writeHSI(loopCount(120)+120,1.0,0.1); Alarm.delay(25);
//        writeHSI((loopCount(120)+300)%360,1.0,1.0); Alarm.delay(25);
        break;

    default:
        curMode = 0; // Loop back
        break;
    }

    // Check alarm times versus the current time
    if(abs(millis() - alarmCheckTime) > ALARMCHECK_TIMEOUT){
        alarmCheckTime = millis();

        // Grab current time of day
        long curTime = calcTOD(hour(),minute(),second());

        // Loop over all alarms (minus one, for the comparison)
        for(int i = 0; i<NUMALARMS-1; i++){
            // Check if our current time is between a set of alarms
            if(timeAlarms[i] < curTime && curTime < timeAlarms[i+1] && curAlarm!=i){
                curAlarm = i;
                doAlarmOnceFlag = true;
            }
        }
    }

    // Print out current time (once every second)
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
        digitalClockDisplay();
    }

    // This Alarm.delay is needed within the loop() function in order to keep the timers/alarms alive and working
    Alarm.delay(0);
}

// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other [complex] functions
ISR(ANALOG_COMP_vect) {
    buttonCurTime = millis();
    //check to see if the interrupt was called in the last 250 milliseconds
    if (abs(buttonCurTime - buttonLastTime) > 250) {
        buttonLastTime = buttonCurTime;
        changeMode = true;
    }
}
