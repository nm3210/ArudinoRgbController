// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _HsiColor_H_
#define _HsiColor_H_
#include "Arduino.h"
//add your includes for the project Testing01 here

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project Testing01 here


byte ledPinR = 9;
byte ledPinG = 10;
byte ledPinB = 11;

// RAM-specific variables to keep track within the interrupts
volatile unsigned long buttonCurTime = 0;
volatile unsigned long buttonLastTime = 0;
volatile float hsi_Hue = 0;
volatile float hsi_Saturation = 0.75;
volatile float hsi_Intensity = 0.1;
volatile boolean changeMode = false;
volatile int curMode = 0;
unsigned int count = 0;
const int alarmState = 0;

int rgb[3];
int redVal = 0;
int grnVal = 0;
int bluVal = 0;
int prevR = 0;
int prevG = 0;
int prevB = 0;

static const float expConvert = 0.0217457938712615;
static const float M_1PI3 = 1.0 * M_PI / 3.0;
static const float M_2PI3 = 2.0 * M_PI / 3.0;
static const float M_4PI3 = 4.0 * M_PI / 3.0;

void writeRGB(int rgb[]);
void writeRGB(int r, int g, int b);
int convertColor(int c);
void hsi2rgb(float H, float S, float I, int* rgb);
int *hsi2rgb(float H, float S, float I);

struct Color {
    float hue, sat, intensity;
    uint8_t red, green, blue;

    Color(float h, float s, float i){
    	int rgbTemp[3];
    	hue = h; sat = s; intensity = i;
    	hsi2rgb(hue, sat, intensity, rgbTemp);
    	red = rgbTemp[0];
    	green = rgbTemp[1];
    	blue = rgbTemp[2];
    }
};

// These colors are in HSI
static const Color RED     ((float)   0, (float) 1, (float) 1);
static const Color GREEN   ((float) 120, (float) 1, (float) 1);
static const Color BLUE    ((float) 240, (float) 1, (float) 1);

static const Color MAGENTA ((float) 300, (float) 1, (float) 1);
static const Color YELLOW  ((float)  57, (float) 1, (float) 1);
static const Color CYAN    ((float) 180, (float) 1, (float) 1);

static const Color ORANGE  ((float)   3, (float) 1, (float) 0.33);
static const Color INDIGO  ((float) 252, (float) 1, (float) 0.20);
static const Color VIOLET  ((float) 252, (float) 1, (float) 0.40);
static const Color PINK    ((float) 277, (float) 0.9,(float) 1);

static const Color WHITE   ((float)   0, (float) 0, (float) 1);
static const Color DIM     ((float)   0, (float) 0, (float) 0.5);
static const Color BLACK   ((float)   0, (float) 0, (float) 0);
static const Color ON = WHITE;
static const Color OFF = BLACK;
static const Color PURPLE = VIOLET;


void writeRGB(Color c);
void blinkRGB(Color c);

//Do not add code below this line
#endif /* _HsiColor_H_ */
