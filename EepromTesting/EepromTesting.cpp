#include "EepromTesting.h"

const int eeAddress = 0;
const uint8_t numSegments = 6;
SegmentData segmentData[numSegments];
uint8_t curSeg = 2;
uint8_t * curSegPointer = &curSeg;

EepromData eepromData = EepromData(eeAddress, numSegments, segmentData, curSegPointer);

void setup() {
    // Turn on serial port
    Serial.begin(9600);
    while(!Serial){}; // Wait for serial to connect
    Serial.println("Starting Arduino...");

    Serial.printf("Initial ");
    EepromData::printAll();

    // Initialize segment data
    for(uint8_t i=0; i < numSegments; i++){
        segmentData[i].initialize();

        // Fill in the data with some data to test reading
        segmentData[i].brightness += i;                          //  1,  2,  3,  4,  5,  6
        segmentData[i].saturation -= i;                          // 15, 14, 13, 12, 11, 10
        segmentData[i].curMode    = static_cast<LightMode>(i+4); //  4,  5,  6,  7,  8,  9
        segmentData[i].prevMode   = static_cast<LightMode>(i+8); //  8,  9, 10, 11, 12, 13
    }

    // Clear EEPROM, then save our current data structure to it (for testing)
    eepromData.clearAll();
    Serial.printf("Cleared ");
    EepromData::printAll();
    eepromData.saveAll();
    Serial.printf("Saved ");
    EepromData::printAll();
    printSegmentData();

    // Make some arbitrary changes
    curSeg = 4;
    segmentData[2].brightness = 15;
    printSegmentData();

    // Check for changes and update the EEPROM
    eepromData.saveChanges();
}

void loop() {
    delay(1000);
}

void printSegmentData(){
    Serial.printf("Read SegmentData [sizeof(SegmentData)==%d bytes, sizeof(segmentData)==%d bytes]:\n",
            sizeof(SegmentData),sizeof(segmentData));

    for(uint8_t i=0; i < numSegments; i++){
        Serial.printf("    item #%d: brightness=%d, saturation=%d, curMode=%d, prevMode=%d\n",
                i,segmentData[i].brightness,segmentData[i].saturation,segmentData[i].curMode,segmentData[i].prevMode);
    }
}
