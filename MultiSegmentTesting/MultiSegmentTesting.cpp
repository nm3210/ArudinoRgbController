#include "MultiSegmentTesting.h"

#define IR_PIN   2
#define LED_PIN  17

//const uint16_t numLeds = 8;
//const uint8_t  numSegments = 4;
//const uint16_t segments[numSegments+1] = {0, 2, 4, 6, 8};
//const uint16_t segments[numSegments+1] = {0, 5, 6, 7, 8};
//const uint16_t numLeds = 150;
//const uint8_t  numSegments = 5;
//const uint16_t segments[numSegments+1] = {0, 30, 60, 90, 120, 150};
//const uint16_t numLeds = 56;
//const uint8_t  numSegments = 3;
//const uint16_t segments[numSegments+1] = {0, 18, 38, 56};
const uint16_t numLeds = 112;
const uint8_t  numSegments = 6;
const uint16_t segments[numSegments+1] = {0, 18, 38, 56, 74, 94, 112};

int16_t rainbowIndex[numSegments] = {0,0,0,0,0,0};

const uint16_t eeAddress = 0;
uint16_t rainbowIncrement = 4;

SegmentData segmentData[numSegments];
uint8_t curSeg = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, LED_PIN, NEO_GRBW + NEO_KHZ800);

IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {
    // Turn on serial port
    Serial.begin(9600);
    Serial.println("Starting Arduino...");

    // Start pixel strip
    strip.begin();
    strip.show();

    // Enable IR pin & start receiving data
    irrecv.enableIRIn();

    // Initialize segment data
    for(uint8_t i=0; i < numSegments; i++){
        segmentData[i].initialize();
    }

    // Read data from EEPROM to get previous configuration
//    writeSegDataToEeprom(eeAddress);
    readSegDataFromEeprom(eeAddress);
}

void loop() {
	irInterrupt();

	// Switch through different modes
	for(uint8_t seg = 0; seg < numSegments; seg++){
		switch(segmentData[seg].curMode){
		case MODE_OFF:
			writeRGBW(OFF,seg); break;

		case SOLID_WHITE:
			writeRGBW(WHITE,seg); break;
		case SOLID_RED:
			writeRGBW(RED,seg); break;
		case SOLID_GREEN:
			writeRGBW(GREEN,seg); break;
		case SOLID_BLUE:
			writeRGBW(BLUE,seg); break;
		case SOLID_CYAN:
			writeRGBW(CYAN,seg); break;
		case SOLID_YELLOW:
			writeRGBW(YELLOW,seg); break;
		case SOLID_MAGENTA:
			writeRGBW(MAGENTA,seg); break;

		case SOLID_COLOR01:
			writeHSI( 10,1.0,1.0,seg); break;
		case SOLID_COLOR04:
			writeHSI( 20,1.0,1.0,seg); break;
		case SOLID_COLOR07:
			writeHSI( 30,1.0,1.0,seg); break;
		case SOLID_COLOR10:
			writeHSI( 40,1.0,1.0,seg); break;

		case SOLID_COLOR02:
			writeHSI(130,1.0,1.0,seg); break;
		case SOLID_COLOR05:
			writeHSI(147,1.0,1.0,seg); break;
		case SOLID_COLOR08:
			writeHSI(163,1.0,1.0,seg); break;
		case SOLID_COLOR11:
			writeHSI(180,1.0,1.0,seg); break;

		case SOLID_COLOR03:
			writeHSI(260,1.0,1.0,seg); break;
		case SOLID_COLOR06:
			writeHSI(280,1.0,1.0,seg); break;
		case SOLID_COLOR09:
			writeHSI(300,1.0,1.0,seg); break;
		case SOLID_COLOR12:
			writeHSI(320,1.0,1.0,seg); break;

		case SOLID_COLOR13:
			writeHSI(330,1.0,1.0,seg); break;
		case SOLID_COLOR14:
			writeHSI(345,1.0,1.0,seg); break;
		case SOLID_COLOR15:
			writeHSI(200,1.0,1.0,seg); break;
		case SOLID_COLOR16:
			writeHSI(220,1.0,1.0,seg); break;

		case MODE1:
        case MODE2:
        case MODE3:
        case MODE4:
        case MODE5:
        case MODE6:
		    writeRainbow(seg); break;
		default:
			break;
		}

	}

	if(blinkCurSegment){
		waitForButton(BLINKTIME);
		Color c = Color(0,0,0);
		if(segmentData[curSeg].curMode!=SOLID_WHITE || segmentData[curSeg].brightness!=1){
			c = WHITE;
		}
		c = adjustInt(c,c.intensity * ((float) brightnessLevels[1])/255.0);
		if(doAllSegmentsMode){
			for(uint8_t i = 0; i < numSegments; i++){
				writeRGBW(c.red, c.green, c.blue, c.white, i);
			}
		} else {
		    writeRGBW(c.red, c.green, c.blue, c.white, curSeg);
		}
		waitForButton(BLINKTIME);

		blinkCount++;
		if(blinkCount>=NUMBLINKS){
			blinkCurSegment = false;
			blinkCount = 0;
		}
	}

    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();
    }

    if(abs(millis() - rainbowRefreshTime) > RAINBOW_SPEED){
        rainbowRefreshTime = millis();

        for(uint8_t seg = 0; seg < numSegments; seg++){
            switch(segmentData[seg].curMode){
            case MODE1:
            case MODE2:
            case MODE3:
            case MODE4:
            case MODE5:
            case MODE6:
            default:
                rainbowIndex[seg] = (rainbowIndex[seg] - rainbowIncrement) % 360;
                if(rainbowIndex[seg] < 0){
                    rainbowIndex[seg] += 360;
                }
                break;
            }
        }
    }

    delay(0);
}

bool irInterrupt(){
    bool changedModes = false;
	unsigned long tempButtonPress = decodeIrSignal();

	// Check for repeat button
    if(!checkRepeatBtn(tempButtonPress)){
        lastButtonPressed = tempButtonPress;
    }

    // Handle long press detection (using repeat button press)
    if(tempButtonPress == BTN_REPEAT){
        if(longPressCheck==false){
        	longPressTime = millis();
        	longPressCheck = true;
        }

        if(longPressCheck && abs(millis() - longPressTime) > LONGPRESS_WAIT){
        	longPressCheck = false;
        	doLongPressAction(lastButtonSave);
        }
    } else if(tempButtonPress !=0) {
    	longPressCheck = false;
    }


    // Check for valid button press (meets one of our cases)
    if(lastButtonPressed!=0 && lastButtonPressed!=BTN_REPEAT){
        checkButtonBrightness(lastButtonPressed);
        changedModes |= checkButtonSpecial(lastButtonPressed);
        changedModes |= checkButtonMode(lastButtonPressed);

        lastButtonSave = lastButtonPressed;
    }

    if(changedModes && segmentData[curSeg].curMode!=segmentData[curSeg].prevMode && segmentData[curSeg].curMode!=MODE_OFF){
        Serial.print("curMode = ");
        Serial.println(segmentData[curSeg].curMode);
    }

	return changedModes;
}

long decodeIrSignal(){
    uint32_t tempResults = 0;
    while (!irrecv.isIdle()); // Listen to the whole IR signal coming in (very important!)
    if (irrecv.decode(&results)) {
        tempResults = results.value;
		Serial.println(results.value, HEX); //print the value

        // Restart the ir receiver state
        irrecv.resume();
    }
    return tempResults;
}

bool checkRepeatBtn(uint32_t buttonPressed){
    bool returnVal = false;
    if(buttonPressed == BTN_REPEAT){
        switch(lastButtonSave){
        case CTRL2BTN_BRIGHTUP: // case CTRL2BTN2_BRIGHTUP:
        case CTRL3BTN_BRIGHTUP:
        case CTRL4BTN_VOLUP:
        case CTRL2BTN_BRIGHTDOWN: // case CTRL2BTN2_BRIGHTDOWN:
        case CTRL3BTN_BRIGHTDOWN:
        case CTRL4BTN_VOLDOWN:
        case CTRL3BTN_ROW07_4:
        case CTRL3BTN_ROW08_4:
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

void doLongPressAction(uint32_t buttonPressed){
	Serial.print("<!--- Long press detected!! ---!> ");
	Serial.println(buttonPressed, HEX);

	switch(buttonPressed){
	case CTRL3BTN_ROW01_3:
		doAllSegmentsMode = !doAllSegmentsMode;
		if(doAllSegmentsMode){
			blinkCurSegment = true;
		}
		blinkCount = 0;
		break;
	case CTRL3BTN_ONOFF:
        if(segmentData[curSeg].curMode!=MODE_OFF){
            for(uint8_t i=0; i < numSegments; i++){
            	changeMode(segmentData[i].prevMode,i);
            }
        } else {
            changeMode(MODE_OFF,true);
        }
		break;

    case CTRL2BTN_RED: // case CTRL2BTN2_RED:
    case CTRL3BTN_RED:
    case CTRL2BTN_GREEN: // case CTRL2BTN2_GREEN:
    case CTRL3BTN_GREEN:
    case CTRL2BTN_BLUE: // case CTRL2BTN2_BLUE:
    case CTRL3BTN_BLUE:
    case CTRL2BTN_WHITE: // case CTRL2BTN2_WHITE:
    case CTRL3BTN_WHITE:
    case CTRL2BTN_ROW3_1: // case CTRL2BTN2_ROW2_1:
    case CTRL3BTN_ROW03_1:
    case CTRL2BTN_ROW3_2: // case CTRL2BTN2_ROW2_2:
    case CTRL3BTN_ROW03_2:
    case CTRL2BTN_ROW3_3: // case CTRL2BTN2_ROW2_3:
    case CTRL3BTN_ROW03_3:
    case CTRL2BTN_ROW4_1: // case CTRL2BTN2_ROW3_1:
    case CTRL3BTN_ROW04_1:
    case CTRL2BTN_ROW4_2: // case CTRL2BTN2_ROW3_2:
    case CTRL3BTN_ROW04_2:
    case CTRL2BTN_ROW4_3: // case CTRL2BTN2_ROW3_3:
    case CTRL3BTN_ROW04_3:
    case CTRL2BTN_ROW5_1: // case CTRL2BTN2_ROW4_1:
    case CTRL3BTN_ROW05_1:
    case CTRL2BTN_ROW5_2: // case CTRL2BTN2_ROW4_2:
    case CTRL3BTN_ROW05_2:
    case CTRL2BTN_ROW5_3: // case CTRL2BTN2_ROW4_3:
    case CTRL3BTN_ROW05_3:
    case CTRL2BTN_ROW6_1: // case CTRL2BTN2_ROW5_1:
    case CTRL3BTN_ROW06_1:
    case CTRL2BTN_ROW6_2: // case CTRL2BTN2_ROW5_2:
    case CTRL3BTN_ROW06_2:
    case CTRL2BTN_ROW6_3: // case CTRL2BTN2_ROW5_3:
    case CTRL3BTN_ROW06_3:
    case CTRL3BTN_ROW03_4:
    case CTRL3BTN_ROW04_4:
    case CTRL3BTN_ROW05_4:
    case CTRL3BTN_ROW06_4:
    	// Copy over brightness to all the other segments
        for(uint8_t i=0; i < numSegments; i++){
        	if(i==curSeg) continue;
        	segmentData[i].brightness = segmentData[curSeg].brightness;
        }
        // Apply color to all segments
    	checkButtonMode(buttonPressed,true);
        break;
	default:
		break;
	}
}

bool checkButtonBrightness(uint32_t buttonPressed){
    switch(buttonPressed){
//    case CTRL1BTN_UP: case CTRL1BTN2_UP:
    case CTRL2BTN_BRIGHTUP: // case CTRL2BTN2_BRIGHTUP:
    case CTRL3BTN_BRIGHTUP:
    case CTRL4BTN_VOLUP:
        //do bright increase
    	if(doAllSegmentsMode){
    		for(uint8_t i = 0; i < numSegments; i++){
    		    segmentData[i].brightness = constrain(segmentData[i].brightness + 1,1,numBrightLevels-1);
    		}
    	} else {
    	    segmentData[curSeg].brightness = constrain(segmentData[curSeg].brightness + 1,1,numBrightLevels-1);
    	}
        // Update EEPROM with new configuration
        writeSegDataToEeprom(eeAddress);
        break;
//    case CTRL1BTN_DOWN: case CTRL1BTN2_DOWN:
    case CTRL2BTN_BRIGHTDOWN: // case CTRL2BTN2_BRIGHTDOWN:
    case CTRL3BTN_BRIGHTDOWN:
    case CTRL4BTN_VOLDOWN:
        // do bright decrease
        if(doAllSegmentsMode){
            for(uint8_t i = 0; i < numSegments; i++){
                segmentData[i].brightness = constrain(segmentData[i].brightness - 1,1,numBrightLevels-1);
            }
        } else {
            segmentData[curSeg].brightness = constrain(segmentData[curSeg].brightness - 1,1,numBrightLevels-1);
        }
        // Update EEPROM with new configuration
        writeSegDataToEeprom(eeAddress);
        break;
    default:
        return false;
    }
    return true;
}

bool checkButtonSpecial(uint32_t buttonPressed){
    switch(buttonPressed){
    // Turn things OFF
    case CTRL2BTN_OFF: // case CTRL2BTN2_OFF:
        if(doAllSegmentsMode){
            if(segmentData[curSeg].curMode!=MODE_OFF){
                changeMode(MODE_OFF,true);
            } else {
                return false;
            }
        } else {
            if(segmentData[curSeg].curMode!=MODE_OFF){
                changeMode(MODE_OFF);
            } else {
                return false;
            }
        }
        break;

    // Turn things ON
    case CTRL2BTN_ON: // case CTRL2BTN2_ON:
        if(doAllSegmentsMode){
            if(segmentData[curSeg].curMode==MODE_OFF){
                for(uint8_t i=0; i < numSegments; i++){
                    changeMode(segmentData[i].prevMode);
                }
            } else {
                return false;
            }
        } else {
            if(segmentData[curSeg].curMode==MODE_OFF){
                changeMode(segmentData[curSeg].prevMode);
            } else {
                return false;
            }
        }
        break;

    // Toggle ON/OFF state
    case CTRL3BTN_ONOFF:
        if(doAllSegmentsMode){
            if(segmentData[curSeg].curMode==MODE_OFF){
                for(uint8_t i=0; i < numSegments; i++){
                    changeMode(segmentData[i].prevMode,i);
                }
            } else {
                changeMode(MODE_OFF,true);
            }
        } else {
            if(segmentData[curSeg].curMode==MODE_OFF){
                changeMode(segmentData[curSeg].prevMode);
            } else {
                changeMode(MODE_OFF);
            }
        }
        break;

	// Change segment
	case CTRL3BTN_ROW01_3:
		if(blinkCurSegment){
			curSeg = (curSeg + 1) % numSegments;
		}
		blinkCurSegment = true;
		blinkCount = 0;
		break;

    case CTRL2BTN_FLASH:  // case CTRL2BTN2_FLASH:
    case CTRL2BTN_STROBE: // case CTRL2BTN2_STROBE:
    case CTRL2BTN_FADE:   // case CTRL2BTN2_FADE:
    case CTRL2BTN_SMOOTH: // case CTRL2BTN2_SMOOTH:
        break;
    default:
        return false;
    }
    return true;
}

bool checkButtonMode(uint32_t buttonPressed){
	return checkButtonMode(buttonPressed, doAllSegmentsMode);
}

bool checkButtonMode(uint32_t buttonPressed, bool doAllSegments){
    switch(buttonPressed){
    case CTRL2BTN_RED: // case CTRL2BTN2_RED:
    case CTRL3BTN_RED:
        changeMode(SOLID_RED,doAllSegments); break;
    case CTRL2BTN_GREEN: // case CTRL2BTN2_GREEN:
    case CTRL3BTN_GREEN:
        changeMode(SOLID_GREEN,doAllSegments); break;
    case CTRL2BTN_BLUE: // case CTRL2BTN2_BLUE:
    case CTRL3BTN_BLUE:
        changeMode(SOLID_BLUE,doAllSegments); break;
    case CTRL2BTN_WHITE: // case CTRL2BTN2_WHITE:
    case CTRL3BTN_WHITE:
        changeMode(SOLID_WHITE,doAllSegments); break;

    case CTRL2BTN_ROW3_1: // case CTRL2BTN2_ROW2_1:
    case CTRL3BTN_ROW03_1:
        changeMode(SOLID_COLOR01,doAllSegments); break;
    case CTRL2BTN_ROW3_2: // case CTRL2BTN2_ROW2_2:
    case CTRL3BTN_ROW03_2:
        changeMode(SOLID_COLOR02,doAllSegments); break;
    case CTRL2BTN_ROW3_3: // case CTRL2BTN2_ROW2_3:
    case CTRL3BTN_ROW03_3:
        changeMode(SOLID_COLOR03,doAllSegments); break;
    case CTRL2BTN_ROW4_1: // case CTRL2BTN2_ROW3_1:
    case CTRL3BTN_ROW04_1:
        changeMode(SOLID_COLOR04,doAllSegments); break;
    case CTRL2BTN_ROW4_2: // case CTRL2BTN2_ROW3_2:
    case CTRL3BTN_ROW04_2:
        changeMode(SOLID_COLOR05,doAllSegments); break;
    case CTRL2BTN_ROW4_3: // case CTRL2BTN2_ROW3_3:
    case CTRL3BTN_ROW04_3:
        changeMode(SOLID_COLOR06,doAllSegments); break;
    case CTRL2BTN_ROW5_1: // case CTRL2BTN2_ROW4_1:
    case CTRL3BTN_ROW05_1:
        changeMode(SOLID_COLOR07,doAllSegments); break;
    case CTRL2BTN_ROW5_2: // case CTRL2BTN2_ROW4_2:
    case CTRL3BTN_ROW05_2:
        changeMode(SOLID_COLOR08,doAllSegments); break;
    case CTRL2BTN_ROW5_3: // case CTRL2BTN2_ROW4_3:
    case CTRL3BTN_ROW05_3:
        changeMode(SOLID_COLOR09,doAllSegments); break;
    case CTRL2BTN_ROW6_1: // case CTRL2BTN2_ROW5_1:
    case CTRL3BTN_ROW06_1:
        changeMode(SOLID_COLOR10,doAllSegments); break;
    case CTRL2BTN_ROW6_2: // case CTRL2BTN2_ROW5_2:
    case CTRL3BTN_ROW06_2:
        changeMode(SOLID_COLOR11,doAllSegments); break;
    case CTRL2BTN_ROW6_3: // case CTRL2BTN2_ROW5_3:
    case CTRL3BTN_ROW06_3:
        changeMode(SOLID_COLOR12,doAllSegments); break;

    case CTRL3BTN_ROW03_4:
        changeMode(SOLID_COLOR13,doAllSegments); break;
    case CTRL3BTN_ROW04_4:
        changeMode(SOLID_COLOR14,doAllSegments); break;
    case CTRL3BTN_ROW05_4:
        changeMode(SOLID_COLOR15,doAllSegments); break;
    case CTRL3BTN_ROW06_4:
        changeMode(SOLID_COLOR16,doAllSegments); break;

    case CTRL3BTN_ROW09_1:
        for(uint8_t seg = 0; seg < numSegments; seg++){
            rainbowIndex[seg] = 0;
        }
        changeMode(MODE1,doAllSegments);
        break;
    case CTRL3BTN_ROW09_2:
        for(uint8_t seg = 0; seg < numSegments; seg++){
            rainbowIndex[seg] = 0;
        }
        changeMode(MODE2,doAllSegments); break;
    case CTRL3BTN_ROW09_3:
        for(uint8_t seg = 0; seg < numSegments; seg++){
            rainbowIndex[seg] = 0;
        }
        changeMode(MODE3,doAllSegments); break;
    case CTRL3BTN_ROW10_1:
        for(uint8_t seg = 0; seg < numSegments; seg++){
            rainbowIndex[seg] = 0;
        }
        changeMode(MODE4,doAllSegments); break;
    case CTRL3BTN_ROW10_2:
        for(uint8_t seg = 0; seg < numSegments; seg++){
            rainbowIndex[seg] = 0;
        }
        changeMode(MODE5,doAllSegments); break;
    case CTRL3BTN_ROW10_3:
        for(uint8_t seg = 0; seg < numSegments; seg++){
            rainbowIndex[seg] = 0;
        }
        changeMode(MODE6,doAllSegments); break;

    case CTRL3BTN_ROW07_4:
        rainbowIncrement += 1;
        rainbowRefreshTime = millis();
        break;
    case CTRL3BTN_ROW08_4:
        if(rainbowIncrement > 1){
            rainbowIncrement -= 1;
        } else {
            rainbowIncrement = 1;
        }
        rainbowRefreshTime = millis();
        break;

    default:
        return false;
    }
    return true;
}

void changeMode(LightMode newMode, uint8_t seg){
    if(segmentData[seg].curMode!=MODE_OFF){
        segmentData[seg].prevMode = segmentData[seg].curMode;
    }
    segmentData[seg].curMode = newMode;

    // Update EEPROM with new configuration
    writeSegDataToEeprom(eeAddress);
}

void changeMode(LightMode newMode){
	if(doAllSegmentsMode){
		changeModeAllSegments(newMode);
	} else {
		changeMode(newMode,curSeg);
	}
}

void changeMode(LightMode newMode, bool doAllSegments){
	if(doAllSegments){
		changeModeAllSegments(newMode);
	} else {
		changeMode(newMode);
	}
}

void changeModeAllSegments(LightMode newMode){
	for(uint8_t i = 0; i < numSegments; i++){
		changeMode(newMode,i);
	}
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

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint16_t start, uint16_t stop) {
    for (uint16_t i = start; i < stop; i++) {
        if(irInterrupt()){return;};
        strip.setPixelColor(i, strip.Color(r, g, b, w));
    }
    strip.show();
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    for (uint16_t i = 0; i < numLeds; i++) {
        if(irInterrupt()){return;};
        strip.setPixelColor(i, strip.Color(r, g, b, w));
    }
    strip.show();
}

void writeRGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w, uint8_t seg){
	if(seg>=numSegments) {return;}
	uint16_t start = segments[seg];
	uint16_t stop  = segments[seg+1];
	writeRGBW(r,g,b,w,start,stop);
}

void writeRGBW(Color c){
	writeRGBW(c, curSeg);
}
void writeRGBW(Color c, uint8_t seg){
    c = adjustInt(c,c.intensity * ((float) brightnessLevels[segmentData[seg].brightness])/255.0);
    c = adjustSat(c,c.sat * ((float) 256-brightnessLevels[numBrightLevels-segmentData[seg].saturation-1])/255.0);
//    PREV = c; // Save the new color to the 'previous' value
//    isManualOn = false;
    writeRGBW(c.red, c.green, c.blue, c.white, seg);
}
void writeHSI(int h, float s, float i) {
	writeHSI(h,s,i,curSeg);
}
void writeHSI(int h, float s, float i, uint8_t seg){
    writeRGBW(Color(h,s,i),seg);
}

void writeRainbow(uint8_t seg){
    writeRainbow(seg, rainbowIndex[seg]);
}
void writeRainbow(uint8_t seg, uint16_t curIdx){
    if(seg>=numSegments) {return;}
    uint16_t start = segments[seg];
    uint16_t stop  = segments[seg+1];
    uint16_t rainbowWidthPxls = 20;

    for (uint16_t i = start; i < stop; i++) {
        if(irInterrupt()){return;};
        float hue;
        switch(segmentData[seg].curMode){
        case MODE1:
        default:
            rainbowWidthPxls = 10;
//            hue = fmod((i * (360 / rainbowWidthPxls)) + curIdx, 360.0);
            hue = biasRed(fmod((i * (360 / rainbowWidthPxls)) + curIdx, 360.0));
            break;
        case MODE2:
            rainbowWidthPxls = segments[0+1]; // one section-ish
//            hue = fmod((i * (360 / rainbowWidthPxls)) + curIdx, 360.0);
            hue = biasRed(fmod((i * (360 / rainbowWidthPxls)) + curIdx, 360.0));
            break;
        case MODE3:
            rainbowWidthPxls = numLeds; // full length
//            hue = fmod((i * (360 / rainbowWidthPxls)) + curIdx, 360.0);
            hue = biasRed(fmod((i * (360 / rainbowWidthPxls)) + curIdx, 360.0));
            break;
        case MODE4:
            rainbowWidthPxls = numLeds; // full length
            hue = fmod((((i * (360 / rainbowWidthPxls)) + curIdx) % 40) + 120, 360);
            break;
        case MODE5:
            rainbowWidthPxls = numLeds; // full length
            hue = fmod((((i * (360 / rainbowWidthPxls)) + curIdx) % 40) + 175, 360);
            break;
        case MODE6:
            rainbowWidthPxls = numLeds; // full length
            hue = fmod((((i * (360 / rainbowWidthPxls)) + curIdx) % 40) + 270, 360);
            break;
        }
        float intensity =  1.0 * ((float) brightnessLevels[segmentData[seg].brightness])/255.0;
        float saturation = 1.0 * ((float) 256-brightnessLevels[numBrightLevels-segmentData[seg].saturation-1])/255.0;
        Color c = Color(hue,saturation,intensity);
        strip.setPixelColor(i, strip.Color(c.red, c.green, c.blue, c.white));
    }
    strip.show();
}

float biasRed(float inputHue){
//    float scaling = 20.0;
//    float factor  = -0.576883720633533;
    float scaling = 50.0;
    float factor  = -0.753332478;

    if(inputHue >= 180.0){
        return fmod(360.0-(360.0-inputHue)/(scaling*pow(360.0-inputHue,factor)),360.0);
    } else {
        return fmod(inputHue/(scaling*pow(inputHue,factor)),360.0);
    }
}

Color adjustSat(Color c, float newSat){
    return Color(c.hue,newSat,c.intensity);
}

Color adjustInt(Color c, float newInt){
    return Color(c.hue,c.sat,newInt);
}

void waitForButton(unsigned long i){
    unsigned long trackTime = millis();
    boolean waitLoop = true;
    while(waitLoop){
        if(irInterrupt()){waitLoop = false;return;};
        if(abs(millis() - trackTime) > i){
            waitLoop = false;
            return;
        }
        delay(0);
    }
}

void writeSegDataToEeprom(int address){
    EEPROM.put(address, segmentData);
}
void readSegDataFromEeprom(int address){
    EEPROM.get(address, segmentData);
}

