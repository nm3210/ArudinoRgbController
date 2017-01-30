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
            writeHSI(wrapCount(360),1.0,0.1); Alarm.delay(25);
            break;
        case 4: // timeAlarms[4] = calcTOD( 7, 30, 00); //   7:30am
            writeHSI(wrapCount(360),1.0,1.0); Alarm.delay(25);

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
        writeHSI(wrapCount(360),1.0,1.0); Alarm.delay(25);
        break;

    case 4 :
        writeHSI(loopCount(120)+120,1.0,1.0); Alarm.delay(25);
        break;

    case 5 :
        writeHSI(wrapCount(360),1.0,0.1); Alarm.delay(25);
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
                doAlarmOnceFlag = true;
                curAlarm = i;
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
    interruptCurTime = millis();
    //check to see if the interrupt was called in the last 250 milliseconds
    if (abs(interruptCurTime - interruptLastTime) > 250) {
        interruptLastTime = interruptCurTime;
        changeMode = true;
    }
}
// Functions
void rtcCorrection(){
    digitalClockDisplay();
    Serial.print("rtcCorrection: ");
    // Correct the clock
    long singleWeekDelta = (long) -offsetSecPerYear / 365 * 7;
    Serial.print(singleWeekDelta);
    adjustTime(singleWeekDelta);
    // Re-set the time
    RTC.set(now());
    Serial.println(" Done.");
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

bool doModeOnce(){
    if(doModeOnceFlag){
        doModeOnceFlag = false;
        return true;
    } else {
        return false;
    }
}

bool doAlarmOnce(){
    if(doAlarmOnceFlag){
        doAlarmOnceFlag = false;
        return true;
    } else {
        return false;
    }
}

int wrapCount(int mod){
    count = (count + 1) % mod;
    return count;
}

int loopCount(int mod){
    if(count<0){
        countMode=0;
    } else if(count>mod){
        countMode=1;
    }
    if (countMode == 0){
        count = (count + 1);
    } else if(countMode == 1){
        count = (count - 1);
    }
    return count;
}

void writeRGB(int r, int g, int b) {
    // Write the pin values out (this assumes high is off, and low is on);
    analogWrite(ledPinR, 255-r);
    analogWrite(ledPinG, 255-g);
    analogWrite(ledPinB, 255-b);
}

void hsi2rgb(int h, float Sat, float Inten, int* rgb) {
    // From: http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi
    int r, g, b; float Hue;
    Hue = fmod(h,360); // cycle H around to 0-360 degrees
    Hue = M_PI*Hue/(float)180; // Convert to radians.
    Sat = Sat>0?(Sat<1?Sat:1):0; // clamp S and I to interval [0,1]
    Inten = Inten>0?(Inten<2?Inten:2):0;

    // Math! Thanks in part to Kyle Miller.
    if(Hue < M_2PI3) { // First third
        r = 255*Inten/3*(1+Sat*(  cos(Hue)/cos(M_1PI3-Hue)));
        g = 255*Inten/3*(1+Sat*(1-cos(Hue)/cos(M_1PI3-Hue)));
        b = 255*Inten/3*(1-Sat);
    } else if(Hue < M_4PI3) { // Second third
        Hue = Hue - M_2PI3;
        r = 255*Inten/3*(1-Sat);
        g = 255*Inten/3*(1+Sat*(  cos(Hue)/cos(M_1PI3-Hue)));
        b = 255*Inten/3*(1+Sat*(1-cos(Hue)/cos(M_1PI3-Hue)));
    } else { // Third section
        Hue = Hue - M_4PI3;
        r = 255*Inten/3*(1+Sat*(1-cos(Hue)/cos(M_1PI3-Hue)));
        g = 255*Inten/3*(1-Sat);
        b = 255*Inten/3*(1+Sat*(  cos(Hue)/cos(M_1PI3-Hue)));
    }
    rgb[0]=r>0?(r<255?r:255):0;//r;
    rgb[1]=g>0?(g<255?g:255):0;//g;
    rgb[2]=b>0?(b<255?b:255):0;//b;
}

void writeHSI(int h, float s, float i) {
    int rgbTemp[3];
    hsi2rgb(h,s,i,rgbTemp);
    writeRGB(rgbTemp);
}

void writeRGB(int rgb[]){
    writeRGB(rgb[0],rgb[1],rgb[2]);
}

void writeRGB(Color c){
    PREV = c; // Save the new color to the 'previous' value
    writeRGB(c.red, c.green, c.blue);
}

void blinkRGB(Color c){
    blinkRGB(c,25);
}

void blinkRGB(Color c, int d){
    writeRGB(c);
    delay(d);
    writeRGB(OFF);
    delay(d);
}

void blinkRGBnTimes(Color c, int count){
    for(int i=0;i<count;i++){
        blinkRGB(c);
    }
}

void crossFadeHSI(int h1, float s1, float i1,
                  int h2, float s2, float i2, long steps, int dur){
    if(changeMode){writeRGB(OFF);return;}; // Check for button press
    float rho1 = i1;//(i1!=0)?i1:LOWERLIMIT;
    float rho2 = i2;//(i2!=0)?i2:LOWERLIMIT;

    bool ccw = fmod((h2-h1+360.0),360.0) < 180.0;
    float gamma = (180 - abs(fmod(abs(h1-h2),360.0) - 180.0));
    float length = sqrt(pow(rho1,2)-2.0*rho1*rho2*cos(radians(gamma))+pow(rho2,2));
    float alpha;
    if(rho1 != 0){
        alpha = acos((pow(rho1,2)+pow(length,2)-pow(rho2,2))/(2.0*rho1*length));
    } else {
        alpha = acos((pow(rho2,2)+pow(length,2)-pow(rho1,2))/(2.0*rho2*length));
    }

    float newRho;
    uint16_t newTheta;
    // Loop through the number of steps
    for (int i = 1; i <= steps; i++) {
        if(changeMode){writeRGB(OFF);return;}; // Check for button press (so you can escape the loop)

        float tempRho = length*((i-1.0)/(steps-1.0)); // calculate one part of the line

        if (tempRho != 0){
            newRho = sqrt(pow(rho1,2) - 2.0*rho1*tempRho*cos(alpha)+pow(tempRho,2));
        } else {
            newRho = rho1;
        }
        if ((rho1 != 0) && (newRho != 0)){
            float acosarg = (pow(rho1,2)+pow(newRho,2)+-pow(tempRho,2))/(2.0*rho1*newRho);
            newTheta = round(degrees((round(acosarg*1000000)/1000000 == 1)?0:acos(acosarg))*1000)/1000;
        } else {
            if(fmod(h1-h2,360)<180){
                newTheta = fmod(h1-h2,360);
            } else {
                newTheta = -fmod(h1-h2,360);
            }
        }
        if (ccw){
            newTheta = fmod(h1+newTheta,360.0);
        } else {
            newTheta = fmod(h1-newTheta+360,360.0);
        }

        float newSat = s1+(s2-s1)*((i-1.0)/(steps-1.0)); // simple interpolation
//        float newInten = i1+(i2-i1)*((i-1.0)/(steps-1.0)); // simple interpolation
        writeHSI(newTheta, newSat, newRho);
        Alarm.delay(dur);

    }
}

void crossFade(Color c1, Color c2, long steps, int dur){
    if(c1 == OFF){
        c1.hue = c2.hue;
        c1.sat = c2.sat;
    } else if(c2 == OFF){
        c2.hue = c1.hue;
        c2.sat = c1.sat;
    }
    PREV = c2; // Save the new color to the 'previous' value
    crossFadeHSI(c1.hue, c1.sat, c1.intensity,
                 c2.hue, c2.sat, c2.intensity, steps, dur);
}

void crossFade(Color c1, Color c2, float msec){
    int maxDelay = 5; // set max delay to 5ms (wiggle room)
    if(msec<maxDelay){return;};
    int timeStep = max(maxDelay, msec / 100); // default to 100 steps (pretty smooth)
    long numSteps = (long) (msec/timeStep);

    crossFade(c1, c2, numSteps, timeStep);
}

void crossFadeTo(Color c1, float msec){
    if(PREV!=c1){
        crossFade(PREV,c1,msec);
    }
}

void waitForButton(unsigned long i){
    unsigned long trackTime = millis();
    boolean waitLoop = true;
    while(waitLoop){
        if(changeMode){writeRGB(OFF);waitLoop = false;return;};
        if(abs(millis() - trackTime) > i){
            waitLoop = false;
            return;
        }
        Alarm.delay(0);
    }
}

long calcTOD(int hr, int min, int sec){
    // Calculate the Time Of Day, in seconds, since midnight
    return hr * SECS_PER_HOUR + min * SECS_PER_MIN + sec;
}

Color adjustSat(Color c, float newSat){
    return Color(c.hue,newSat,c.intensity);
}

Color adjustInt(Color c, float newInt){
    return Color(c.hue,c.sat,newInt);
}
