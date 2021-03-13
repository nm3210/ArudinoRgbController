// Do not remove the include below
#include "RTC_SetTime.h"

const char *monthName[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// RAM-specific variables to keep track within the interrupts
volatile unsigned long buttonCurTime = 0;
volatile unsigned long buttonLastTime = 0;
volatile boolean changeState = false;

tmElements_t tm;

//The setup function is called once at startup of the sketch
void setup() {
    pinMode(A2,OUTPUT);
    pinMode(A3,OUTPUT);
    digitalWrite(A2,LOW); // Turn on the ground for the DS1307
    digitalWrite(A3,HIGH);// Turn on the +5vcc for the DS1307

    // Enable the AIN0 comparator interrupt (called with the ISR(ANALOG_COMP_vect) function below)
    ACSR = (0 << ACD)  |  // Analog Comparator Enabled
           (0 << ACBG) |  // Analog Comparator Bandgap Select: AIN0 is applied to the positive input
           (0 << ACO)  |  // Analog Comparator Output: Off
           (1 << ACI)  |  // Analog Comparator Interrupt Flag: Clear Pending Interrupt
           (1 << ACIE) |  // Analog Comparator Interrupt: Enabled
           (0 << ACIC) |  // Analog Comparator Input Capture: Disabled
           (1 << ACIS1)| (0 << ACIS0); // Analog Comparator Interrupt Mode: Comparator Interrupt on Falling Output Edge

    Serial.begin(115200);
    while (!Serial)
        ; // wait for Arduino Serial Monitor
    delay(200);

    bool parse = false;
    bool config = false;

    delay(200);

    // get the date and time the compiler was run
    if (getDate(__DATE__) && getTime(__TIME__)) {
        parse = true;
        // and configure the RTC with this info
        if (RTC.write(tm)) {
            config = true;
            setSyncProvider(RTC.get);
            if (timeStatus() != timeSet) {
                Serial.println("Unable to sync with the RTC");
            } else {
                Serial.println("RTC has set the system time");
            }
        }
    }

    delay(200);
    if (parse && config) {
        Serial.print("DS1307 configured Time=");
        Serial.print(__TIME__);
        Serial.print(", Date=");
        Serial.println(__DATE__);
    } else if (parse) {
        Serial.println("DS1307 Communication Error :-{");
        Serial.println("Please check your circuitry");
    } else {
        Serial.print("Could not parse info from the compiler, Time=\"");
        Serial.print(__TIME__);
        Serial.print("\", Date=\"");
        Serial.print(__DATE__);
        Serial.println("\"");
    }
}

// The loop function is called in an endless loop
void loop() {
    if(changeState){
        Serial.println("Adding one (1) second to DS1307 time.");
        long addSomeTime = 1;
        adjustTime(addSomeTime);
        RTC.set(now()); // Try to write the new time to the DC1307
        changeState = false;
    }

//    writeRtcTime(tm);
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

bool getTime(const char *str) {
    int Hour, Min, Sec;

    if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3)
        return false;
    tm.Hour = (Hour + 0) % 24; // timezone offset, not automatic
    tm.Minute = Min;
    tm.Second = Sec + 13; // added to compensate for compile time -> flash time
    return true;
}

bool getDate(const char *str) {
    char Month[12];
    int Day, Year;
    uint8_t monthIndex;

    if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3)
        return false;
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(Month, monthName[monthIndex]) == 0)
            break;
    }
    if (monthIndex >= 12)
        return false;
    tm.Day = Day + 0; // sometimes a '+1' is needed, depending on time of day of flash
    tm.Month = monthIndex + 1; // necessary to take zero-indexed month to proper month
    tm.Year = CalendarYrToTm(Year);
    return true;
}
