// Do not remove the include below
#include "EepromTesting.h"

const uint8_t  numSegments = 3;
SegmentData segmentData[numSegments];

void setup() {
    // Turn on serial port
    Serial.begin(9600);
    while(!Serial){}; // Wait for serial to connect
    Serial.println("Starting Arduino...");

    // Initialize segment data
    for(uint8_t i=0; i < numSegments; i++){
        segmentData[i].initialize();

        // Fill in the data with some data to test reading
        segmentData[i].brightness =+ i;                          // 1,2,3
        segmentData[i].saturation = segmentData[i].saturation-i; // 15,14,13
        segmentData[i].curMode    = static_cast<LightMode>(i+1); // 1,2,3
        segmentData[i].prevMode   = static_cast<LightMode>(i+5); // 5,6,7
    }


    int eeAddress = 0;
    EEPROM.put(eeAddress, segmentData);
    Serial.println("Wrote SegmentData type!");
}

void loop() {
    int eeAddress = 0;
    EEPROM.get(eeAddress, segmentData);

    Serial.print("Read SegmentData [sizeof(SegmentData)==");
    Serial.print(sizeof(SegmentData));
    Serial.print("bytes, sizeof(segmentData)==");
    Serial.print(sizeof(segmentData));
    Serial.println("bytes]: ");

    for(uint8_t i=0; i < numSegments; i++){
        Serial.print("    item #");
        Serial.print(i);
        Serial.print(": brightness=");
        Serial.print(segmentData[i].brightness);
        Serial.print(", saturation=");
        Serial.print(segmentData[i].saturation);
        Serial.print(", curMode=");
        Serial.print(segmentData[i].curMode);
        Serial.print(", prevMode=");
        Serial.println(segmentData[i].prevMode);
    }
    delay(1000);
}
