// Do not remove the include below
#include "IrRemoteTesting.h"

#define IR_PIN   2
#define LED_PIN  5
#define NUM_LEDS 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

IRrecv irrecv(IR_PIN);
decode_results results;

uint8_t adjustBrightnessVal = 0;
uint8_t adjustSaturationVal = numBrightLevels-1;
int curHueVal = 0;

void setup() {
    // Turn on serial port
    Serial.begin(115200);
    Serial.println("Starting Arduino...");
    // Start pixel strip
    pixels.begin();
    pixels.show();
    // Enable IR pin & start receiving data
    irrecv.enableIRIn();
}

void loop() {
    // Detect IR and change mode
    handleInterrupt();

    // Switch through different modes
    switch(curMode){
    case 0:
        if(doModeOnce()){
            crossFade(WHITE,OFF,1500);
        }
        break;
    case 1:
        if(doModeOnce()){
            crossFadeTo(WHITE,200);
            crossFadeTo(RED,200);
            crossFadeTo(YELLOW,200);
            crossFadeTo(GREEN,200);
            crossFadeTo(OFF,250);
            waitForButton(1000);
        }
        break;
    case 2:
//        writeRGBW(RED);
        writeHSI(wrapCount(360),1.0,1.0);
        delay(25);
        break;
    case 3:
//        writeRGBW(GREEN);
        writeHSI(RED.hue,1.0,loopCount(255)/255.0);
        delay(5);
        break;
    case 4:
//        writeRGBW(BLUE);
        writeHSI(GREEN.hue,1.0,loopCount(255)/255.0);
        delay(5);
        break;
    case 5:
        writeHSI(BLUE.hue,1.0,loopCount(255)/255.0);
        delay(5);
        break;
    case 7:
        writeHSI(WHITE.hue,0.0,loopCount(255)/255.0);
        delay(5);
        break;

    case 6:
        writeHSI(wrapCount(360),1.0,1.0);
        delay(5);
        break;
    case 8:
        writeHSI(wrapCount(360),1.0,1.0);
        delay(1);
        break;
    case 9:
        writeRGBW(RED);
        writeRGBW(YELLOW);
        writeRGBW(GREEN);
        writeRGBW(CYAN);
        writeRGBW(BLUE);
        writeRGBW(MAGENTA);
        break;

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

    lastButtonPressed = decodeIrSignal();

    // Check for valid button press (meets one of our cases)
    if(lastButtonPressed!=0){
        checkBrightnessChange();
        changedModes |= checkModesSpecial();
        changedModes |= checkModes();
    }

    if(changedModes && curMode!=prevMode && curMode!=MODE_OFF){
        Serial.print("curMode = "); Serial.println(curMode);
        doModeOnceFlag = true;
        count = 0;
//        blinkRGBWnTimes(WHITE,2);
    }

    return changedModes;
}

bool checkBrightnessChange(){
    switch(lastButtonPressed){
//    case CTRL1BTN_UP: case CTRL1BTN2_UP:
    case CTRL2BTN_BRIGHTUP: case CTRL2BTN2_BRIGHTUP:
    case CTRL3BTN_VOLUP:
        //do bright increase
//        Serial.println("Button: Brightness UP");
        adjustBrightnessVal = constrain(adjustBrightnessVal + 1,0,numBrightLevels-1);
        break;
//    case CTRL1BTN_DOWN: case CTRL1BTN2_DOWN:
    case CTRL2BTN_BRIGHTDOWN: case CTRL2BTN2_BRIGHTDOWN:
    case CTRL3BTN_VOLDOWN:
        // do bright decrease
//        Serial.println("Button: Brightness DOWN");
        adjustBrightnessVal = constrain(adjustBrightnessVal - 1,0,numBrightLevels-1);
        break;
    default:
        return false;
    }
    return true;
}
bool checkModesSpecial(){
    switch(lastButtonPressed){
    case CTRL2BTN_OFF: case CTRL2BTN2_OFF:
//        Serial.println("Button: OFF (hard)");
        if(curMode!=MODE_OFF){
            changeMode(MODE_OFF);
        } else {
            return false;
        }
        break;
    case CTRL2BTN_ON: case CTRL2BTN2_ON:
//        Serial.println("Button: ON (hard)");
        if(curMode==MODE_OFF){
            changeMode(prevMode);
        } else {
            return false;
        }
        break;

    case CTRL2BTN_FLASH: case CTRL2BTN2_FLASH:
    case CTRL2BTN_STROBE: case CTRL2BTN2_STROBE:
    case CTRL2BTN_FADE: case CTRL2BTN2_FADE:
    case CTRL2BTN_SMOOTH: case CTRL2BTN2_SMOOTH:
        changeMode(MODE1);
        break;

    case CTRL1BTN_UP: case CTRL1BTN2_UP:
        adjustSaturationVal = constrain(adjustSaturationVal + 1,0,numBrightLevels-1);
        Serial.print("New sat value: "); Serial.println(((float) 256-brightnessLevels[numBrightLevels-adjustSaturationVal-1])/255.0);
        return false;
    case CTRL1BTN_DOWN: case CTRL1BTN2_DOWN:
        adjustSaturationVal = constrain(adjustSaturationVal - 1,0,numBrightLevels-1);
        Serial.print("New sat value: "); Serial.println(((float) 256-brightnessLevels[numBrightLevels-adjustSaturationVal-1])/255.0);
        return false;
    case CTRL1BTN_LEFT: case CTRL1BTN2_LEFT:
//        Serial.print("Cur hue value: "); Serial.print(curHueVal);
        curHueVal = (((curHueVal-1) % 360) + 360) % 360; // (((curHueVal-1) % 360) + 360) % 360
        Serial.print("New hue value: "); Serial.println(curHueVal);
        return false;
    case CTRL1BTN_RIGHT: case CTRL1BTN2_RIGHT:
//        Serial.print("Cur hue value: "); Serial.print(curHueVal);
        curHueVal = (((curHueVal+1) % 360) + 360) % 360;
        Serial.print("New hue value: "); Serial.println(curHueVal);
        return false;
    default:
        return false;
    }
    return true;
}
bool checkModes(){
    switch(lastButtonPressed){
    case CTRL2BTN_RED: case CTRL2BTN2_RED:
//        Serial.println("Mode: SOLID_RED");
        changeMode(SOLID_RED); break;
    case CTRL2BTN_GREEN: case CTRL2BTN2_GREEN:
//        Serial.println("Mode: SOLID_GREEN");
        changeMode(SOLID_GREEN); break;
    case CTRL2BTN_BLUE: case CTRL2BTN2_BLUE:
//        Serial.println("Mode: SOLID_BLUE");
        changeMode(SOLID_BLUE); break;
    case CTRL2BTN_WHITE: case CTRL2BTN2_WHITE:
//        Serial.println("Mode: SOLID_WHITE");
        changeMode(SOLID_WHITE); break;

    case CTRL2BTN_ROW3_1: case CTRL2BTN2_ROW2_1:
        changeMode(SOLID_COLOR1); break;
    case CTRL2BTN_ROW3_2: case CTRL2BTN2_ROW2_2:
        changeMode(SOLID_COLOR2); break;
    case CTRL2BTN_ROW3_3: case CTRL2BTN2_ROW2_3:
        changeMode(SOLID_COLOR3); break;
    case CTRL2BTN_ROW4_1: case CTRL2BTN2_ROW3_1:
        changeMode(SOLID_COLOR4); break;
    case CTRL2BTN_ROW4_2: case CTRL2BTN2_ROW3_2:
        changeMode(SOLID_COLOR5); break;
    case CTRL2BTN_ROW4_3: case CTRL2BTN2_ROW3_3:
        changeMode(SOLID_COLOR6); break;
    case CTRL2BTN_ROW5_1: case CTRL2BTN2_ROW4_1:
        changeMode(SOLID_COLOR7); break;
    case CTRL2BTN_ROW5_2: case CTRL2BTN2_ROW4_2:
        changeMode(SOLID_COLOR8); break;
    case CTRL2BTN_ROW5_3: case CTRL2BTN2_ROW4_3:
        changeMode(SOLID_COLOR9); break;

    case CTRL2BTN_ROW6_1: case CTRL2BTN2_ROW5_1:
        changeMode(SOLID_YELLOW); break;
    case CTRL2BTN_ROW6_2: case CTRL2BTN2_ROW5_2:
        changeMode(SOLID_CYAN); break;
    case CTRL2BTN_ROW6_3: case CTRL2BTN2_ROW5_3:
        changeMode(SOLID_MAGENTA); break;
    default:
        return false;
    }
    return true;
}

void changeMode(LightColor newMode){
    if(curMode!=MODE_OFF){
        prevMode = curMode;
    }
    curMode = newMode;
}

long decodeIrSignal(){
    uint32_t tempResults = 0;
    while (!irrecv.isIdle()); // Listen to the whole IR signal coming in (very important!)
    if (irrecv.decode(&results)) {
        tempResults = results.value;
        if (tempResults != 0xFFFFFFFF) {
            Serial.println(tempResults, HEX); //print the value
        }
        // Restart the ir receiver state
        irrecv.resume();
    }
    return tempResults;
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
        if(handleInterrupt()){return;};
        pixels.setPixelColor(i, pixels.Color(r, g, b, w));
    }
    pixels.show();
}

Color adjustSat(Color c, float newSat){
    return Color(c.hue,newSat,c.intensity);
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
    Inten = Inten>0?(Inten<2?Inten:2):0; //TODO: should this be 1 or 2...?

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
    writeRGBW(Color(h,s,i));
}

void writeRGBW(int rgbw[]){
    writeRGBW(rgbw[0],rgbw[1],rgbw[2],rgbw[3]);
}

void writeRGBW(Color c){
    c = adjustInt(c,c.intensity * adjustBrightnessVal);
    c = adjustSat(c,c.sat * adjustSaturationVal);
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

void crossFadeHSI(int h1, float s1, float i1,
                  int h2, float s2, float i2, long steps, int dur){
    if(handleInterrupt()){return;};
//    if(handleInterrupt()){writeRGBW(OFF);return;}; // Check for button press
    float rho1 = i1;//(i1!=0)?i1:LOWERLIMIT;
    float rho2 = i2;//(i2!=0)?i2:LOWERLIMIT;

    bool ccw = fmod((h2-h1+360.0),360.0) < 180.0;
    float gamma = (180 - abs(fmod(abs(h1-h2),360.0) - 180.0));
    float length = sqrt(pow(rho1,2)-2.0*rho1*rho2*cos(radians(gamma))+pow(rho2,2));
    float alpha;
    if(rho1 != 0){
        alpha = acos((pow(rho1,2)+pow(length,2)-pow(rho2,2))/(2.0*rho1*length));
    } else {
        alpha = acos((pow(rho2,2)+pow(length,2)-pow(rho1,2))/(2.0*rho2*length));
    }

    float newRho;
    uint16_t newTheta;
    // Loop through the number of steps
    for (int i = 1; i <= steps; i++) {
        if(handleInterrupt()){return;};
//        if(handleInterrupt()){writeRGBW(OFF);return;}; // Check for button press (so you can escape the loop)

        float tempRho = length*((i-1.0)/(steps-1.0)); // calculate one part of the line

        if (tempRho != 0){
            newRho = sqrt(pow(rho1,2) - 2.0*rho1*tempRho*cos(alpha)+pow(tempRho,2));
        } else {
            newRho = rho1;
        }
        if ((rho1 != 0) && (newRho != 0)){
            float acosarg = (pow(rho1,2)+pow(newRho,2)+-pow(tempRho,2))/(2.0*rho1*newRho);
            newTheta = round(degrees((round(acosarg*1000000)/1000000 == 1)?0:acos(acosarg))*1000)/1000;
        } else {
            if(fmod(h1-h2,360)<180){
                newTheta = fmod(h1-h2,360);
            } else {
                newTheta = -fmod(h1-h2,360);
            }
        }
        if (ccw){
            newTheta = fmod(h1+newTheta,360.0);
        } else {
            newTheta = fmod(h1-newTheta+360,360.0);
        }

        float newSat = s1+(s2-s1)*((i-1.0)/(steps-1.0)); // simple interpolation
//        float newInten = i1+(i2-i1)*((i-1.0)/(steps-1.0)); // simple interpolation
        writeHSI(newTheta, newSat, newRho);
        delay(dur);

    }
}

void crossFade(Color c1, Color c2, long steps, int dur){
    if(c1 == OFF){
        c1.hue = c2.hue;
        c1.sat = c2.sat;
    } else if(c2 == OFF){
        c2.hue = c1.hue;
        c2.sat = c1.sat;
    }
    PREV = c2; // Save the new color to the 'previous' value
    crossFadeHSI(c1.hue, c1.sat, c1.intensity,
                 c2.hue, c2.sat, c2.intensity, steps, dur);
}

void crossFade(Color c1, Color c2, float msec){
    int maxDelay = 5; // set max delay to 5ms (wiggle room)
    if(msec<maxDelay){return;};
    int timeStep = max(maxDelay, msec / 100); // default to 100 steps (pretty smooth)
    long numSteps = (long) (msec/timeStep);

    crossFade(c1, c2, numSteps, timeStep);
}

void crossFadeTo(Color c1, float msec){
    if(PREV!=c1){
        crossFade(PREV,c1,msec);
    }
}

void waitForButton(unsigned long i){
    unsigned long trackTime = millis();
    boolean waitLoop = true;
    while(waitLoop){
        if(handleInterrupt()){return;};
//        if(changeMode){writeRGBW(OFF);waitLoop = false;return;};
        if(abs(millis() - trackTime) > i){
            waitLoop = false;
            return;
        }
        delay(0);
    }
}

int wrapCount(uint16_t mod){
    count = (count + 1) % mod;
    return count;
}

int loopCount(uint16_t mod){
    if(count<=0){
        countMode=0;
    } else if(count>mod){
        countMode=1;
    }
    if (countMode == 0){
        count = (count + 1);
    } else if(countMode == 1){
        count = (count - 1);
    }
    return count;
}
