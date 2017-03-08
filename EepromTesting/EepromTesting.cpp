// Do not remove the include below
#include "EepromTesting.h"

void setup() {
    // Turn on serial port
    Serial.begin(9600);
    while(!Serial){}; // Wait for serial to connect
    Serial.println("Starting Arduino...");

    int eeAddress = 0;
    float floatVarWrite = 123.456f;
    EEPROM.put(eeAddress, floatVarWrite);
    Serial.println("Written float data type!");
}

void loop() {
    int eeAddress = 0;
    float floatVarRead = 0.0f;
    EEPROM.get(eeAddress, floatVarRead);
    Serial.print("Read float data type! = ");
    Serial.println(floatVarRead, 3);

    delay(1000);
}
