// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _ArudinoRgbController_H_
#define _ArudinoRgbController_H_
#include "Arduino.h"
#include "Time.h"
#include "TimeAlarms.h"
#include "DS1307RTC.h"

#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();

#ifdef __cplusplus
} // extern "C"
#endif

// Constant variables
const int offMode = 0;
const int alarmMode = 1; // this is the state number of when the alarms will go off (see if-statements in loop())
const int dynamicMode = 2;
const int NUMALARMS = 8;
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;
const uint16_t ALARMCHECK_TIMEOUT = 1000;
const float M_1PI3 = 1.0 * M_PI / 3.0;
const float M_2PI3 = 2.0 * M_PI / 3.0;
const float M_4PI3 = 4.0 * M_PI / 3.0;
const signed int offsetSecPerYear = 5012; // time offset for DS1307 chip, I think it changes based on how much processing you're doing

// Arduino pins
const byte ledPinR  =  9;
const byte ledPinG  = 10;
const byte ledPinB  = 11;

// RAM-specific variables to keep track while handling the interrupts
volatile unsigned long buttonCurTime = 0;
volatile unsigned long buttonLastTime = 0;
volatile bool changeMode = false;

// Button/mode variables
int curMode = alarmMode;
int curAlarm = 0;
int countMode = 0;
bool doModeOnceFlag = true;
bool doAlarmOnceFlag = true;

// Global HSI variables
uint16_t hsi_Hue = 0;
float hsi_Saturation = 1.0;
float hsi_Intensity = 1.0;

// Miscellaneous
long timeAlarms[NUMALARMS];
long displayClockTime = 0;
long alarmCheckTime = 0;
int count = 0;


// Initialize the main hsi2rgb function
void hsi2rgb(int H, float S, float I, int* rgb);


// Create the color structure
struct Color {
public:
    float sat, intensity;       // ranges from [0.0 to 1.0]
    uint16_t hue;               // ranges from [0 to 360]
    uint8_t red, green, blue;   // ranges from [0 to 255]

    Color(int h, float s, float i){
    	int rgbTemp[3];
    	hue = h; sat = s; intensity = i;
    	// Convert HSI to RGB
    	hsi2rgb(hue, sat, intensity, rgbTemp);
    	red = rgbTemp[0];
    	green = rgbTemp[1];
    	blue = rgbTemp[2];
    	// Store hash
    	hash = (hue+1)*(red+1)*(green+1)*(blue+1);
    }

    bool operator == (const Color &other) { return(this->hash == other.getHash()); }
    bool operator != (const Color &other) { return(this->hash != other.getHash()); }

private:
    int hash;
    const int getHash() const {return(this->hash);}
};


// Set up all the basic colors, these colors are in HSI
//  -->  -->  -->  color   (hue,  sat,  int);
static const Color RED     (  0, 1.00, 1.00);
static const Color GREEN   (120, 1.00, 1.00);
static const Color BLUE    (240, 1.00, 1.00);

static const Color MAGENTA (300, 1.00, 1.00);
static const Color YELLOW  ( 60, 1.00, 1.00);
static const Color CYAN    (180, 1.00, 1.00);

static const Color ORANGE  ( 15, 1.00, 1.00);
static const Color INDIGO  (252, 1.00, 0.20);
static const Color VIOLET  (250, 1.00, 1.00);
//static const Color VIOLET  (252, 1.00, 0.40);
static const Color PINK    (277, 0.90, 1.00);

static const Color WHITE   (  0, 0.00, 1.00);
static const Color DIM     (  0, 0.00, 0.10);
static const Color BLACK   (  0, 0.00, 0.00);
static const Color ON = WHITE;
static const Color OFF = BLACK;
static const Color PURPLE = VIOLET;
Color PREV (0,0,0); // create the 'previous' color variable


// Function declarations
void rtcCorrection();
void digitalClockDisplay();
void print2digits(int digits);
void writeRGB(Color c);
void writeRGB(int rgb[]);
void writeRGB(int r, int g, int b);
void writeHSI(int h, float s, float i);
void blinkRGB(Color c);
void blinkRGB(Color c, int d);
void blinkRGBnTimes(Color c, int count);
void crossFadeHSI(int h1, float s1, float i1, int h2, float s2, float i2, long n, int d);
void crossFade(Color c1, Color c2, long n, int d);
void crossFade(Color c1, Color c2, float msec);
void crossFadeTo(Color c1, float msec);
void waitForButton(unsigned long i);
long calcTOD(int hr, int min, int sec);
Color adjustSat(Color c, float newSat);
Color adjustInt(Color c, float newInt);
bool doModeOnce();
bool doAlarmOnce();
int loopCount(int n);


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

int loopCount(int mod){
    if(count<0){
        countMode=0;
    } else if(count>mod){
        countMode=1;
    }
    if (countMode == 0){
        count = (count+1);
    } else if(countMode == 1){
        count = (count-1);
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

//Do not add code below this line
#endif /* _ArudinoRgbController_H_ */
