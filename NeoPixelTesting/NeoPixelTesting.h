// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _NeoPixelTesting_H_
#define _NeoPixelTesting_H_
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "Time.h"

// Constant variables
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;
const float M_1PI3 = 1.0 * M_PI / 3.0;
const float M_2PI3 = 2.0 * M_PI / 3.0;
const float M_4PI3 = 4.0 * M_PI / 3.0;

// RAM-specific variables to keep track while handling the interrupts
volatile unsigned long interruptCurTime = 0;
volatile unsigned long interruptLastTime = 0;
volatile bool changeMode = false;

// Button/mode variables
int curMode = 0;
int countMode = 0;
bool doModeOnceFlag = true;

// Miscellaneous
long displayClockTime = 0;
long alarmCheckTime = 0;
uint16_t count = 0;

// Initialize the main hsi2rgb/w functions
void hsi2rgbw(int H, float S, float I, int* rgbw);

// Create the color structure
struct Color {
public:
    float sat, intensity;            // ranges from [0.0 to 1.0]
    uint16_t hue;                    // ranges from [0 to 360]
    uint8_t red, green, blue, white; // ranges from [0 to 255]

    Color(int h, float s, float i){
        int rgbwTemp[4];
        hue = h; sat = s; intensity = i;
        // Convert HSI to RGBW
        hsi2rgbw(hue, sat, intensity, rgbwTemp);
        red   = rgbwTemp[0];
        green = rgbwTemp[1];
        blue  = rgbwTemp[2];
        white = rgbwTemp[3];
        // Store hash
        hash = (hue+1)*(red+1)*(green+1)*(blue+1)*(white+1);
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
static const Color VIOLET  (250, 1.00, 1.00);
static const Color PINK    (277, 0.90, 1.00);

static const Color WHITE   (  0, 0.00, 1.00);
static const Color DIM     (  0, 0.00, 0.10);
static const Color BLACK   (  0, 0.00, 0.00);
static const Color ON = WHITE;
static const Color OFF = BLACK;
static const Color PURPLE = VIOLET;
Color PREV (0,0,0); // create the 'previous' color variable

// Function declarations
void digitalClockDisplay();
void print2digits(int digits);
bool doModeOnce();
void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
Color adjustInt(Color c, float newInt);
void writeHSI(int h, float s, float i);
void writeRGBW(int rgbw[]);
void writeRGBW(Color c);
void blinkRGBW(Color c);
void blinkRGBW(Color c, int d);
void blinkRGBWnTimes(Color c, int count);
void crossFadeHSI(int h1, float s1, float i1, int h2, float s2, float i2, long n, int d);
void crossFade(Color c1, Color c2, long n, int d);
void crossFade(Color c1, Color c2, float msec);
void crossFadeTo(Color c1, float msec);
void waitForButton(unsigned long i);
int wrapCount(uint16_t mod);
int loopCount(uint16_t n);

//Do not add code below this line
#endif /* _NeoPixelTesting_H_ */
