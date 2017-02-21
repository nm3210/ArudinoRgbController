// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _IrRemoteTesting_H_
#define _IrRemoteTesting_H_
#include "Arduino.h"
//add your includes for the project IrRemoteTesting here
#include "IRremote.h"
#include "Adafruit_NeoPixel.h"
#include "Time.h"

// Define buttons
// I'm having a very hard time finding a good sweet spot for the IR
// so I've just put all the values I'm reading from the sensor, even
// if it's for the same button. I imagine this is some bad PWM
// frequency matching, but this solution is easier...
#define CTRL1BTN_1           0xFFA25D // E318261B and FFA25D
#define CTRL1BTN_2           0xFF629D // 511DBB   and FF629D
#define CTRL1BTN_3           0xFFE21D // EE886D7F and FFE21D
#define CTRL1BTN_4           0xFF22DD // 52A3D41F and FF22DD
#define CTRL1BTN_5           0xFF02FD // D7E84B1B and FF02FD
#define CTRL1BTN_6           0xFFC23D // 20FE4DBB and FFC23D
#define CTRL1BTN_7           0xFFE01F // F076C13B and FFE01F
#define CTRL1BTN_8           0xFFA857 // A3C8EDDB and FFA857
#define CTRL1BTN_9           0xFF906F // E5CFBD7F and FF906F
#define CTRL1BTN_0           0xFF9867 // 97483BFB and FF9867
#define CTRL1BTN_ASTRX       0xFF6897 // C101E57B and FF6897
#define CTRL1BTN_POUND       0xFFB04F // F0C41643 and FFB04F
#define CTRL1BTN_SELECT      0xFF38C7 // 488F3CBB and FF38C7
#define CTRL1BTN_UP          0xFF18E7 // 3D9AE3F7 and FF18E7
#define CTRL1BTN_DOWN        0xFF4AB5 // 1BC0157B and FF4AB5
#define CTRL1BTN_LEFT        0xFF10EF // 8C22657B and FF10EF
#define CTRL1BTN_RIGHT       0xFF5AA5 // 449E79F  and FF5AA5

#define CTRL1BTN2_1          0xE318261B // E318261B and FFA25D
#define CTRL1BTN2_2          0x511DBB   // 511DBB   and FF629D
#define CTRL1BTN2_3          0xEE886D7F // EE886D7F and FFE21D
#define CTRL1BTN2_4          0x52A3D41F // 52A3D41F and FF22DD
#define CTRL1BTN2_5          0xD7E84B1B // D7E84B1B and FF02FD
#define CTRL1BTN2_6          0x20FE4DBB // 20FE4DBB and FFC23D
#define CTRL1BTN2_7          0xF076C13B // F076C13B and FFE01F
#define CTRL1BTN2_8          0xA3C8EDDB // A3C8EDDB and FFA857
#define CTRL1BTN2_9          0xE5CFBD7F // E5CFBD7F and FF906F
#define CTRL1BTN2_0          0x97483BFB // 97483BFB and FF9867
#define CTRL1BTN2_ASTRX      0xC101E57B // C101E57B and FF6897
#define CTRL1BTN2_POUND      0xF0C41643 // F0C41643 and FFB04F
#define CTRL1BTN2_SELECT     0x488F3CBB // 488F3CBB and FF38C7
#define CTRL1BTN2_UP         0x3D9AE3F7 // 3D9AE3F7 and FF18E7
#define CTRL1BTN2_DOWN       0x1BC0157B // 1BC0157B and FF4AB5
#define CTRL1BTN2_LEFT       0x8C22657B // 8C22657B and FF10EF
#define CTRL1BTN2_RIGHT      0x449E79F  // 449E79F  and FF5AA5

#define CTRL2BTN_BRIGHTUP    0xF700FF // 8503705D and F700FF
#define CTRL2BTN_BRIGHTDOWN  0xF7807F // DEB0C861 and F7807F
#define CTRL2BTN_OFF         0xF740BF // D4DD0381 and F740BF
#define CTRL2BTN_ON          0xF7C03F // CE1972FD and F7C03F
#define CTRL2BTN_RED         0xF720DF // E85952E1 and F720DF
#define CTRL2BTN_GREEN       0xF7A05F // 78CDA4DD and F7A05F
#define CTRL2BTN_BLUE        0xF7609F // A2672345 and F7609F
#define CTRL2BTN_WHITE       0xF7E01F // 9BA392C1 and F7E01F
#define CTRL2BTN_ROW3_1      0xF710EF // D3FD9A81 and F710EF
#define CTRL2BTN_ROW3_2      0xF7906F // 6471EC7D and F7906F
#define CTRL2BTN_ROW3_3      0xF750AF // 9D52009D and F750AF
#define CTRL2BTN_ROW4_1      0xF730CF // 84044BBD and F730CF
#define CTRL2BTN_ROW4_2      0xF7B04F // 14789DB9 and F7B04F
#define CTRL2BTN_ROW4_3      0xF7708F // 3E121C21 and F7708F
#define CTRL2BTN_ROW5_1      0xF708F7 // B0F9B3E1 and F708F7
#define CTRL2BTN_ROW5_2      0xF78877 // 416E05DD and F78877
#define CTRL2BTN_ROW5_3      0xF748B7 // 6A844445 and F748B7
#define CTRL2BTN_ROW6_1      0xF728D7 // 9DE75E1D and F728D7
#define CTRL2BTN_ROW6_2      0xF7A857 // F794B621 and F7A857
#define CTRL2BTN_ROW6_3      0xF76897 // 57F52E81 and F76897
#define CTRL2BTN_FLASH       0xF7D02F // DCC45BE1 and F7D02F
#define CTRL2BTN_STROBE      0xF7F00F // 374E8B9D and F7F00F
#define CTRL2BTN_FADE        0xF7C837 // B9C07541 and F7C837
#define CTRL2BTN_SMOOTH      0xF7E817 // A7315F7D and F7E817

#define CTRL2BTN2_BRIGHTUP   0x8503705D // 8503705D and F700FF
#define CTRL2BTN2_BRIGHTDOWN 0xDEB0C861 // DEB0C861 and F7807F
#define CTRL2BTN2_OFF        0xD4DD0381 // D4DD0381 and F740BF
#define CTRL2BTN2_ON         0xCE1972FD // CE1972FD and F7C03F
#define CTRL2BTN2_RED        0xE85952E1 // E85952E1 and F720DF
#define CTRL2BTN2_GREEN      0x78CDA4DD // 78CDA4DD and F7A05F
#define CTRL2BTN2_BLUE       0xA2672345 // A2672345 and F7609F
#define CTRL2BTN2_WHITE      0x9BA392C1 // 9BA392C1 and F7E01F
#define CTRL2BTN2_ROW2_1     0xD3FD9A81 // D3FD9A81 and F710EF
#define CTRL2BTN2_ROW2_2     0x6471EC7D // 6471EC7D and F7906F
#define CTRL2BTN2_ROW2_3     0x9D52009D // 9D52009D and F750AF
#define CTRL2BTN2_ROW3_1     0x84044BBD // 84044BBD and F730CF
#define CTRL2BTN2_ROW3_2     0x14789DB9 // 14789DB9 and F7B04F
#define CTRL2BTN2_ROW3_3     0x3E121C21 // 3E121C21 and F7708F
#define CTRL2BTN2_ROW4_1     0xB0F9B3E1 // B0F9B3E1 and F708F7
#define CTRL2BTN2_ROW4_2     0x416E05DD // 416E05DD and F78877
#define CTRL2BTN2_ROW4_3     0x6A844445 // 6A844445 and F748B7
#define CTRL2BTN2_ROW5_1     0x9DE75E1D // 9DE75E1D and F728D7
#define CTRL2BTN2_ROW5_2     0xF794B621 // F794B621 and F7A857
#define CTRL2BTN2_ROW5_3     0x57F52E81 // 57F52E81 and F76897
#define CTRL2BTN2_FLASH      0xDCC45BE1 // DCC45BE1 and F7D02F
#define CTRL2BTN2_STROBE     0x374E8B9D // 374E8B9D and F7F00F
#define CTRL2BTN2_FADE       0xB9C07541 // B9C07541 and F7C837
#define CTRL2BTN2_SMOOTH     0xA7315F7D // A7315F7D and F7E817

#define CTRL3BTN_ONOFF1      0x20DF10EF
#define CTRL3BTN_ONOFF2      0x736ED82E
#define CTRL3BTN_VOLUP       0x20DF40BF
#define CTRL3BTN_VOLDOWN     0x20DFC03F
#define CTRL3BTN_CHANNELUP   0x8A67822
#define CTRL3BTN_CHANNELDOWN 0xFBACE420
#define CTRL3BTN_PLAY        0x313599EC
#define CTRL3BTN_PAUSE       0xD922D768

// LightMode enum
enum LightColor{
    MODE_OFF,
    SOLID_RED, SOLID_GREEN, SOLID_BLUE, SOLID_WHITE,
    SOLID_CYAN, SOLID_YELLOW, SOLID_MAGENTA,
    SOLID_COLOR1, SOLID_COLOR2, SOLID_COLOR3,
    SOLID_COLOR4, SOLID_COLOR5, SOLID_COLOR6,
    SOLID_COLOR7, SOLID_COLOR8, SOLID_COLOR9,
    MODE1, MODE2, MODE3, MODE4, MODE5, MODE6
};

#define numBrightLevels 10
uint8_t brightnessLevels[numBrightLevels] = {
//    1, 2, 5, 11, 24, 52, 116, 255 // 8
    1, 2, 3, 6, 12, 22, 40, 75, 139, 255 // 10
//    1, 2, 3, 4, 5, 6, 9, 14, 19, 28, 40, 58, 84, 122, 176, 255 // 16
};

// Constant variables
const uint16_t CLOCKDISPLAY_TIMEOUT = 1000;
const float M_1PI3 = 1.0 * M_PI / 3.0;
const float M_2PI3 = 2.0 * M_PI / 3.0;
const float M_4PI3 = 4.0 * M_PI / 3.0;

// Button/mode variables
uint32_t lastButtonPressed = 0;
LightColor prevMode = MODE_OFF;
LightColor  curMode = MODE_OFF;
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
        hash = (hue+1)*(red+1)*(green+1)*(blue+1)*(white+1)*(sat+1)*(intensity+1);
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
static const Color PINK    (  0, 0.50, 1.00);

static const Color WHITE   (  0, 0.00, 1.00);
static const Color DIM     (  0, 0.00, 0.10);
static const Color BLACK   (  0, 0.00, 0.00);
static const Color ON = WHITE;
static const Color OFF = BLACK;
static const Color PURPLE = VIOLET;
Color PREV (0,0,0); // create the 'previous' color variable

// Function declarations
bool handleInterrupt();
bool checkBrightnessChange();
bool checkModesSpecial();
bool checkModes();
void changeMode(LightColor newMode);
long decodeIrSignal();
void digitalClockDisplay();
void print2digits(int digits);
bool doModeOnce();
void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
Color adjustSat(Color c, float newSat);
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
#endif /* _IrRemoteTesting_H_ */
