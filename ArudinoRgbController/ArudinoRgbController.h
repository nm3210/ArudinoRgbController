// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _ArudinoRgbController_H_
#define _ArudinoRgbController_H_
#include "Arduino.h"
//add your includes for the project Testing01 here
#include "Time.h"
#include "TimeAlarms.h"
#include "DS1307RTC.h"

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
int crossFadeState;
int wait = 10;      // 10ms internal crossFade delay; increase for slower fades
int hold = 0; // Optional hold when a color is complete, before the next crossFade

// RTC Correction Values
//const signed int secPerYear = 4368;
const signed int offsetSecPerYear = 5012;
AlarmID_t clockCorrect;

// RAM-specific variables to keep track within the interrupts
volatile unsigned long buttonCurTime = 0;
volatile unsigned long buttonLastTime = 0;
volatile int count = 0;
volatile int switchState = 0;
volatile boolean changeState = false;

int redVal = 0;
int grnVal = 0;
int bluVal = 0;
int prevR = 0;
int prevG = 0;
int prevB = 0;

const double expConvert = 0.0217457938712615;

struct Color {
    const uint8_t red, green, blue;
    Color(uint8_t r, uint8_t g, uint8_t b):
            red(r), green(g), blue(b) {
    }
};

static const Color RED     (255,   0,   0);
static const Color GREEN   (  0, 255,   0);
static const Color BLUE    (  0,   0, 255);

static const Color MAGENTA (255,   0, 255);
static const Color YELLOW  (255, 150,   0);
static const Color CYAN    (  0, 255, 255);

static const Color ORANGE  ( 83,   4, 000);
static const Color INDIGO  ( 10,   0,  50);
static const Color VIOLET  ( 20,   0, 100);
static const Color PINK    (158,   4,  79);

static const Color WHITE   (255, 255, 255);
static const Color DIM     ( 20,  20,  20);
static const Color BLACK   (  0,   0,   0);
static const Color ON = WHITE;
static const Color OFF = BLACK;
static const Color PURPLE = VIOLET;

void alarm01();
void alarm02();
void alarm03();
void rtcCorrection();
void digitalClockDisplay();
void print2digits(int digits);
void btnInterrupt();
void crossFadeRGB(int g);
void crossFadeRGB2(int g);
void writeRGB(Color c);
void writeRGB(Color c, int d);
void writeRGB(int r, int g, int b);
void writeRGB(int r, int g, int b, int d);
void crossFade(Color c);
void crossFade(Color c, int w);
void crossFade(Color c, int w, int d);
void blinkRGB(Color c);
void blinkRGB(Color c, int count);
int convertColor(int c);

void writeRGB(Color c){
    writeRGB(c.red,c.green,c.blue);
}
void writeRGB(int r, int g, int b, int d) {
    writeRGB(r, g, b);
    Alarm.delay(d);
}
void writeRGB(Color c, int d) {
    writeRGB(c);
    Alarm.delay(d);
}

void crossFade(Color c, int w) {
    int oldwait = wait;
    wait = w;
    crossFade(c);
    wait = oldwait;
}
void crossFade(Color c, int w, int d) {
    int oldhold = hold;
    hold = d;
    crossFade(c, w);
    hold = oldhold;
}
void blinkRGB(Color c, int count){
    for(int i=0;i<count;i++){
        blinkRGB(c);
    }
}
int convertColor(int c){
	return ceil((double) exp(c * expConvert) - 1);
}
//Do not add code below this line
#endif /* _ArudinoRgbController_H_ */
