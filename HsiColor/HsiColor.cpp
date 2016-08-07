// Do not remove the include below
#include "HsiColor.h"

const int numModes = 11;

//The setup function is called once at startup of the sketch
void setup() {
    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD)   |  // Analog Comparator Enabled
           (0 << ACBG)  |  // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
           (0 << ACO)   |  // Analog Comparator Output: Off
           (1 << ACI)   |  // Analog Comparator Interrupt Flag: Clear Pending Interrupt
           (1 << ACIE)  |  // Analog Comparator Interrupt: Enabled
           (0 << ACIC)  |  // Analog Comparator Input Capture: Disabled
           (1 << ACIS1) |
           (0 << ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

    Serial.begin(115200);
    Serial.println("Starting Arduino...");
    writeRGB(OFF);
}

// The loop function is called in an endless loop
void loop() {


    if(curMode == alarmState){
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        // Alarm state, leave blank to let the alarms turn on
//        writeRGB(OFF);

    	hsi_Hue = (count+=1)%360;
        hsi2rgb(hsi_Hue,hsi_Saturation,hsi_Intensity, rgb);
        writeRGB(rgb);
        delay(2);

    } else if (curMode == 1) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(WHITE);
    } else if (curMode == 2) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(RED);
    } else if (curMode == 3) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(GREEN);
    } else if (curMode == 4) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(BLUE);
    } else if (curMode == 5) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(CYAN);
    } else if (curMode == 6) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(YELLOW);
    } else if (curMode == 7) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(MAGENTA);
    } else if (curMode == 8) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(ORANGE);
    } else if (curMode == 9) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(PINK);
    } else if (curMode == 10) {
        if(changeMode){blinkRGB(OFF);changeMode=false;};
        writeRGB(PURPLE);
    }
}


// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other functions
ISR(ANALOG_COMP_vect) {
    buttonCurTime = millis();
    //check to see if the interrupt was called in the last 250 milliseconds
    if (abs(buttonCurTime - buttonLastTime) > 250) {
        changeMode = true;
        buttonLastTime = buttonCurTime;
        curMode = (curMode+1) % numModes;
    }
}

void writeRGB(int r, int g, int b) {
    redVal = r;//convertColor(r);
    grnVal = g;//convertColor(g);
    bluVal = b;//convertColor(b);

    // Write the pin values out (this assumes high is off, and low is on);
    analogWrite(ledPinR,   255-redVal);
    analogWrite(ledPinG, 255-grnVal);
    analogWrite(ledPinB,  255-bluVal);
}

void writeRGB(int rgb[]){
    writeRGB(rgb[0],rgb[1],rgb[2]);
}

void writeRGB(Color c){
    writeRGB(c.red,c.green,c.blue);
}

int convertColor(int c){
	return ceil((float) exp(c * expConvert) - 1);
}

void hsi2rgb(float Hue, float Sat, float Inten, int* rgb) {
  int r, g, b;
  Hue = fmod(Hue,360); // cycle H around to 0-360 degrees
  Hue = M_PI*Hue/(float)180; // Convert to radians.
  Sat = Sat>0?(Sat<1?Sat:1):0; // clamp S and I to interval [0,1]
  Inten = Inten>0?(Inten<2?Inten:2):0;

  // Math! Thanks in part to Kyle Miller.
  if(Hue < M_2PI3) { // First third
    r = 255*Inten/3*(1+Sat*(  cos(Hue)/cos(M_1PI3-Hue)));
    g = 255*Inten/3*(1+Sat*(1-cos(Hue)/cos(M_1PI3-Hue)));
    b = 255*Inten/3*(1-Sat);
  } else if(Hue < M_4PI3) { // Second third
    Hue = Hue - M_2PI3;
    r = 255*Inten/3*(1-Sat);
    g = 255*Inten/3*(1+Sat*(  cos(Hue)/cos(M_1PI3-Hue)));
    b = 255*Inten/3*(1+Sat*(1-cos(Hue)/cos(M_1PI3-Hue)));
  } else { // Third section
    Hue = Hue - M_4PI3;
    r = 255*Inten/3*(1+Sat*(1-cos(Hue)/cos(M_1PI3-Hue)));
    g = 255*Inten/3*(1-Sat);
    b = 255*Inten/3*(1+Sat*(  cos(Hue)/cos(M_1PI3-Hue)));
  }
  rgb[0]=r>0?(r<255?r:255):0;//r;
  rgb[1]=g>0?(g<255?g:255):0;//g;
  rgb[2]=b>0?(b<255?b:255):0;//b;
}

int *hsi2rgb(float H, float S, float I) {
	hsi2rgb(H,S,I,rgb);
	return rgb;
}
void blinkRGB(Color c){
    writeRGB(c);
    delay(25);
    writeRGB(OFF);
    delay(25);
}
