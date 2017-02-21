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

        blinkRGBWnTimes(adjustInt(WHITE,0.005),2);
    }

    // Switch through different modes
    switch(curMode){
    case 0:
        writeRGBW(adjustInt(RED,0.005));
        break;
    case 1:
        writeRGBW(adjustInt(GREEN,0.005));
        break;
    case 2:
        writeRGBW(adjustInt(BLUE,0.005));
        break;
    case 3:
        writeRGBW(adjustInt(WHITE,0.005));
        break;
    case 4:
        writeRGBW(adjustInt(RED,0.005));
        delay(2);
        writeRGBW(adjustInt(YELLOW,0.005));
        delay(2);
        writeRGBW(adjustInt(GREEN,0.005));
        delay(2);
        writeRGBW(adjustInt(CYAN,0.005));
        delay(2);
        writeRGBW(adjustInt(BLUE,0.005));
        delay(2);
        writeRGBW(adjustInt(MAGENTA,0.005));
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

bool doModeOnce(){
    if(doModeOnceFlag){
        doModeOnceFlag = false;
        return true;
    } else {
        return false;
    }
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.Color(r, g, b, w));
    }
    pixels.show();
}

Color adjustInt(Color c, float newInt){
    return Color(c.hue,c.sat,newInt);
}

void hsi2rgbw(int h, float Sat, float Inten, int* rgbw) {
    // From: http://blog.saikoled.com/post/44677718712/how-to-convert-from-hsi-to-rgb-white
    int r, g, b, w; float Hue;
    Hue = fmod(h,360); // cycle H around to 0-360 degrees
    Hue = M_PI*Hue/(float)180; // Convert to radians.
    Sat = Sat>0?(Sat<1?Sat:1):0; // clamp S and I to interval [0,1]
    Inten = Inten>0?(Inten<2?Inten:2):0;

    if(Hue < M_2PI3) { // First third
        r = Sat*255*Inten/3*(1+   cos(Hue)/cos(M_1PI3-Hue));
        g = Sat*255*Inten/3*(1+(1-cos(Hue)/cos(M_1PI3-Hue)));
        b = 0;
        w = 255*(1-Sat)*Inten;
    } else if(Hue < M_4PI3) { // Second third
        Hue = Hue - M_2PI3;
        r = 0;
        g = Sat*255*Inten/3*(1+   cos(Hue)/cos(M_1PI3-Hue));
        b = Sat*255*Inten/3*(1+(1-cos(Hue)/cos(M_1PI3-Hue)));
        w = 255*(1-Sat)*Inten;
    } else { // Third section
        Hue = Hue - M_4PI3;
        r = Sat*255*Inten/3*(1+(1-cos(Hue)/cos(M_1PI3-Hue)));
        g = 0;
        b = Sat*255*Inten/3*(1+   cos(Hue)/cos(M_1PI3-Hue));
        w = 255*(1-Sat)*Inten;
    }
    rgbw[0]=r>0?(r<255?r:255):0;//r;
    rgbw[1]=g>0?(g<255?g:255):0;//g;
    rgbw[2]=b>0?(b<255?b:255):0;//b;
    rgbw[3]=w>0?(w<255?w:255):0;//b;
}

void writeHSI(int h, float s, float i) {
    int rgbwTemp[3];
    hsi2rgbw(h,s,i,rgbwTemp);
    writeRGBW(rgbwTemp);
}

void writeRGBW(int rgbw[]){
    writeRGBW(rgbw[0],rgbw[1],rgbw[2],rgbw[3]);
}

void writeRGBW(Color c){
    PREV = c; // Save the new color to the 'previous' value
    writeRGBW(c.red, c.green, c.blue, c.white);
}

void blinkRGBW(Color c){
    blinkRGBW(c,25);
}

void blinkRGBW(Color c, int d){
    writeRGBW(c);
    delay(d);
    writeRGBW(OFF);
    delay(d);
}

void blinkRGBWnTimes(Color c, int count){
    for(int i=0;i<count;i++){
        blinkRGBW(c);
    }
}
