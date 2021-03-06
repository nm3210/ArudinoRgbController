#ifndef _RgbControllerWithIR_H_
#define _RgbControllerWithIR_H_
#include "Arduino.h"
#include "TimeLib.h"
#include "TimeAlarms.h"
#include "DS1307RTC.h"
//#include "IRremote.h"
#include "Timezone.h"
#include "Dusk2Dawn.h"

//// Define buttons
//// I'm having a very hard time finding a good sweet spot for the IR
//// so I've just put all the values I'm reading from the sensor, even
//// if it's for the same button. I imagine this is some bad PWM
//// frequency matching, but this solution is easier...
//// 2017-01-23 update: most of the frustrations have gone away by
//// changing the tolerance to 40%
//#define BTN_REPEAT           0xFFFFFFFF
//
//// 17 Key NumPad + D-Pad Remote
//#define CTRL1BTN_1           0xFFA25D // E318261B and FFA25D
//#define CTRL1BTN_2           0xFF629D // 511DBB   and FF629D
//#define CTRL1BTN_3           0xFFE21D // EE886D7F and FFE21D
//#define CTRL1BTN_4           0xFF22DD // 52A3D41F and FF22DD
//#define CTRL1BTN_5           0xFF02FD // D7E84B1B and FF02FD
//#define CTRL1BTN_6           0xFFC23D // 20FE4DBB and FFC23D
//#define CTRL1BTN_7           0xFFE01F // F076C13B and FFE01F
//#define CTRL1BTN_8           0xFFA857 // A3C8EDDB and FFA857
//#define CTRL1BTN_9           0xFF906F // E5CFBD7F and FF906F
//#define CTRL1BTN_0           0xFF9867 // 97483BFB and FF9867
//#define CTRL1BTN_ASTRX       0xFF6897 // C101E57B and FF6897
//#define CTRL1BTN_POUND       0xFFB04F // F0C41643 and FFB04F
//#define CTRL1BTN_SELECT      0xFF38C7 // 488F3CBB and FF38C7
//#define CTRL1BTN_UP          0xFF18E7 // 3D9AE3F7 and FF18E7
//#define CTRL1BTN_DOWN        0xFF4AB5 // 1BC0157B and FF4AB5
//#define CTRL1BTN_LEFT        0xFF10EF // 8C22657B and FF10EF
//#define CTRL1BTN_RIGHT       0xFF5AA5 // 449E79F  and FF5AA5
//
//// 22 Key RGBW Remote
//#define CTRL2BTN_BRIGHTUP    0xF700FF // 8503705D and F700FF
//#define CTRL2BTN_BRIGHTDOWN  0xF7807F // DEB0C861 and F7807F
//#define CTRL2BTN_OFF         0xF740BF // D4DD0381 and F740BF
//#define CTRL2BTN_ON          0xF7C03F // CE1972FD and F7C03F
//#define CTRL2BTN_RED         0xF720DF // E85952E1 and F720DF
//#define CTRL2BTN_GREEN       0xF7A05F // 78CDA4DD and F7A05F
//#define CTRL2BTN_BLUE        0xF7609F // A2672345 and F7609F
//#define CTRL2BTN_WHITE       0xF7E01F // 9BA392C1 and F7E01F
//#define CTRL2BTN_ROW3_1      0xF710EF // D3FD9A81 and F710EF
//#define CTRL2BTN_ROW3_2      0xF7906F // 6471EC7D and F7906F
//#define CTRL2BTN_ROW3_3      0xF750AF // 9D52009D and F750AF
//#define CTRL2BTN_ROW4_1      0xF730CF // 84044BBD and F730CF
//#define CTRL2BTN_ROW4_2      0xF7B04F // 14789DB9 and F7B04F
//#define CTRL2BTN_ROW4_3      0xF7708F // 3E121C21 and F7708F
//#define CTRL2BTN_ROW5_1      0xF708F7 // B0F9B3E1 and F708F7
//#define CTRL2BTN_ROW5_2      0xF78877 // 416E05DD and F78877
//#define CTRL2BTN_ROW5_3      0xF748B7 // 6A844445 and F748B7
//#define CTRL2BTN_ROW6_1      0xF728D7 // 9DE75E1D and F728D7
//#define CTRL2BTN_ROW6_2      0xF7A857 // F794B621 and F7A857
//#define CTRL2BTN_ROW6_3      0xF76897 // 57F52E81 and F76897
//#define CTRL2BTN_FLASH       0xF7D02F // DCC45BE1 and F7D02F
//#define CTRL2BTN_STROBE      0xF7F00F // 374E8B9D and F7F00F
//#define CTRL2BTN_FADE        0xF7C837 // B9C07541 and F7C837
//#define CTRL2BTN_SMOOTH      0xF7E817 // A7315F7D and F7E817
//
//// 44 Key RGBW Remote
//#define CTRL3BTN_BRIGHTUP    0xFF3AC5
//#define CTRL3BTN_BRIGHTDOWN  0xFFBA45
//#define CTRL3BTN_ROW01_3     0xFF827D
//#define CTRL3BTN_ONOFF       0xFF02FD
//#define CTRL3BTN_RED         0xFF1AE5
//#define CTRL3BTN_GREEN       0xFF9A65
//#define CTRL3BTN_BLUE        0xFFA25D
//#define CTRL3BTN_WHITE       0xFF22DD
//#define CTRL3BTN_ROW03_1     0xFF2AD5
//#define CTRL3BTN_ROW03_2     0xFFAA55
//#define CTRL3BTN_ROW03_3     0xFF926D
//#define CTRL3BTN_ROW03_4     0xFF12ED
//#define CTRL3BTN_ROW04_1     0xFF0AF5
//#define CTRL3BTN_ROW04_2     0xFF8A75
//#define CTRL3BTN_ROW04_3     0xFFB24D
//#define CTRL3BTN_ROW04_4     0xFF32CD
//#define CTRL3BTN_ROW05_1     0xFF38C7
//#define CTRL3BTN_ROW05_2     0xFFB847
//#define CTRL3BTN_ROW05_3     0xFF7887
//#define CTRL3BTN_ROW05_4     0xFFF807
//#define CTRL3BTN_ROW06_1     0xFF18E7
//#define CTRL3BTN_ROW06_2     0xFF9867
//#define CTRL3BTN_ROW06_3     0xFF58A7
//#define CTRL3BTN_ROW06_4     0xFFD827
//#define CTRL3BTN_ROW07_1     0xFF28D7
//#define CTRL3BTN_ROW07_2     0xFFA857
//#define CTRL3BTN_ROW07_3     0xFF6897
//#define CTRL3BTN_ROW07_4     0xFFE817
//#define CTRL3BTN_ROW08_1     0xFF08F7
//#define CTRL3BTN_ROW08_2     0xFF8877
//#define CTRL3BTN_ROW08_3     0xFF48B7
//#define CTRL3BTN_ROW08_4     0xFFC837
//#define CTRL3BTN_ROW09_1     0xFF30CF
//#define CTRL3BTN_ROW09_2     0xFFB04F
//#define CTRL3BTN_ROW09_3     0xFF708F
//#define CTRL3BTN_ROW09_4     0xFFF00F
//#define CTRL3BTN_ROW10_1     0xFF10EF
//#define CTRL3BTN_ROW10_2     0xFF906F
//#define CTRL3BTN_ROW10_3     0xFF50AF
//#define CTRL3BTN_ROW10_4     0xFFD02F
//#define CTRL3BTN_ROW11_1     0xFF20DF
//#define CTRL3BTN_ROW11_2     0xFFA05F
//#define CTRL3BTN_ROW11_3     0xFF609F
//#define CTRL3BTN_ROW11_4     0xFFE01F
//
//// Verizon FiOS Remote
//#define CTRL4BTN_ONOFF1      0x20DF10EF
//#define CTRL4BTN_ONOFF2      0x736ED82E
//#define CTRL4BTN_VOLUP       0x20DF40BF
//#define CTRL4BTN_VOLDOWN     0x20DFC03F
//#define CTRL4BTN_CHANNELUP   0x8A67822
//#define CTRL4BTN_CHANNELDOWN 0xFBACE420
//#define CTRL4BTN_PLAY        0x313599EC
//#define CTRL4BTN_PAUSE       0xD922D768

//// LightMode enum
//enum LightMode{
//    MODE_OFF, MANUAL_COLOR,
//    SOLID_RED, SOLID_GREEN, SOLID_BLUE, SOLID_WHITE, SOLID_CYAN, SOLID_YELLOW, SOLID_MAGENTA,
//    SOLID_COLOR01, SOLID_COLOR02, SOLID_COLOR03, SOLID_COLOR04,
//    SOLID_COLOR05, SOLID_COLOR06, SOLID_COLOR07, SOLID_COLOR08,
//    SOLID_COLOR09, SOLID_COLOR10, SOLID_COLOR11, SOLID_COLOR12,
//    SOLID_COLOR13, SOLID_COLOR14, SOLID_COLOR15, SOLID_COLOR16,
//    MODE1, MODE2, MODE3, MODE4, MODE5, MODE6
//};
//
//#define numBrightLevels 16
//uint8_t brightnessLevels[numBrightLevels] = {
//    0, 1, 2, 3, 4, 6, 9, 14, 19, 28, 40, 58, 84, 122, 176, 255 // 16
//};

// Constant variables
const int offMode = 0;
const int alarmMode = 1; // This is the state number of when the alarms will go off (see if-statements in loop())
const int dynamicMode = 2;
const int NUMALARMS = 12;
const int NUMSUNALARMS = 12;
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;  // milliseconds
const uint16_t ALARMCHECK_TIMEOUT = 5000;    // milliseconds
const long SUNRISECHECK_TIMEOUT = 86400000L; // milliseconds; milliseconds per day (reset astronomical alarms once per day)
const float M_1PI3 = 1.0 * M_PI / 3.0;
const float M_2PI3 = 2.0 * M_PI / 3.0;
const float M_4PI3 = 4.0 * M_PI / 3.0;
//const signed int offsetSecPerYear = 4352; // Time offset for DS1307 chip

// RAM-specific variables to keep track while handling the interrupts
volatile unsigned long interruptCurTime = 0;
volatile unsigned long interruptLastTime = 0;
volatile bool changeMode = false;

// Button/mode variables
uint32_t lastButtonPressed = 0;
uint32_t lastButtonSave = 0;
//LightMode prevColorMode = MODE_OFF;
//LightMode  curColorMode = MODE_OFF;
int curMode = offMode;
int curAlarm = 0;
int curSunAlarm = 0;
int countMode = 0;
bool doModeOnceFlag = true;
bool doAlarmOnceFlag = true;
bool doSunAlarm = true;
bool isManualOn = false;

//uint8_t adjustBrightnessVal = 1;
//uint8_t adjustSaturationVal = numBrightLevels-1;
//uint8_t manualRedIndex = 0;
//uint8_t manualBlueIndex = 0;
//uint8_t manualGreenIndex = 0;
//uint8_t manualWhiteIndex = 0;

// Global HSI variables
uint16_t hsi_Hue = 0;
float hsi_Saturation = 1.0;
float hsi_Intensity = 1.0;

// Miscellaneous
long timeAlarms[NUMALARMS];
long sunAlarms[NUMSUNALARMS]; // apparent, civil, nautical, and astronomical
long displayClockTime = 0;
long alarmCheckTime = 0;
long sunriseUpdateTime = 0;
uint16_t count = 0;
int curSunrise = 0;
int curSunset = 0;

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
static const Color PINK    (277, 0.90, 1.00);

static const Color WHITE   (  0, 0.00, 1.00);
static const Color DIM     (  0, 0.00, 0.10);
static const Color BLACK   (  0, 0.00, 0.00);
static const Color ON = WHITE;
static const Color OFF = BLACK;
static const Color PURPLE = VIOLET;

Color PREV (0,0,0); // create the 'previous' color variable
Color controlColor = RED; // reference the current manual color (via ir)
int MANUAL[3] = {0,0,0};

// Function declarations
//bool handleInterrupt();
//bool checkBrightnessChange();
//bool checkModesSpecial();
//bool checkModes();
//bool checkRepeatBtn(uint32_t tempButtonPress);
//void checkForManualColor();
//void changeColorMode(LightMode newMode);
//long decodeIrSignal();
//void rtcCorrection();
void digitalClockDisplay();
void digitalClockDisplay(time_t curTime);
void print2digits(int digits);
void hsi2rgb(Color c, int* rgb);
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
int findClosestIndex(uint8_t values[], uint8_t size, uint8_t find);
long calcTOD(int hr, int min, int sec);
Color adjustSat(Color c, float newSat);
Color adjustInt(Color c, float newInt);
bool doModeOnce();
bool doAlarmOnce();
int wrapCount(uint16_t mod);
int loopCount(uint16_t n);

#endif /* _RgbControllerWithIR_H_ */
