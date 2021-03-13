// Do not remove the include below
#include "RTC_ReadTime.h"

// RAM-specific variables to keep track within the interrupts
volatile unsigned long buttonCurTime = 0;
volatile unsigned long buttonLastTime = 0;
volatile boolean changeState = false;

//The setup function is called once at startup of the sketch
void setup() {
    pinMode(A2,OUTPUT);
    pinMode(A3,OUTPUT);
    digitalWrite(A2,LOW); // Turn on the ground for the DS1307
    digitalWrite(A3,HIGH);// Turn on the +5vcc for the DS1307

    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD) |     // Analog Comparator Enabled
            (0 << ACBG) |   // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
            (0 << ACO) |    // Analog Comparator Output: Off
            (1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt
            (1 << ACIE) |   // Analog Comparator Interrupt: Enabled
            (0 << ACIC) |   // Analog Comparator Input Capture: Disabled
            (1 << ACIS1) | (0 << ACIS0); // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

    Serial.begin(115200);
    while (!Serial)
        ; // wait for Arduino Serial Monitor
    delay(200);
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
        Serial.println("Unable to sync with the RTC");
    } else {
        Serial.println("RTC has set the system time");
    }
}

// The loop function is called in an endless loop
void loop() {
//    if(changeState){
//        Serial.println("Adding one (1) second to DC1307 time.");
//        long addSomeTime = 1;
//        adjustTime(addSomeTime);
//        RTC.set(now()); // Try to write the new time to the DS1307
//        changeState = false;
//    }
    digitalClockDisplay();
    delay(100);
}

// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other functions
ISR(ANALOG_COMP_vect) {
    buttonCurTime = millis();
    //check to see if the interrupt was called in the last 250 milliseconds
    if (abs(buttonCurTime - buttonLastTime) > 250) {
        buttonLastTime = buttonCurTime;
        // Call our interrupt function
        btnInterrupt();
    }
}

void btnInterrupt(){
    changeState = true;
}

void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(year());
    Serial.print("-");
    print2digits(month());
    Serial.print("-");
    print2digits(day());
    Serial.print(" ");
    Serial.print(hour());
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
