// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _EepromTesting_H_
#define _EepromTesting_H_
#include "Arduino.h"
#include "EEPROM.h"

#define numBrightLevels 16

// LightMode enum
enum LightMode {
    MODE_OFF, MANUAL_COLOR,
    SOLID_RED, SOLID_GREEN, SOLID_BLUE, SOLID_WHITE, SOLID_CYAN, SOLID_YELLOW, SOLID_MAGENTA,
    SOLID_COLOR01, SOLID_COLOR02, SOLID_COLOR03, SOLID_COLOR04,
    SOLID_COLOR05, SOLID_COLOR06, SOLID_COLOR07, SOLID_COLOR08,
    SOLID_COLOR09, SOLID_COLOR10, SOLID_COLOR11, SOLID_COLOR12,
    SOLID_COLOR13, SOLID_COLOR14, SOLID_COLOR15, SOLID_COLOR16,
    MODE1, MODE2, MODE3, MODE4, MODE5, MODE6
};

// Set up segment data structure
struct SegmentData {
    uint8_t brightness;
    uint8_t saturation;
    LightMode curMode;
    LightMode prevMode;

    void initialize(){
        brightness = 1;
        saturation = numBrightLevels-1;
        curMode = MODE_OFF;
        prevMode = MODE_OFF;
    }
};

#endif /* _EepromTesting_H_ */
