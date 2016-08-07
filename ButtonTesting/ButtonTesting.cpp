// Do not remove the include below
#include "ButtonTesting.h"

const int inputPin = 6; // AIN0 pin

//The setup function is called once at startup of the sketch
void setup() {

    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD)   |  // Analog Comparator Enabled
           (0 << ACBG)  |  // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
           (0 << ACO)   |  // Analog Comparator Output: Off
           (1 << ACI)   |  // Analog Comparator Interrupt Flag: Clear Pending Interrupt
           (1 << ACIE)  |  // Analog Comparator Interrupt: Enabled
           (0 << ACIC)  |  // Analog Comparator Input Capture: Disabled
           (0 << ACIS1) | (0 << ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on Either Edge

    // Start the AIN0 pin set to input
    pinMode(inputPin,INPUT);

    Serial.begin(115200);
    Serial.println("Starting Arduino...");
    writeRGB(0,0,0);
}

// The loop function is called in an endless loop
void loop() {
	// Check for our current button state
	if(changeMode){
		curClickState = checkBtnState();
		switch(curClickState){
		case CLICK_NULL:
			break;
		case CLICK_SINGLE:
			Serial.print("Button Press: CLICK_SINGLE @ ");
			Serial.println(millis());
			changeMode = false;
			writeRGB(255,0,0);
			break;
		case CLICK_LONG:
			Serial.print("Button Press: CLICK_LONG @ ");
			Serial.println(millis());
			changeMode = false;
			writeRGB(0,0,255);
			break;
		}
	}
	delay(10);
}

// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other functions
ISR(ANALOG_COMP_vect) {
	buttonLastTime = buttonCurTime;
    buttonCurTime = millis();
    buttonCurState = digitalRead(inputPin);
    changeMode = true;
}

clickState checkBtnState(){
	clickState ClickState = CLICK_NULL;
	long curTime = millis();
    // Button goes from Up to Down
	if((buttonCurState == BTN_DOWN) && ((buttonCurTime - buttonLastTime) > DEBOUNCE)){
		buttonDownTime = buttonCurTime;
		buttonClickIgnoreSingle = false;
		buttonClickIgnoreLong = false;
	}
	// Button goes from Down to Up (single click)
	if((buttonCurState == BTN_UP) && !buttonClickIgnoreSingle && ((buttonCurTime - buttonUpTime) > DEBOUNCE)){
		buttonUpTime = buttonCurTime;
		buttonClickIgnoreLong = true;
		ClickState = CLICK_SINGLE;
	}
	// Check for long press (Down + time)
	if (buttonCurState == BTN_DOWN && ((curTime - buttonDownTime) > LONGPRESS_HOLDTIME) && (buttonDownTime != 0) && ((buttonCurTime - buttonLastTime) > DEBOUNCE) && !buttonClickIgnoreLong){
		buttonClickIgnoreSingle = true;
		ClickState = CLICK_LONG;
	}
	return ClickState;
}

void writeRGB(int r, int g, int b) {
    // Write the pin values out (this assumes high is off, and low is on);
    analogWrite(ledPinR, 255-r);
    analogWrite(ledPinG, 255-g);
    analogWrite(ledPinB, 255-b);
}
