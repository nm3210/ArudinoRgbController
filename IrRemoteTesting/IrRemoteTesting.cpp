// Do not remove the include below
#include "IrRemoteTesting.h"

#define IR_PIN   2
#define LED_PIN  5
#define NUM_LEDS 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

IRrecv irrecv(IR_PIN);
decode_results results;

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
	case MODE_OFF:
		writeRGBW(OFF); break;

	case MANUAL_COLOR:
		writeRGBW(MANUAL);
		break;

	case SOLID_RED:
		writeRGBW(RED); break;
	case SOLID_GREEN:
		writeRGBW(GREEN); break;
	case SOLID_BLUE:
		writeRGBW(BLUE); break;
	case SOLID_WHITE:
		writeRGBW(WHITE); break;
	case SOLID_CYAN:
		writeRGBW(CYAN); break;
	case SOLID_YELLOW:
		writeRGBW(YELLOW); break;
	case SOLID_MAGENTA:
		writeRGBW(MAGENTA); break;

	case SOLID_COLOR01:
		writeHSI( 10,1.0,1.0); break;
	case SOLID_COLOR04:
		writeHSI( 20,1.0,1.0); break;
	case SOLID_COLOR07:
		writeHSI( 30,1.0,1.0); break;
	case SOLID_COLOR10:
		writeHSI( 40,1.0,1.0); break;

	case SOLID_COLOR02:
		writeHSI(130,1.0,1.0); break;
	case SOLID_COLOR05:
		writeHSI(147,1.0,1.0); break;
	case SOLID_COLOR08:
		writeHSI(163,1.0,1.0); break;
	case SOLID_COLOR11:
		writeHSI(180,1.0,1.0); break;

	case SOLID_COLOR03:
		writeHSI(260,1.0,1.0); break;
	case SOLID_COLOR06:
		writeHSI(280,1.0,1.0); break;
	case SOLID_COLOR09:
		writeHSI(300,1.0,1.0); break;
	case SOLID_COLOR12:
		writeHSI(320,1.0,1.0); break;

	case SOLID_COLOR13:
		writeHSI(330,1.0,1.0); break;
	case SOLID_COLOR14:
		writeHSI(345,1.0,1.0); break;
	case SOLID_COLOR15:
		writeHSI(200,1.0,1.0); break;
	case SOLID_COLOR16:
		writeHSI(220,1.0,1.0); break;
		break;

	case MODE1:
	case MODE2:
	case MODE3:
	case MODE4:
	case MODE5:
	case MODE6:
	default:
		break;
	}

	// Print out current time (once every second)
//    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
//        displayClockTime = millis();
//        digitalClockDisplay();
//    }
	delay(0);
}

bool handleInterrupt(){
    bool changedModes = false;

    unsigned long tempButtonPress = decodeIrSignal();
    if(!checkRepeatBtn(tempButtonPress)){
        lastButtonPressed = tempButtonPress;
    }

    // Check for valid button press (meets one of our cases)
    if(lastButtonPressed!=0){
        checkBrightnessChange();
        changedModes |= checkModesSpecial();
        changedModes |= checkModes();

        lastButtonSave = lastButtonPressed;
    }

    if(changedModes && curMode!=prevMode && curMode!=MODE_OFF){
        Serial.print("curMode = "); Serial.println(curMode);
        doModeOnceFlag = true;
        count = 0;
    }

    return changedModes;
}

bool checkBrightnessChange(){
    switch(lastButtonPressed){
//    case CTRL1BTN_UP: case CTRL1BTN2_UP:
    case CTRL2BTN_BRIGHTUP: // case CTRL2BTN2_BRIGHTUP:
    case CTRL3BTN_BRIGHTUP:
    case CTRL4BTN_VOLUP:
        //do bright increase
        adjustBrightnessVal = constrain(adjustBrightnessVal + 1,1,numBrightLevels-1);
        break;
//    case CTRL1BTN_DOWN: case CTRL1BTN2_DOWN:
    case CTRL2BTN_BRIGHTDOWN: // case CTRL2BTN2_BRIGHTDOWN:
    case CTRL3BTN_BRIGHTDOWN:
    case CTRL4BTN_VOLDOWN:
        // do bright decrease
        adjustBrightnessVal = constrain(adjustBrightnessVal - 1,1,numBrightLevels-1);
        break;
    default:
        return false;
    }
    return true;
}
bool checkModesSpecial(){
    switch(lastButtonPressed){
    // Turn things OFF
    case CTRL2BTN_OFF: // case CTRL2BTN2_OFF:
        if(curMode!=MODE_OFF){
            changeMode(MODE_OFF);
        } else {
            return false;
        }
        break;

    // Turn things ON
    case CTRL2BTN_ON: // case CTRL2BTN2_ON:
        if(curMode==MODE_OFF){
            changeMode(prevMode);
        } else {
            return false;
        }
        break;

    // Toggle ON/OFF state
    case CTRL3BTN_ONOFF:
        if(curMode==MODE_OFF){
            changeMode(prevMode);
        } else {
            changeMode(MODE_OFF);
        }
        break;

    case CTRL2BTN_FLASH:  // case CTRL2BTN2_FLASH:
    case CTRL2BTN_STROBE: // case CTRL2BTN2_STROBE:
    case CTRL2BTN_FADE:   // case CTRL2BTN2_FADE:
    case CTRL2BTN_SMOOTH: // case CTRL2BTN2_SMOOTH:
        break;

    case CTRL3BTN_ROW07_1: // Red UP
        checkForManualColor();
        manualRedIndex = constrain(manualRedIndex + 1,0,numBrightLevels-1);
        MANUAL[0] = brightnessLevels[manualRedIndex];
        changeMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW07_2: // Green UP
        checkForManualColor();
        manualGreenIndex = constrain(manualGreenIndex + 1,0,numBrightLevels-1);
        MANUAL[1] = brightnessLevels[manualGreenIndex];
        changeMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW07_3: // Blue UP
        checkForManualColor();
        manualBlueIndex = constrain(manualBlueIndex + 1,0,numBrightLevels-1);
        MANUAL[2] = brightnessLevels[manualBlueIndex];
        changeMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW07_4: // WHITE UP
        checkForManualColor();
        manualWhiteIndex = constrain(manualWhiteIndex + 1,0,numBrightLevels-1);
        MANUAL[3] = brightnessLevels[manualWhiteIndex];
        changeMode(MANUAL_COLOR);
        break;

    case CTRL3BTN_ROW08_1: // Red DOWN
        checkForManualColor();
        manualRedIndex = constrain(manualRedIndex - 1,0,numBrightLevels-1);
        MANUAL[0] = brightnessLevels[manualRedIndex];
        changeMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW08_2: // Green DOWN
        checkForManualColor();
        manualGreenIndex = constrain(manualGreenIndex - 1,0,numBrightLevels-1);
        MANUAL[1] = brightnessLevels[manualGreenIndex];
        changeMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW08_3: // Blue DOWN
        checkForManualColor();
        manualBlueIndex = constrain(manualBlueIndex - 1,0,numBrightLevels-1);
        MANUAL[2] = brightnessLevels[manualBlueIndex];
        changeMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW08_4: // WHITE DOWN
        checkForManualColor();
        manualWhiteIndex = constrain(manualWhiteIndex - 1,0,numBrightLevels-1);
        MANUAL[3] = brightnessLevels[manualWhiteIndex];
        changeMode(MANUAL_COLOR);
        break;

//    case CTRL1BTN_UP:   // case CTRL1BTN2_UP:
//        adjustSaturationVal = constrain(adjustSaturationVal + 1,0,numBrightLevels-1);
//        Serial.print("New sat value: "); Serial.println(((float) 256-brightnessLevels[numBrightLevels-adjustSaturationVal-1])/255.0);
//        return false;
//    case CTRL1BTN_DOWN: // case CTRL1BTN2_DOWN:
//        adjustSaturationVal = constrain(adjustSaturationVal - 1,0,numBrightLevels-1);
//        Serial.print("New sat value: "); Serial.println(((float) 256-brightnessLevels[numBrightLevels-adjustSaturationVal-1])/255.0);
//        return false;
//    case CTRL1BTN_LEFT: // case CTRL1BTN2_LEFT:
//        curHueVal = (((curHueVal-1) % 360) + 360) % 360; // (((curHueVal-1) % 360) + 360) % 360
//        Serial.print("New hue value: "); Serial.println(curHueVal);
//        return false;
//    case CTRL1BTN_RIGHT: // case CTRL1BTN2_RIGHT:
//        curHueVal = (((curHueVal+1) % 360) + 360) % 360;
//        Serial.print("New hue value: "); Serial.println(curHueVal);
//        return false;
    default:
        return false;
    }
    return true;
}
bool checkModes(){
    switch(lastButtonPressed){
    case CTRL2BTN_RED: // case CTRL2BTN2_RED:
    case CTRL3BTN_RED:
//        Serial.println("Mode: SOLID_RED");
        changeMode(SOLID_RED); break;
    case CTRL2BTN_GREEN: // case CTRL2BTN2_GREEN:
    case CTRL3BTN_GREEN:
//        Serial.println("Mode: SOLID_GREEN");
        changeMode(SOLID_GREEN); break;
    case CTRL2BTN_BLUE: // case CTRL2BTN2_BLUE:
    case CTRL3BTN_BLUE:
//        Serial.println("Mode: SOLID_BLUE");
        changeMode(SOLID_BLUE); break;
    case CTRL2BTN_WHITE: // case CTRL2BTN2_WHITE:
    case CTRL3BTN_WHITE:
//        Serial.println("Mode: SOLID_WHITE");
        changeMode(SOLID_WHITE); break;

    case CTRL2BTN_ROW3_1: // case CTRL2BTN2_ROW2_1:
    case CTRL3BTN_ROW03_1:
        changeMode(SOLID_COLOR01); break;
    case CTRL2BTN_ROW3_2: // case CTRL2BTN2_ROW2_2:
    case CTRL3BTN_ROW03_2:
        changeMode(SOLID_COLOR02); break;
    case CTRL2BTN_ROW3_3: // case CTRL2BTN2_ROW2_3:
    case CTRL3BTN_ROW03_3:
        changeMode(SOLID_COLOR03); break;
    case CTRL2BTN_ROW4_1: // case CTRL2BTN2_ROW3_1:
    case CTRL3BTN_ROW04_1:
        changeMode(SOLID_COLOR04); break;
    case CTRL2BTN_ROW4_2: // case CTRL2BTN2_ROW3_2:
    case CTRL3BTN_ROW04_2:
        changeMode(SOLID_COLOR05); break;
    case CTRL2BTN_ROW4_3: // case CTRL2BTN2_ROW3_3:
    case CTRL3BTN_ROW04_3:
        changeMode(SOLID_COLOR06); break;
    case CTRL2BTN_ROW5_1: // case CTRL2BTN2_ROW4_1:
    case CTRL3BTN_ROW05_1:
        changeMode(SOLID_COLOR07); break;
    case CTRL2BTN_ROW5_2: // case CTRL2BTN2_ROW4_2:
    case CTRL3BTN_ROW05_2:
        changeMode(SOLID_COLOR08); break;
    case CTRL2BTN_ROW5_3: // case CTRL2BTN2_ROW4_3:
    case CTRL3BTN_ROW05_3:
        changeMode(SOLID_COLOR09); break;
    case CTRL2BTN_ROW6_1: // case CTRL2BTN2_ROW5_1:
    case CTRL3BTN_ROW06_1:
        changeMode(SOLID_COLOR10); break;
    case CTRL2BTN_ROW6_2: // case CTRL2BTN2_ROW5_2:
    case CTRL3BTN_ROW06_2:
        changeMode(SOLID_COLOR11); break;
    case CTRL2BTN_ROW6_3: // case CTRL2BTN2_ROW5_3:
    case CTRL3BTN_ROW06_3:
        changeMode(SOLID_COLOR12); break;

    case CTRL3BTN_ROW03_4:
        changeMode(SOLID_COLOR13); break;
    case CTRL3BTN_ROW04_4:
        changeMode(SOLID_COLOR14); break;
    case CTRL3BTN_ROW05_4:
        changeMode(SOLID_COLOR15); break;
    case CTRL3BTN_ROW06_4:
        changeMode(SOLID_COLOR16); break;

    default:
        return false;
    }
    return true;
}

bool checkRepeatBtn(uint32_t tempButtonPress){
    bool returnVal = false;
    if(tempButtonPress == BTN_REPEAT){
        switch(lastButtonSave){
        case CTRL2BTN_BRIGHTUP: // case CTRL2BTN2_BRIGHTUP:
        case CTRL3BTN_BRIGHTUP:
        case CTRL4BTN_VOLUP:
        case CTRL2BTN_BRIGHTDOWN: // case CTRL2BTN2_BRIGHTDOWN:
        case CTRL3BTN_BRIGHTDOWN:
        case CTRL4BTN_VOLDOWN:
        case CTRL3BTN_ROW07_1: // Red UP
        case CTRL3BTN_ROW07_2: // Green UP
        case CTRL3BTN_ROW07_3: // Blue UP
        case CTRL3BTN_ROW07_4: // WHITE UP
        case CTRL3BTN_ROW08_1: // Red DOWN
        case CTRL3BTN_ROW08_2: // Green DOWN
        case CTRL3BTN_ROW08_3: // Blue DOWN
        case CTRL3BTN_ROW08_4: // WHITE DOWN
            Serial.print("Repeat! Using last button press: ");
            Serial.println(lastButtonSave, HEX);
            lastButtonPressed = lastButtonSave;
            returnVal = true;
            break;
        default:
            break;
        }
    }
    return returnVal;
}

void checkForManualColor(){
    if(!isManualOn){
        hsi2rgbw(PREV, MANUAL);
        manualRedIndex   = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[0]);
        manualGreenIndex = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[1]);
        manualBlueIndex  = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[2]);
        manualWhiteIndex = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[3]);
    }
    isManualOn = true;
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
        if (tempResults != BTN_REPEAT) {
            Serial.println(tempResults, HEX); //print the value
        }
        // Restart the ir receiver state
        irrecv.resume();
    }
    return tempResults;
}

//void digitalClockDisplay(){
//    // digital clock display of the time
//    Serial.print(year());
//    Serial.print("-");
//    print2digits(month());
//    Serial.print("-");
//    print2digits(day());
//    Serial.print(" ");
//    print2digits(hour());
//    Serial.print(":");
//    print2digits(minute());
//    Serial.print(":");
//    print2digits(second());
//    Serial.println();
//}
//
//void print2digits(int number) {
//    if (number >= 0 && number < 10) {
//        Serial.write('0');
//  }
//  Serial.print(number);
//}

bool doModeOnce(){
    if(doModeOnceFlag){
        doModeOnceFlag = false;
        return true;
    } else {
        return false;
    }
}

Color adjustSat(Color c, float newSat){
    return Color(c.hue,newSat,c.intensity);
}

Color adjustInt(Color c, float newInt){
    return Color(c.hue,c.sat,newInt);
}

void hsi2rgbw(Color c, uint8_t* rgbw) {
    hsi2rgbw(c.hue,c.sat,c.intensity,rgbw);
}

void hsi2rgbw(int h, float Sat, float Inten, uint8_t* rgbw) {
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
    rgbw[3]=w>0?(w<255?w:255):0;//w;
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
        if(handleInterrupt()){return;};
        pixels.setPixelColor(i, pixels.Color(r, g, b, w));
    }
    pixels.show();
}

void writeRGBW(uint8_t rgbw[]){
    writeRGBW(rgbw[0],rgbw[1],rgbw[2],rgbw[3]);
}

void writeRGBW(Color c){
    c = adjustInt(c,c.intensity * ((float) brightnessLevels[adjustBrightnessVal])/255.0);
    c = adjustSat(c,c.sat * ((float) 256-brightnessLevels[numBrightLevels-adjustSaturationVal-1])/255.0);
    PREV = c; // Save the new color to the 'previous' value
    isManualOn = false;
    writeRGBW(c.red, c.green, c.blue, c.white);
}

void writeHSI(int h, float s, float i) {
    writeRGBW(Color(h,s,i));
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
        if(handleInterrupt()){waitLoop = false;return;};
        if(abs(millis() - trackTime) > i){
            waitLoop = false;
            return;
        }
        delay(0);
    }
}

int findClosestIndex(uint8_t values[], uint8_t size, uint8_t find) {
    int dist = abs(values[0] - find); //calculating first difference
    int closest = 0;
    int i;
    for (i = 1; i < size; ++i) {
        if (abs(values[i] - find) < dist) {  //checking for closest value
            dist = abs(values[i] - find);    //saving closest value in dist
            closest = i;                     //saving the position of the closest value
        }
    }
    return closest;
}

int wrapCount(uint16_t mod){
    count = (count + 1) % mod;
    return count;
}

int loopCount(uint16_t mod){
    if(count<=1){
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
