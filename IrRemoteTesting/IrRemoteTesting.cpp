// Do not remove the include below
#include "IrRemoteTesting.h"

#define IR_PIN   2
IRrecv irrecv(IR_PIN);
decode_results results;

// Define buttons
#define BTN_1   0xFFA25D
#define BTN_2   0xFF629D
#define BTN_3   0xFFE21D
#define BTN_4   0xFF22DD
#define BTN_5   0xFF02FD
#define BTN_6   0xFFC23D
#define BTN_7   0xFFE01F
#define BTN_8   0xFFA857
#define BTN_9   0xFF906F
#define BTN_0   0xFF9867
#define BTN_ASTRX   0xFF6897
#define BTN_POUND   0xFFB04F
#define BTN_SELECT  0xFF38C7
#define BTN_UP      0xFF18E7
#define BTN_RIGHT   0xFF5AA5
#define BTN_DOWN    0xFF4AB5
#define BTN_LEFT    0xFF10EF

void setup() {
    // Turn on serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");
    irrecv.enableIRIn(); // start receiving data

    attachInterrupt(digitalPinToInterrupt(IR_PIN), interruptIr, LOW);
}

void loop() {
    // TODO: figure out how to continue the writeRGBW when the interrupt has been triggered

    // Detect IR and change mode
    if(decodeIrSignal() && changeMode){
        changeMode = false;
        count = 0;
        Serial.print("curMode = "); Serial.println(curMode);

        // Reattach interrupt
        attachInterrupt(digitalPinToInterrupt(IR_PIN), interruptIr, LOW);
    }

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
void interruptIr(){
    changeMode = true;
    detachInterrupt(0);
}

bool decodeIrSignal(){
    bool gotResults = false;
    if (irrecv.decode(&results)) {
        gotResults = true;
        switch(results.value){
        case 0xFFFFFFFF:
        default:
            gotResults = false;
            break;

        case BTN_1:
            Serial.println("Button 1");
            curMode = 0;
            break;

        case BTN_2:
            Serial.println("Button 2");
            curMode = 1;
            break;

        case BTN_3:
            Serial.println("Button 3");
            curMode = 2;
            break;

        case BTN_4:
            Serial.println("Button 4");
            curMode = 3;
            break;

        case BTN_5:
            Serial.println("Button 5");
            curMode = 4;
            break;

        case BTN_6:
            Serial.println("Button 6");
            curMode = 5;
            break;

        case BTN_7:
            Serial.println("Button 7");
            curMode = 6;
            break;

        case BTN_8:
            Serial.println("Button 8");
            curMode = 7;
            break;

        case BTN_9:
            Serial.println("Button 9");
            curMode = 8;
            break;

        case BTN_0:
            Serial.println("Button 0");
            break;

        case BTN_ASTRX:
            Serial.println("Button ASTERIX");
            break;

        case BTN_POUND:
            Serial.println("Button POUND");
            break;

        case BTN_SELECT:
            Serial.println("Button OK");
            break;

        case BTN_UP:
            Serial.println("Button UP");
            break;

        case BTN_DOWN:
            Serial.println("Button DOWN");
            break;

        case BTN_LEFT:
            Serial.println("Button LEFT");
            break;

        case BTN_RIGHT:
            Serial.println("Button RIGHT");
            break;
        }

        // Restart the ir receiver
        irrecv.resume();
    }
    return gotResults;
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
