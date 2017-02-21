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
void writeRGB(uint8_t r, uint8_t g, uint8_t b);
Color adjustInt(Color c, float newInt);
void writeHSI(int h, float s, float i);
void writeRGB(int rgb[]);
void writeRGB(Color c);
void blinkRGB(Color c);
void blinkRGB(Color c, int d);
void blinkRGBnTimes(Color c, int count);

//Do not add code below this line
#endif /* _NeoPixelTesting_H_ */
