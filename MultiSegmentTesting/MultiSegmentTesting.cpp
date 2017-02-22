#include "MultiSegmentTesting.h"

#define IR_PIN   2
#define LED_PIN  17

const uint16_t numLeds = 8;
const uint8_t  numSegments = 4;
const uint16_t segments[numSegments+1] = {0, 2, 4, 6, 8};
uint8_t  curSeg = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, LED_PIN, NEO_GRBW + NEO_KHZ800);

void setup() {
    // Turn on serial port
    Serial.begin(9600);
    Serial.println("Starting Arduino...");

    // Start pixel strip
    strip.begin();
    strip.show();
}

void loop() {
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
        switch(curSeg){
        case 0:
        	writeRGBW(0,0,0,0);
        	writeRGBW(5,5,0,0,curSeg);
        	break;
        case 1:
        	writeRGBW(0,0,0,0);
        	writeRGBW(0,5,5,0,curSeg);
        	break;
        case 2:
        	writeRGBW(0,0,0,0);
        	writeRGBW(5,0,5,0,curSeg);
        	break;
        case 3:
        	writeRGBW(0,0,0,0);
        	writeRGBW(0,0,0,5,curSeg);
        	break;
        case 4:
        	writeRGBW(0,0,0,0);
        	writeRGBW(5,5,5,0,0);
        	writeRGBW(0,0,5,0,1);
        	writeRGBW(0,5,0,0,2);
        	writeRGBW(5,0,0,0,3);
//        	break;
//        default:
        	curSeg = -1;
        	break;
        }
        curSeg++;
    }
    delay(0);
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t start, uint16_t stop) {
    for (uint16_t i = start; i < stop; i++) {
        strip.setPixelColor(i, strip.Color(r, g, b, w));
    }
    strip.show();
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    for (uint16_t i = 0; i < numLeds; i++) {
        strip.setPixelColor(i, strip.Color(r, g, b, w));
    }
    strip.show();
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t seg){
	if(seg>=numSegments) {return;}
	uint16_t start = segments[seg];
	uint16_t stop  = segments[seg+1];
	writeRGBW(r,g,b,w,start,stop);
}
