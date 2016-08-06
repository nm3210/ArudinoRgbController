// Do not remove the include below
#include "ArudinoRgbController.h"

byte ledPinR = 9;
byte ledPinG = 10;
byte ledPinB = 11;

const int numStates = 9;
const int alarmState = 0; // this is the state number of when the alarms will go off (see if-statements in loop())

tmElements_t tm;

void setup() {
    pinMode(A2,OUTPUT);
    pinMode(A3,OUTPUT);
    digitalWrite(A2,LOW); // Turn on the ground for the DS1307
    digitalWrite(A3,HIGH);// Turn on the +5vcc for the DS1307

    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD) |     // Analog Comparator Enabled
            (0 << ACBG) |   // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
            (0 << ACO) |    // Analog Comparator Output: Off
            (1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt
            (1 << ACIE) |   // Analog Comparator Interrupt: Enabled
            (0 << ACIC) |   // Analog Comparator Input Capture: Disabled
            (1 << ACIS1) | (0 << ACIS0); // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

    // Turn on the serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");

    // Enable our LED pinouts
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

    // Set up our alarms
    Alarm.alarmRepeat(6, 15, 00, alarm01);  // Turn color to RED
    Alarm.alarmRepeat(6, 30, 00, alarm02);  // Turn clock to YELLOW
    Alarm.alarmRepeat(6, 45, 00, alarm03);  // Turn clock to GREEN
//    Alarm.alarmRepeat(23, 55, 00, alarm03);  // Turn clock to GREEN
//    Alarm.alarmRepeat(23, 50, 10, alarm03);  // Turn clock to GREEN
//    Alarm.alarmRepeat(23, 54, 55, alarm04);  // Turn clock to GREEN

    clockCorrect = Alarm.alarmRepeat(dowWednesday, 12, 00, 00, rtcCorrection); // Correct the clock for a bit

    writeRGB(OFF);
}

void loop() {
    if(switchState == alarmState){
        if(changeState){blinkRGB(OFF);changeState=false;};
        // Alarm state, leave blank to let the alarms turn on
//        writeRGB(OFF);
    } else if (switchState == 1) {
        if(changeState){blinkRGB(OFF);changeState=false;};
        writeRGB(ON);
    } else if (switchState == 2) {
        if(changeState){blinkRGB(OFF);changeState=false;};
//        writeRGB(RED);
        writeRGB(PINK);
    } else if (switchState == 3) {
        if(changeState){blinkRGB(OFF);changeState=false;};
//        writeRGB(GREEN);
        writeRGB(ORANGE);
    } else if (switchState == 4) {
        if(changeState){blinkRGB(OFF);changeState=false;};
//        writeRGB(BLUE);
        writeRGB(GREEN);
    } else if (switchState == 5) {
        if(changeState){blinkRGB(OFF);changeState=false;};
//        crossFadeState = switchState;
//        if(switchState!=crossFadeState){writeRGB(OFF);return;};
//        crossFade(OFF,0,0);
//        crossFade(RED,0,0);
        writeRGB(BLUE);
    } else if (switchState == 6) {
        if(changeState){blinkRGB(OFF);changeState=false;};
//        crossFadeState = switchState;
//        if(switchState!=crossFadeState){writeRGB(OFF);return;};
//        crossFade(OFF,0,0);
//        crossFade(GREEN,0,0);
        writeRGB(VIOLET);
    } else if (switchState == 7) {
        if(changeState){blinkRGB(OFF);changeState=false;};
//        crossFadeState = switchState;
//        if(switchState!=crossFadeState){writeRGB(OFF);return;};
//        crossFade(OFF,0,0);
//        crossFade(BLUE,0,0);
        crossFadeRGB2(switchState);
    } else if (switchState == 8) {
        if(changeState){blinkRGB(OFF);changeState=false;};
        crossFadeRGB(switchState);
    }

    digitalClockDisplay();
    Alarm.delay(100);
}

void alarm01(){
	Serial.println("alarm01");
    if(switchState!=alarmState){return;}
    crossFadeState = switchState;
    crossFade(RED,3);
}

void alarm02(){
	Serial.println("alarm02");
    if(switchState!=alarmState){return;}
    crossFadeState = switchState;
    crossFade(YELLOW,3);
}

void alarm03(){
	Serial.println("alarm03");
    if(switchState!=alarmState){return;}
    crossFadeState = switchState;
    crossFade(GREEN,3);
}

void rtcCorrection(){
	digitalClockDisplay();
	Serial.print("rtcCorrection: ");
    if(switchState!=alarmState){return;}
	// Disable the current alarm
//	Alarm.disable(clockCorrect);
	// Correct the clock
	long singleWeekDelta = (long) -offsetSecPerYear / 365 * 7;
	Serial.print(singleWeekDelta);
    adjustTime(singleWeekDelta);
    RTC.set(now());
    // Wait until AFTER our alarm is going to start again to enable it
//	Serial.print(" & wait for ");
//	Serial.print(singleDayDelay*110);
//    Alarm.delay(singleDayDelay*110);
//    Alarm.enable(clockCorrect);
    Serial.println(". Done.");
    digitalClockDisplay();
}


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

void crossFadeRGB(int curState){
    crossFadeState = curState;
    if(switchState!=crossFadeState){writeRGB(OFF);return;};
    crossFade(RED, 0, 0);
    crossFade(YELLOW, 0, 0);
    crossFade(GREEN, 0, 0);
    crossFade(CYAN, 0, 0);
    crossFade(BLUE, 0, 0);
    crossFade(MAGENTA, 0, 0);
}

void crossFadeRGB2(int curState){
    crossFadeState = curState;
    if(switchState!=crossFadeState){writeRGB(OFF);return;};
    crossFade(BLUE, 3, 0);
    crossFade(CYAN, 3, 0);
    crossFade(GREEN, 3, 0);
    crossFade(CYAN, 3, 0);
}

// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other functions
ISR(ANALOG_COMP_vect) {
    buttonCurTime = millis();
    //check to see if the interrupt was called in the last 250 milliseconds
    if (abs(buttonCurTime - buttonLastTime) > 250) {
        buttonLastTime = buttonCurTime;
        // Call our interrupt function
        btnInterrupt();
    }
}

void btnInterrupt() {
    count++;
    switchState = count % numStates;
    changeState = true;
}

void writeRGB(int r, int g, int b) {
    // Clamp the values to be between the min/max
//    r = max(min(r,255), 0);
//    g = max(min(g,255), 0);
//    b = max(min(b,255), 0);

    redVal = r;
    grnVal = g;
    bluVal = b;

    // Write the pin values out (this assumes high is off, and low is on);
    analogWrite(ledPinR, 255-r);
    analogWrite(ledPinG, 255-g);
    analogWrite(ledPinB, 255-b);
}

int calculateStep(int prevValue, int endValue) {
    int step = endValue - prevValue; // What's the overall gap?
    if (step) {                      // If its non-zero,
        step = 1020 / step;              //   divide by 1020
    }
    return step;
}
int calculateVal(int step, int val, int i) {
    if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
        if (step > 0) {          //   increment the value if step is positive...
            val += 1;
        } else if (step < 0) {       //   ...or decrement it if step is negative
            val -= 1;
        }
    }
    // Defensive driving: make sure val stays in the range 0-255
    return min(max(val,0), 255);
}
void crossFade(Color c) {
    if(switchState!=crossFadeState){writeRGB(OFF);return;}; // Bail out
    int stepR = calculateStep(prevR, c.red);
    int stepG = calculateStep(prevG, c.green);
    int stepB = calculateStep(prevB, c.blue);

    for (int i = 0; i <= 1020; i++) {
        if(switchState!=crossFadeState){writeRGB(OFF);return;}; // Bail out if we set our RAM flag

        redVal = calculateVal(stepR, redVal, i);
        grnVal = calculateVal(stepG, grnVal, i);
        bluVal = calculateVal(stepB, bluVal, i);

        writeRGB(redVal,grnVal,bluVal);

        Alarm.delay(wait); // Pause for 'wait' milliseconds before resuming the loop
    }

    // Update current values for next loop
    prevR = redVal;
    prevG = grnVal;
    prevB = bluVal;
    Alarm.delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}

void blinkRGB(Color c){
    writeRGB(c);
    Alarm.delay(50);
    writeRGB(OFF);
    Alarm.delay(50);
}
