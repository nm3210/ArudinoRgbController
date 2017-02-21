// Do not remove the include below
#include "NeoPixelTesting.h"

#define LED_PIN 5
#define NUM_LEDS 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

//The setup function is called once at startup of the sketch
void setup() {
    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD)  |  // Analog Comparator Enabled
           (0 << ACBG) |  // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
           (0 << ACO)  |  // Analog Comparator Output: Off
           (1 << ACI)  |  // Analog Comparator Interrupt Flag: Clear Pending Interrupt
           (1 << ACIE) |  // Analog Comparator Interrupt: Enabled
           (0 << ACIC) |  // Analog Comparator Input Capture: Disabled
           (1 << ACIS1)| (0 << ACIS0); // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

    // Turn on serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");

    pixels.begin();
    pixels.show();
}

// The loop function is called in an endless loop
void loop() {
    // Handle button press
    if(changeMode){
        changeMode = false;
        doModeOnceFlag = true;
        count = 0;
        curMode++;
        Serial.print("curMode = "); Serial.println(curMode);

        fullColor(0,0,0,1);
        delay(25);
        fullColor(0,0,0,0);
        delay(25);
        fullColor(0,0,0,1);
        delay(25);
        fullColor(0,0,0,0);
        delay(25);
    }

    // Switch through different modes
    switch(curMode){
    case 0:
        fullColor(1,0,0,0);
        break;
    case 1:
        fullColor(0,1,0,0);
        break;
    case 2:
        fullColor(0,0,1,0);
        break;
    case 3:
        fullColor(0,0,0,1);
        break;
    case 4:
        fullColor(1,0,0,0);
        delay(2);
        fullColor(1,1,0,0);
        delay(2);
        fullColor(0,1,0,0);
        delay(2);
        fullColor(0,1,1,0);
        delay(2);
        fullColor(0,0,1,0);
        delay(2);
        fullColor(1,0,1,0);
        delay(2);
        break;

    default:
        curMode = 0; // Loop back
        break;

    }
    // Print out current time (once every second)
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
        digitalClockDisplay();
    }
    delay(0);
}

// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other [complex] functions
ISR(ANALOG_COMP_vect) {
    interruptCurTime = millis();
    //check to see if the interrupt was called in the last 250 milliseconds
    if (abs(interruptCurTime - interruptLastTime) > 250) {
        interruptLastTime = interruptCurTime;
        changeMode = true;
    }
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

void fullColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.Color(r, g, b, w));
    }
    pixels.show();
}
