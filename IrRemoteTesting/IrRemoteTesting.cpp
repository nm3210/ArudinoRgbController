// Do not remove the include below
#include "IrRemoteTesting.h"

int IrPin = 11;
IRrecv irrecv(IrPin);
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
}

void loop() {
    if (irrecv.decode(&results)) {
        if (results.value != 0xFFFFFFFF) {
            Serial.println(results.value, HEX); //print the value
        }

        switch(results.value){
        case BTN_1:
            Serial.println("Button 1");
            break;

        case BTN_2:
            Serial.println("Button 2");
            break;

        case BTN_3:
            Serial.println("Button 3");
            break;

        case BTN_4:
            Serial.println("Button 4");
            break;

        case BTN_5:
            Serial.println("Button 5");
            break;

        case BTN_6:
            Serial.println("Button 6");
            break;

        case BTN_7:
            Serial.println("Button 7");
            break;

        case BTN_8:
            Serial.println("Button 8");
            break;

        case BTN_9:
            Serial.println("Button 9");
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

        case BTN_RIGHT:
            Serial.println("Button RIGHT");
            break;

        case BTN_DOWN:
            Serial.println("Button DOWN");
            break;

        case BTN_LEFT:
            Serial.println("Button LEFT");
            break;

        case 0xFFFFFFFF:
        default:
            break;
        }

        // Restart the receiver
        irrecv.resume();
    }
}
