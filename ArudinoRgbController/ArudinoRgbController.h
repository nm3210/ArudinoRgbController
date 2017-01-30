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
volatile unsigned long interruptCurTime = 0;
volatile unsigned long interruptLastTime = 0;
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
int wrapCount(int mod);
int loopCount(int n);

//Do not add code below this line
#endif /* _ArudinoRgbController_H_ */
