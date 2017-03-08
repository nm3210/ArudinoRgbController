// Do not remove the include below
#include "EepromTesting.h"

void setup() {
    // Turn on serial port
    Serial.begin(9600);
    while(!Serial){}; // Wait for serial to connect
    Serial.println("Starting Arduino...");

    int eeAddress = 0;
    SegmentData segDataWrite;
    segDataWrite.initialize();
    EEPROM.put(eeAddress, segDataWrite);
    Serial.print("Wrote SegmentData type!");
}

void loop() {
    int eeAddress = 0;
    SegmentData segDataRead;
    EEPROM.get(eeAddress, segDataRead);

    Serial.print("Read SegmentData [sizeof(SegmentData)==");
    Serial.print(sizeof(SegmentData));
    Serial.print("bytes, sizeof(segDataRead)==");
    Serial.print(sizeof(segDataRead));
    Serial.print("bytes] brightness=");
    Serial.print(segDataRead.brightness);
    Serial.print(", saturation=");
    Serial.print(segDataRead.saturation);
    Serial.print(", curMode=");
    Serial.print(segDataRead.curMode);
    Serial.print(", prevMode=");
    Serial.println(segDataRead.prevMode);
    delay(1000);
}
