#include "RgbControllerWithIR.h"

#define IR_PIN   2
#define ledPinR  9
#define ledPinG 10
#define ledPinB  5 // Changed from pin 11 because of a conflict with the IRRemote timer

IRrecv irrecv(IR_PIN);
decode_results results;

// Find the sunrise/sunset at the 'home' location
Dusk2Dawn home(00.0, 00.0, 0);

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240}; // Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};  // Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);

void setup() {
    pinMode(A2,OUTPUT);
    pinMode(A3,OUTPUT);
    digitalWrite(A2,LOW);  // Turn on the ground for the DS1307
    digitalWrite(A3,HIGH); // Turn on the +5vcc for the DS1307

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

    // Enable LED pins
    pinMode(ledPinR, OUTPUT);
    pinMode(ledPinG, OUTPUT);
    pinMode(ledPinB, OUTPUT);

    // Enable IR pin & start receiving data
    irrecv.enableIRIn();

    // Get the current time from the DS1307
    setSyncProvider(RTC.get);
    if (timeStatus() != timeSet) {
        Serial.println("RTC: Unable to sync with the RTC, setting time to: ");
        setTime(00,00,00,01,01,2020);
    } else {
        Serial.print("RTC: Successfully set Arduino time to: ");
    }
    digitalClockDisplay();

    // Set up alarms (must be monotonic)
    timeAlarms[ 0] = calcTOD( 0, 00, 00); // The first alarm should be the earliest time
    timeAlarms[ 1] = calcTOD( 6, 00, 00); //   6:00am
    timeAlarms[ 2] = calcTOD( 7, 30, 00); //   7:30am
    timeAlarms[ 3] = calcTOD( 7, 45, 00); //   7:45am
    timeAlarms[ 4] = calcTOD( 8, 00, 00); //   8:00am
    timeAlarms[ 5] = calcTOD( 8, 30, 00); //   8:30am
    timeAlarms[ 6] = calcTOD(19, 00, 00); //   7:00pm
    timeAlarms[ 7] = calcTOD(19, 30, 00); //   7:30pm
    timeAlarms[ 8] = calcTOD(20, 00, 00); //   8:00pm
    timeAlarms[ 9] = calcTOD(21, 00, 00); //   9:00pm
    timeAlarms[10] = calcTOD(21, 30, 00); //   9:30pm
    timeAlarms[11] = calcTOD(23, 59, 59); // The last alarm should be the latest time

    // Set up sun alarms (relating to sunrise/sunset), these will be configured on startup
    sunAlarms[ 0] = calcTOD( 0, 00, 00); // Night
    sunAlarms[ 1] = calcTOD( 0, 00, 00); // Astronomical Sunrise
    sunAlarms[ 2] = calcTOD( 0, 00, 00); // Nautical Sunrise
    sunAlarms[ 3] = calcTOD( 0, 00, 00); // Civil Sunrise
    sunAlarms[ 4] = calcTOD( 0, 00, 00); // Sunrise (apparent)
    sunAlarms[ 5] = calcTOD( 0, 00, 00); // Day
    sunAlarms[ 6] = calcTOD( 0, 00, 00); // Sunset (apparent)
    sunAlarms[ 7] = calcTOD( 0, 00, 00); // Civil Sunset
    sunAlarms[ 8] = calcTOD( 0, 00, 00); // Nautical Sunset
    sunAlarms[ 9] = calcTOD( 0, 00, 00); // Astronomical Sunset
    sunAlarms[10] = calcTOD( 0, 00, 00); // Night
    sunAlarms[11] = calcTOD(23, 59, 59); // Night

//    Alarm.alarmRepeat(dowWednesday, 12, 00, 00, rtcCorrection); // Correct the clock for a bit

    // Make sure LED is off
    writeRGB(OFF);
}

void loop() {
    // Detect IR and change modes
    handleInterrupt();

    // Handle button press
    if(changeMode){
        changeMode = false;
        doModeOnceFlag = true;
        doAlarmOnceFlag = true;
        count = 0;
        curMode++;
        Serial.print("curMode = "); Serial.println(curMode);
        blinkRGBnTimes(WHITE,2);
    }

    // Switch through different modes
    switch(curMode){
    case offMode :
        // OFF State (which really acts as a method to change color via IR)
        if(doModeOnce()){
            curColorMode = MODE_OFF;
            crossFade(WHITE,OFF,1500);
        }

        switch(curColorMode){
        case MODE_OFF:
            writeRGB(OFF);
            break;
        case MANUAL_COLOR:
            writeRGB(MANUAL);
            break;
        default: // All other colors (see @changeColorMode)
            writeRGB(controlColor);
            break;
        }
        break;

    case alarmMode :
        // Alarm State
        if(doModeOnce()){
            crossFadeTo(WHITE,200);
            crossFadeTo(RED,200);
            crossFadeTo(YELLOW,200);
            crossFadeTo(GREEN,200);
            crossFadeTo(OFF,200);
            waitForButton(1000);
        }
        if(doAlarmOnce()){
            if (doSunAlarm){
//                sunAlarms[ 1] = calcTOD( 0, 00, 00); // Astronomical Sunrise
//                sunAlarms[ 2] = calcTOD( 0, 00, 00); // Nautical Sunrise
//                sunAlarms[ 3] = calcTOD( 0, 00, 00); // Civil Sunrise
//                sunAlarms[ 4] = calcTOD( 0, 00, 00); // Sunrise (apparent)
//                sunAlarms[ 5] = calcTOD( 0, 00, 00); // Day
//                sunAlarms[ 6] = calcTOD( 0, 00, 00); // Sunset (apparent)
//                sunAlarms[ 7] = calcTOD( 0, 00, 00); // Civil Sunset
//                sunAlarms[ 8] = calcTOD( 0, 00, 00); // Nautical Sunset
//                sunAlarms[ 9] = calcTOD( 0, 00, 00); // Astronomical Sunset
                switch(curSunAlarm){
                case 6: // sunAlarms[ 6] = calcTOD( 0, 00, 00); // Sunset (apparent)
                    crossFadeTo(adjustInt(ORANGE,0.5),1000.0*30.0);
                    break;
                }
            } else {
//                timeAlarms[ 0] = calcTOD( 0, 00, 00); // The first alarm should be the earliest time
//                timeAlarms[ 1] = calcTOD( 6, 00, 00); //   6:00am
//                timeAlarms[ 2] = calcTOD( 7, 30, 00); //   7:30am
//                timeAlarms[ 3] = calcTOD( 7, 45, 00); //   7:45am
//                timeAlarms[ 4] = calcTOD( 8, 00, 00); //   8:00am
//                timeAlarms[ 5] = calcTOD( 8, 30, 00); //   8:30am
//                timeAlarms[ 6] = calcTOD(19, 00, 00); //   7:00pm
//                timeAlarms[ 7] = calcTOD(19, 30, 00); //   7:30pm
//                timeAlarms[ 8] = calcTOD(20, 00, 00); //   8:00pm
//                timeAlarms[ 9] = calcTOD(21, 00, 00); //   9:00pm
//                timeAlarms[10] = calcTOD(21, 30, 00); //   9:30pm
//                timeAlarms[11] = calcTOD(23, 59, 59); // The last alarm should be the latest time
                switch(curAlarm){
                case 1: // timeAlarms[ 1] = calcTOD( 6, 00, 00); //   6:00am
                    crossFadeTo(adjustInt(RED,0.50),1000.0*30.0);
                    break;
                case 2: // timeAlarms[ 2] = calcTOD( 7, 30, 00); //   7:30am
                    crossFadeTo(adjustInt(YELLOW,0.50),1000.0*30.0);
                    break;
                case 3: // timeAlarms[ 3] = calcTOD( 7, 45, 00); //   7:45am
                    crossFadeTo(adjustInt(GREEN,0.75),1000.0*10.0);
                    break;
                case 5: // timeAlarms[ 5] = calcTOD( 8, 30, 00); //   8:30am
                    crossFadeTo(OFF,1000.0*90.0);
                    break;
                case 6: // timeAlarms[ 6] = calcTOD(19, 00, 00); //   7:00pm
                    crossFadeTo(adjustInt(controlColor,0.75),1000.0*30.0);
                    break;
                case 8: // timeAlarms[ 8] = calcTOD(20, 00, 00); //   8:00pm
                    crossFadeTo(adjustInt(controlColor,0.5),1000.0*30.0);
                    break;
                case  0: // timeAlarms[ 0] = calcTOD( 0, 00, 00);
                case  9: // timeAlarms[ 9] = calcTOD(21, 00, 00); //   9:00pm
                case 10: // timeAlarms[10] = calcTOD(21, 30, 00); //   9:30pm
                case 11: // timeAlarms[11] = calcTOD(23, 59, 59);
                    crossFadeTo(adjustInt(controlColor,0.25),1000.0*30.0);
                    break;
                default:
                    break;
                }
            }
        }
        break;

    case 2 :
        writeHSI(wrapCount(360),1.0,1.0); Alarm.delay(25);
        break;

    case 3 :
        writeHSI(loopCount(120)+120,1.0,1.0); Alarm.delay(25);
        break;

    default:
        curMode = 0; // Loop back
        break;
    }

    // Update sun alarm times based on the sunrise/sunset
    if(sunriseUpdateTime==0 || abs(millis() - sunriseUpdateTime) > SUNRISECHECK_TIMEOUT){
        sunriseUpdateTime = millis();

        // Grab current time and adjust for timezone and daylight savings
        time_t local = myTZ.toLocal(now());
        int curYear  = year(local);
        int curMonth = month(local);
        int curDay   = day(local);

        // Calculate sunrise/sunset times
        curSunrise = home.sunrise(curYear, curMonth, curDay, myTZ.locIsDST(local)) + 180;
        curSunset  = home.sunset (curYear, curMonth, curDay, myTZ.locIsDST(local)) + 180;

        Serial.print("Updated sunrise time (min past midnight): ");
        Serial.println(curSunrise);
        Serial.print("Updated sunset time (min past midnight): ");
        Serial.println(curSunset);

        // Adjust alarm times based on sunrise/sunset
        sunAlarms[ 0] = calcTOD(00, 00, 00); // Night
        sunAlarms[ 1] = calcTOD((curSunrise-90) / 60, (curSunrise-90) % 60, 00); // Astronomical Sunrise
        sunAlarms[ 2] = calcTOD((curSunrise-60) / 60, (curSunrise-60) % 60, 00); // Nautical Sunrise
        sunAlarms[ 3] = calcTOD((curSunrise-30) / 60, (curSunrise-30) % 60, 00); // Civil Sunrise
        sunAlarms[ 4] = calcTOD((curSunrise-00) / 60, (curSunrise-00) % 60, 00); // Sunrise (apparent)
        sunAlarms[ 5] = calcTOD((curSunrise+30) / 60, (curSunrise+30) % 60, 00); // Day
        sunAlarms[ 6] = calcTOD((curSunset+00)  / 60, (curSunset+00)  % 60, 00); // Sunset (apparent)
        sunAlarms[ 7] = calcTOD((curSunset+30)  / 60, (curSunset+30)  % 60, 00); // Civil Sunset
        sunAlarms[ 8] = calcTOD((curSunset+60)  / 60, (curSunset+60)  % 60, 00); // Nautical Sunset
        sunAlarms[ 9] = calcTOD((curSunset+90)  / 60, (curSunset+90)  % 60, 00); // Astronomical Sunset
        sunAlarms[10] = calcTOD((curSunset+120)  / 60, (curSunset+120)  % 60, 00); // Night
        sunAlarms[11] = calcTOD(23, 59, 59); // Night
    }

    // Check alarm times versus the current time
    if(abs(millis() - alarmCheckTime) > ALARMCHECK_TIMEOUT){
        alarmCheckTime = millis();

        // Grab current time and adjust for timezone and daylight savings
        time_t local = myTZ.toLocal(now());

        // Convert to time of day
        long curTime = calcTOD(hour(local),minute(local),second(local));

        // Loop over all alarms (minus one, for the comparison)
        for(int i = 0; i<NUMALARMS-1; i++){
            // Check if our current time is between a set of alarms
            if(timeAlarms[i] < curTime && curTime < timeAlarms[i+1] && curAlarm!=i){
                curAlarm = i;
                doAlarmOnceFlag = true;
                doSunAlarm = false;
            }
        }

        // Also check all the sun alarm timers
        for(int i = 0; i<NUMSUNALARMS-1; i++){
            if(sunAlarms[i] < curTime && curTime < sunAlarms[i+1] && curSunAlarm!=i){
                curSunAlarm = i;

                // Select which alarms should trigger the flag, this makes it easy to only call sunset/night times
//                sunAlarms[ 0] = calcTOD( 0, 00, 00); // Night
//                sunAlarms[ 1] = calcTOD( 0, 00, 00); // Astronomical Sunrise
//                sunAlarms[ 2] = calcTOD( 0, 00, 00); // Nautical Sunrise
//                sunAlarms[ 3] = calcTOD( 0, 00, 00); // Civil Sunrise
//                sunAlarms[ 4] = calcTOD( 0, 00, 00); // Sunrise (apparent)
//                sunAlarms[ 5] = calcTOD( 0, 00, 00); // Day
//                sunAlarms[ 6] = calcTOD( 0, 00, 00); // Sunset (apparent)
//                sunAlarms[ 7] = calcTOD( 0, 00, 00); // Civil Sunset
//                sunAlarms[ 8] = calcTOD( 0, 00, 00); // Nautical Sunset
//                sunAlarms[ 9] = calcTOD( 0, 00, 00); // Astronomical Sunset
//                sunAlarms[10] = calcTOD( 0, 00, 00); // Night
//                sunAlarms[11] = calcTOD(23, 59, 59); // Night
                switch(curSunAlarm){
                case 7: // sunAlarms[ 7] = calcTOD( 0, 00, 00); // Civil Sunset
                    if(curAlarm<8){ // timeAlarms[ 8] = calcTOD(19, 00, 00); //   7:00pm
                        doSunAlarm = true;
                        doAlarmOnceFlag = true;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Print out current time (once every second)
    if(abs(millis() - displayClockTime) > CLOCKDISPLAY_TIMEOUT){
        displayClockTime = millis();

        // Grab current time and adjust for timezone and daylight savings
        time_t utc = now();
        time_t local = myTZ.toLocal(utc);

        digitalClockDisplay(local);
    }

    // This Alarm.delay is needed within the loop() function in order to keep the timers/alarms alive and working
    Alarm.delay(0);
}

// Function for the AIN0 interrupt; need to keep all the variables as volatile, can't really call other [complex] functions
ISR(ANALOG_COMP_vect) {
    interruptCurTime = millis();
    // Check to see if the interrupt was called in the last 250 milliseconds
    if (abs(interruptCurTime - interruptLastTime) > 250) {
        interruptLastTime = interruptCurTime;
        changeMode = true;
    }
}


// Functions
bool handleInterrupt(){
    bool changedModes = false;

    unsigned long tempButtonPress = decodeIrSignal();
    if(!checkRepeatBtn(tempButtonPress)){
        lastButtonPressed = tempButtonPress;
    }

    // Check for valid button press (meets one of our cases)
    if(lastButtonPressed!=0){
        changedModes |= checkModesSpecial();
        checkBrightnessChange();
        changedModes |= checkModes();
        lastButtonSave = lastButtonPressed;
    }

    if(changedModes && curColorMode!=prevColorMode && curColorMode!=MODE_OFF){
        Serial.print("curColorMode = "); Serial.println(curColorMode);
    }

    return changedModes;
}

long decodeIrSignal(){
    uint32_t tempResults = 0;
    while (!irrecv.isIdle());
    if (irrecv.decode(&results)) {
        tempResults = results.value;
        Serial.println(results.value, HEX); // Print the value
        // Restart the ir receiver state
        irrecv.resume();
    }
    return tempResults;
}

bool checkBrightnessChange(){
    switch(lastButtonPressed){
//    case CTRL1BTN_UP: case CTRL1BTN2_UP:
    case CTRL2BTN_BRIGHTUP: // case CTRL2BTN2_BRIGHTUP:
    case CTRL3BTN_BRIGHTUP:
    case CTRL4BTN_VOLUP:
        // Do bright increase
        adjustBrightnessVal = constrain(adjustBrightnessVal + 1,1,numBrightLevels-1);
        break;
//    case CTRL1BTN_DOWN: case CTRL1BTN2_DOWN:
    case CTRL2BTN_BRIGHTDOWN: // case CTRL2BTN2_BRIGHTDOWN:
    case CTRL3BTN_BRIGHTDOWN:
    case CTRL4BTN_VOLDOWN:
        // Do bright decrease
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
        if(curColorMode!=MODE_OFF){
            changeColorMode(MODE_OFF);
        } else {
            return false;
        }
        break;

    // Turn things ON
    case CTRL2BTN_ON: // case CTRL2BTN2_ON:
        if(curColorMode==MODE_OFF){
            changeColorMode(prevColorMode);
        } else {
            return false;
        }
        break;

    // Toggle ON/OFF state
    case CTRL3BTN_ONOFF:
        if(curColorMode==MODE_OFF){
            changeColorMode(prevColorMode);
        } else {
            changeColorMode(MODE_OFF);
        }
        break;

    // Change mode
    case CTRL3BTN_ROW01_3:
        changeMode = true;
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
        changeColorMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW07_2: // Green UP
        checkForManualColor();
        manualGreenIndex = constrain(manualGreenIndex + 1,0,numBrightLevels-1);
        MANUAL[1] = brightnessLevels[manualGreenIndex];
        changeColorMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW07_3: // Blue UP
        checkForManualColor();
        manualBlueIndex = constrain(manualBlueIndex + 1,0,numBrightLevels-1);
        MANUAL[2] = brightnessLevels[manualBlueIndex];
        changeColorMode(MANUAL_COLOR);
        break;

    case CTRL3BTN_ROW08_1: // Red DOWN
        checkForManualColor();
        manualRedIndex = constrain(manualRedIndex - 1,0,numBrightLevels-1);
        MANUAL[0] = brightnessLevels[manualRedIndex];
        changeColorMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW08_2: // Green DOWN
        checkForManualColor();
        manualGreenIndex = constrain(manualGreenIndex - 1,0,numBrightLevels-1);
        MANUAL[1] = brightnessLevels[manualGreenIndex];
        changeColorMode(MANUAL_COLOR);
        break;
    case CTRL3BTN_ROW08_3: // Blue DOWN
        checkForManualColor();
        manualBlueIndex = constrain(manualBlueIndex - 1,0,numBrightLevels-1);
        MANUAL[2] = brightnessLevels[manualBlueIndex];
        changeColorMode(MANUAL_COLOR);
        break;
    default:
        return false;
    }
    return true;
}

bool checkModes(){
    switch(lastButtonPressed){
    case CTRL2BTN_RED: // case CTRL2BTN2_RED:
    case CTRL3BTN_RED:
        changeColorMode(SOLID_RED); break;
    case CTRL2BTN_GREEN: // case CTRL2BTN2_GREEN:
    case CTRL3BTN_GREEN:
        changeColorMode(SOLID_GREEN); break;
    case CTRL2BTN_BLUE: // case CTRL2BTN2_BLUE:
    case CTRL3BTN_BLUE:
        changeColorMode(SOLID_BLUE); break;
    case CTRL2BTN_WHITE: // case CTRL2BTN2_WHITE:
    case CTRL3BTN_WHITE:
        changeColorMode(SOLID_WHITE); break;

    case CTRL2BTN_ROW3_1: // case CTRL2BTN2_ROW2_1:
    case CTRL3BTN_ROW03_1:
        changeColorMode(SOLID_COLOR01); break;
    case CTRL2BTN_ROW3_2: // case CTRL2BTN2_ROW2_2:
    case CTRL3BTN_ROW03_2:
        changeColorMode(SOLID_COLOR02); break;
    case CTRL2BTN_ROW3_3: // case CTRL2BTN2_ROW2_3:
    case CTRL3BTN_ROW03_3:
        changeColorMode(SOLID_COLOR03); break;
    case CTRL2BTN_ROW4_1: // case CTRL2BTN2_ROW3_1:
    case CTRL3BTN_ROW04_1:
        changeColorMode(SOLID_COLOR04); break;
    case CTRL2BTN_ROW4_2: // case CTRL2BTN2_ROW3_2:
    case CTRL3BTN_ROW04_2:
        changeColorMode(SOLID_COLOR05); break;
    case CTRL2BTN_ROW4_3: // case CTRL2BTN2_ROW3_3:
    case CTRL3BTN_ROW04_3:
        changeColorMode(SOLID_COLOR06); break;
    case CTRL2BTN_ROW5_1: // case CTRL2BTN2_ROW4_1:
    case CTRL3BTN_ROW05_1:
        changeColorMode(SOLID_COLOR07); break;
    case CTRL2BTN_ROW5_2: // case CTRL2BTN2_ROW4_2:
    case CTRL3BTN_ROW05_2:
        changeColorMode(SOLID_COLOR08); break;
    case CTRL2BTN_ROW5_3: // case CTRL2BTN2_ROW4_3:
    case CTRL3BTN_ROW05_3:
        changeColorMode(SOLID_COLOR09); break;
    case CTRL2BTN_ROW6_1: // case CTRL2BTN2_ROW5_1:
    case CTRL3BTN_ROW06_1:
        changeColorMode(SOLID_COLOR10); break;
    case CTRL2BTN_ROW6_2: // case CTRL2BTN2_ROW5_2:
    case CTRL3BTN_ROW06_2:
        changeColorMode(SOLID_COLOR11); break;
    case CTRL2BTN_ROW6_3: // case CTRL2BTN2_ROW5_3:
    case CTRL3BTN_ROW06_3:
        changeColorMode(SOLID_COLOR12); break;

    case CTRL3BTN_ROW03_4:
        changeColorMode(SOLID_COLOR13); break;
    case CTRL3BTN_ROW04_4:
        changeColorMode(SOLID_COLOR14); break;
    case CTRL3BTN_ROW05_4:
        changeColorMode(SOLID_COLOR15); break;
    case CTRL3BTN_ROW06_4:
        changeColorMode(SOLID_COLOR16); break;

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
        case CTRL3BTN_ROW08_1: // Red DOWN
        case CTRL3BTN_ROW08_2: // Green DOWN
        case CTRL3BTN_ROW08_3: // Blue DOWN
//            Serial.print("Repeat! Using last button press: ");
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
        hsi2rgb(PREV, MANUAL);
        manualRedIndex   = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[0]);
        manualGreenIndex = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[1]);
        manualBlueIndex  = findClosestIndex(brightnessLevels, numBrightLevels, MANUAL[2]);
    }
    isManualOn = true;
}

void changeColorMode(LightMode newMode){
    if(curColorMode!=MODE_OFF){
        prevColorMode = curColorMode;
    }
    curColorMode = newMode;

    // Update the manual colors
    switch(curColorMode){
        case SOLID_RED:
            controlColor = (RED); break;
        case SOLID_GREEN:
            controlColor = (GREEN); break;
        case SOLID_BLUE:
            controlColor = (BLUE); break;
        case SOLID_WHITE:
            controlColor = (WHITE); break;
        case SOLID_CYAN:
            controlColor = (CYAN); break;
        case SOLID_YELLOW:
            controlColor = (YELLOW); break;
        case SOLID_MAGENTA:
            controlColor = (MAGENTA); break;

        case SOLID_COLOR01:
            controlColor = Color( 10,1.0,1.0); break;
        case SOLID_COLOR04:
            controlColor = Color( 20,1.0,1.0); break;
        case SOLID_COLOR07:
            controlColor = Color( 30,1.0,1.0); break;
        case SOLID_COLOR10:
            controlColor = Color( 40,1.0,1.0); break;

        case SOLID_COLOR02:
            controlColor = Color(130,1.0,1.0); break;
        case SOLID_COLOR05:
            controlColor = Color(147,1.0,1.0); break;
        case SOLID_COLOR08:
            controlColor = Color(163,1.0,1.0); break;
        case SOLID_COLOR11:
            controlColor = Color(180,1.0,1.0); break;

        case SOLID_COLOR03:
            controlColor = Color(260,1.0,1.0); break;
        case SOLID_COLOR06:
            controlColor = Color(280,1.0,1.0); break;
        case SOLID_COLOR09:
            controlColor = Color(300,1.0,1.0); break;
        case SOLID_COLOR12:
            controlColor = Color(320,1.0,1.0); break;

        case SOLID_COLOR13:
            controlColor = Color(330,1.0,1.0); break;
        case SOLID_COLOR14:
            controlColor = Color(345,1.0,1.0); break;
        case SOLID_COLOR15:
            controlColor = Color(200,1.0,1.0); break;
        case SOLID_COLOR16:
            controlColor = Color(220,1.0,1.0); break;

        default:
            break;
    }
}

//void rtcCorrection(){
//    digitalClockDisplay();
//    Serial.print("rtcCorrection: ");
//    // Correct the clock
//    long singleWeekDelta = (long) -offsetSecPerYear / 365 * 7;
//    Serial.print(singleWeekDelta);
//    adjustTime(singleWeekDelta);
//    // Re-set the time
//    RTC.set(now());
//    Serial.println(" Done.");
//    digitalClockDisplay();
//}

void digitalClockDisplay(){
    digitalClockDisplay(now());
}

void digitalClockDisplay(time_t curTime){
    // Digital clock display of the time
    Serial.print(year(curTime));
    Serial.print("-");
    print2digits(month(curTime));
    Serial.print("-");
    print2digits(day(curTime));
    Serial.print(" ");
    print2digits(hour(curTime));
    Serial.print(":");
    print2digits(minute(curTime));
    Serial.print(":");
    print2digits(second(curTime));
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

bool doAlarmOnce(){
    if(doAlarmOnceFlag){
        doAlarmOnceFlag = false;
        return true;
    } else {
        return false;
    }
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

void writeRGB(int r, int g, int b) {
    // Write the pin values out (this assumes high is off, and low is on)
    analogWrite(ledPinR, 255-r);
    analogWrite(ledPinG, 255-g);
    analogWrite(ledPinB, 255-b);
}

void hsi2rgb(Color c, int* rgb) {
    hsi2rgb(c.hue,c.sat,c.intensity,rgb);
}

void hsi2rgb(int h, float Sat, float Inten, int* rgb) {
    // From: http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi
    int r, g, b; float Hue;
    Hue = fmod(h,360); // Cycle H around to 0-360 degrees
    Hue = M_PI*Hue/(float)180; // Convert to radians.
    Sat = Sat>0?(Sat<1?Sat:1):0; // Clamp S and I to interval [0,1]
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
    rgb[0]=r>0?(r<255?r:255):0;
    rgb[1]=g>0?(g<255?g:255):0;
    rgb[2]=b>0?(b<255?b:255):0;
}

void writeHSI(int h, float s, float i) {
    writeRGB(Color(h,s,i));
}

void writeRGB(int rgb[]){
    writeRGB(rgb[0],rgb[1],rgb[2]);
}

void writeRGB(Color c){
    switch(curMode){
    case offMode:
    case 2:
    case 3:
        c = adjustInt(c,c.intensity * ((float) brightnessLevels[adjustBrightnessVal])/255.0);
        c = adjustSat(c,c.sat * ((float) 256-brightnessLevels[numBrightLevels-adjustSaturationVal-1])/255.0);
        break;
    default:
        break;
    }
    isManualOn = false;
    PREV = c; // Save the new color to the 'previous' value
    writeRGB(c.red, c.green, c.blue);
}

void blinkRGB(Color c){
    blinkRGB(c,25);
}

void blinkRGB(Color c, int d){
    writeRGB(c);
    Alarm.delay(d);
    writeRGB(OFF);
    Alarm.delay(d);
}

void blinkRGBnTimes(Color c, int count){
    for(int i=0;i<count;i++){
        blinkRGB(c);
    }
}

void crossFadeHSI(int h1, float s1, float i1,
                  int h2, float s2, float i2, long steps, int dur){
    if(changeMode){writeRGB(OFF);return;}; // Check for button press
    if(handleInterrupt()){return;};
    float rho1 = i1; //(i1!=0)?i1:LOWERLIMIT;
    float rho2 = i2; //(i2!=0)?i2:LOWERLIMIT;

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
        if(changeMode){writeRGB(OFF);return;}; // Check for button press (so you can escape the loop)
        if(handleInterrupt()){return;};

        float tempRho = length*((i-1.0)/(steps-1.0)); // Calculate one part of the line

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

        float newSat = s1+(s2-s1)*((i-1.0)/(steps-1.0)); // Simple interpolation
        writeHSI(newTheta, newSat, newRho);
        Alarm.delay(dur);
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
    int maxDelay = 5; // Set max delay to 5ms (wiggle room)
    if(msec<maxDelay){return;};
    int timeStep = max(maxDelay, msec / 100); // Default to 100 steps (pretty smooth)
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
        if(changeMode){writeRGB(OFF);waitLoop = false;return;};
        if(handleInterrupt()){waitLoop = false;return;};
        if(abs(millis() - trackTime) > i){
            waitLoop = false;
            return;
        }
        Alarm.delay(0);
    }
}

int findClosestIndex(uint8_t values[], uint8_t size, uint8_t find) {
    int dist = abs(values[0] - find); // Calculating first difference
    int closest = 0;
    int i;
    for (i = 1; i < size; ++i) {
        if (abs(values[i] - find) < dist) {  // Checking for closest value
            dist = abs(values[i] - find);    // Saving closest value in dist
            closest = i;                     // Saving the position of the closest value
        }
    }
    return closest;
}

long calcTOD(int hr, int min, int sec){
    // Calculate the Time Of Day, in seconds, since midnight
    return hr * SECS_PER_HOUR + min * SECS_PER_MIN + sec;
}

Color adjustSat(Color c, float newSat){
    return Color(c.hue,newSat,c.intensity);
}

Color adjustInt(Color c, float newInt){
    return Color(c.hue,c.sat,newInt);
}
