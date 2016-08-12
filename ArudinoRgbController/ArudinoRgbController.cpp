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

    // Set up alarms
    Alarm.alarmRepeat( 5, 45, 00, alarmToRed);   // Turn color to RED
    Alarm.alarmRepeat( 6, 15, 00, alarmToYellow);// Turn clock to YELLOW
    Alarm.alarmRepeat( 6, 30, 00, alarmToGreen); // Turn clock to GREEN
    Alarm.alarmRepeat( 7, 30, 00, alarmToOff);   // Turn clock to OFF

//    // Set up dynamic modes
//    Alarm.alarmRepeat( 7, 30, 00, setDynamicMode00);
//    Alarm.alarmRepeat( 6, 00, 00, setDynamicMode01);
//    Alarm.alarmRepeat(19, 00, 00, setDynamicMode02);
//    Alarm.alarmRepeat(22, 00, 00, setDynamicMode03);

    Alarm.alarmRepeat(dowWednesday, 12, 00, 00, rtcCorrection); // Correct the clock for a bit

    // Make sure LED is off
    writeRGB(OFF);
}

void loop() {
    // Handle button press
    if(changeMode){
        changeMode = false;
        doOnceFlag = true;
        count = 0;
        curMode++;
        Serial.print("curMode = "); Serial.println(curMode);
        blinkRGBnTimes(WHITE,2);
    }

    // Switch through different modes
    switch(curMode){
    case offState :
        // OFF State
        if(doOnce()){
            crossFade(WHITE,OFF,1500);
        }
        break;

    case alarmState :
        // Alarm State
        if(doOnce()){
            crossFadeTo(WHITE,200);
            crossFadeTo(RED,200);
            crossFadeTo(YELLOW,200);
            crossFadeTo(GREEN,200);
            crossFadeTo(OFF,200);
        }
        break;

    case dynamicState :
        curMode++; // Skip dynamic mode for now
//        // Dynamic State
//        if(doOnce()){
//            crossFadeTo(PINK,200);
//            blinkRGBnTimes(PINK,2); writeRGB(PINK);
//            crossFadeTo(PURPLE,200);
//            blinkRGBnTimes(PURPLE,2); writeRGB(PURPLE);
//            crossFadeTo(BLUE,200);
//            blinkRGBnTimes(BLUE,2); writeRGB(BLUE);
//            crossFadeTo(OFF,200);
//        }
//        switch(dynamicMode){
//        case 0:
//            writeHSI(loopCount(360),1.0,1.0); Alarm.delay(25);
//            break;
//        case 1:
//            writeHSI(loopCount(360),1.0,0.1); Alarm.delay(25);
//            break;
//        case 2:
//            writeHSI(loopCount(120)+120,1.0,1.0); Alarm.delay(25);
//            break;
//        case 3:
//            writeHSI(loopCount(120)+120,1.0,0.1); Alarm.delay(25);
//            break;
//        default:
//            dynamicMode = 0; // Loop back
//            break;
//        }
        break;

    case 3 :
        writeHSI(loopCount(360),1.0,1.0); Alarm.delay(25);
        break;

    case 4 :
        writeHSI(loopCount(120)+120,1.0,1.0); Alarm.delay(25);
        break;

    case 5 :
        writeHSI((loopCount(120)+300)%360,1.0,1.0); Alarm.delay(25);
        break;

    case 6 :
        writeHSI(loopCount(360),1.0,0.1); Alarm.delay(25);
        break;

    case 7 :
        writeHSI(loopCount(120)+120,1.0,0.1); Alarm.delay(25);
        break;

    case 8 :
        writeHSI((loopCount(120)+300)%360,1.0,0.1); Alarm.delay(25);
        break;

    case 9 :
        writeRGB(WHITE);
        break;

    case 10 :
        writeRGB(PINK);
        break;

    case 11 :
        writeRGB(PURPLE);
        break;

    case 12 :
        writeRGB(ORANGE);
        break;

    case 13 :
        writeRGB(YELLOW);
        break;

    default:
        curMode = 0; // Loop back
        break;
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
        // Call our interrupt function
//        btnInterrupt();
    }
}

void alarmToRed(){
    Serial.println("alarmToRed");
    if(curMode!=alarmState){return;}
    crossFadeTo(adjustInt(RED,0.1),1000.0*30.0);
}
void alarmToYellow(){
    Serial.println("alarmToYellow");
    if(curMode!=alarmState){return;}
    crossFadeTo(adjustInt(YELLOW,0.2),1000.0*30.0);
}
void alarmToGreen(){
    Serial.println("alarmToGreen");
    if(curMode!=alarmState){return;}
    crossFadeTo(adjustInt(GREEN,0.3),1000.0*5.0);
}
void alarmToOff(){
    Serial.println("alarmToOff");
    if(curMode!=alarmState){return;}
    crossFadeTo(OFF,1000.0*90.0);
}

//void setDynamicMode00(){
//    if(curMode!=dynamicState){return;}
//    dynamicMode = 0;
//}
//void setDynamicMode01(){
//    if(curMode!=dynamicState){return;}
//    dynamicMode = 1;
//}
//void setDynamicMode02(){
//    if(curMode!=dynamicState){return;}
//    dynamicMode = 2;
//}
//void setDynamicMode03(){
//    if(curMode!=dynamicState){return;}
//    dynamicMode = 3;
//}
