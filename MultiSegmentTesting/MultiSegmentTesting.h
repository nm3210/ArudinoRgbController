#ifndef _MultiSegmentTesting_H_
#define _MultiSegmentTesting_H_
#include "Arduino.h"

// Includes
#include "IRremote.h"
#include "Adafruit_NeoPixel.h"

// Variables
const uint16_t CLOCKDISPLAY_TIMEOUT = 250;
long displayClockTime = 0;

// Functions
void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t start, uint16_t stop);
void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t seg);

#endif /* _MultiSegmentTesting_H_ */
