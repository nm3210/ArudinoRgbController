// Do not remove the include below
#include "IrRemoteTesting.h"

#define IR_PIN   2
IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {
    // Turn on serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");
    // Enable IR pin & start receiving data
    irrecv.enableIRIn();
}

void loop() {
    // Detect IR and change mode
    handleInterrupt();

    switch(curMode){
        case 0:
        default:
        	break;
    }

    // Print out current time (once every second)
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
        digitalClockDisplay();
    }
    delay(0);
}

bool handleInterrupt(){
    bool changedModes = false;
    if(decodeIrSignal()){
        changedModes = true;
        count = 0;
        Serial.print("curMode = "); Serial.println(curMode);
    }
    return changedModes;
}

bool decodeIrSignal(){
    bool changeModes = false;
    while (!irrecv.isIdle()); // Listen to the whole IR signal coming in (very important!)
    if (irrecv.decode(&results)) {
        if (results.value != 0xFFFFFFFF) {
            Serial.println(results.value, HEX); //print the value
        }
        changeModes = true;
        switch(results.value){
        case 0xFFFFFFFF:
        default:
            changeModes = false;
            break;

        case BTNCTRL1_1:
            Serial.println("Button 1");
            curMode = 0;
            break;

        case BTNCTRL1_2:
            Serial.println("Button 2");
            curMode = 1;
            break;

        case BTNCTRL1_3:
            Serial.println("Button 3");
            curMode = 2;
            break;

        case BTNCTRL1_4:
            Serial.println("Button 4");
            curMode = 3;
            break;

        case BTNCTRL1_5:
            Serial.println("Button 5");
            curMode = 4;
            break;

        case BTNCTRL1_6:
            Serial.println("Button 6");
            curMode = 5;
            break;

        case BTNCTRL1_7:
            Serial.println("Button 7");
            curMode = 6;
            break;

        case BTNCTRL1_8:
            Serial.println("Button 8");
            curMode = 7;
            break;

        case BTNCTRL1_9:
            Serial.println("Button 9");
            curMode = 8;
            break;

        case BTNCTRL1_0:
            Serial.println("Button 0");
            curMode = 9;
            break;

        case BTNCTRL1_ASTRX:
            Serial.println("Button ASTERIX");
            break;

        case BTNCTRL1_POUND:
            Serial.println("Button POUND");
            break;

        case BTNCTRL1_SELECT:
            Serial.println("Button OK");
            break;

        case BTNCTRL1_UP:
            Serial.println("Button UP");
            break;

        case BTNCTRL1_DOWN:
            Serial.println("Button DOWN");
            break;

        case BTNCTRL1_LEFT:
            Serial.println("Button LEFT");
            break;

        case BTNCTRL1_RIGHT:
            Serial.println("Button RIGHT");
            break;
        }

        // Restart the ir receiver state
        irrecv.resume();
    }
    return changeModes;
}

void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(year());
    Serial.print("-");
    print2digits(month());
    Serial.print("-");
    print2digits(day());
    Serial.print(" ");
    print2digits(hour());
    Serial.print(":");
    print2digits(minute());
    Serial.print(":");
    print2digits(second());
    Serial.println();
}

void print2digits(int number) {
    if (number >= 0 && number < 10) {
        Serial.write('0');
  }
  Serial.print(number);
}
