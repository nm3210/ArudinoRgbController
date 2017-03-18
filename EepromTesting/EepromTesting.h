#ifndef _EepromTesting_H_
#define _EepromTesting_H_
#include "Arduino.h"
#include "EEPROM.h"

#define numBrightLevels 16

// LightMode enum
enum LightMode {
    MODE_OFF, MANUAL_COLOR,
    SOLID_RED, SOLID_GREEN, SOLID_BLUE, SOLID_WHITE, SOLID_CYAN, SOLID_YELLOW, SOLID_MAGENTA,
    SOLID_COLOR01, SOLID_COLOR02, SOLID_COLOR03, SOLID_COLOR04,
    SOLID_COLOR05, SOLID_COLOR06, SOLID_COLOR07, SOLID_COLOR08,
    SOLID_COLOR09, SOLID_COLOR10, SOLID_COLOR11, SOLID_COLOR12,
    SOLID_COLOR13, SOLID_COLOR14, SOLID_COLOR15, SOLID_COLOR16,
    MODE1, MODE2, MODE3, MODE4, MODE5, MODE6
};

// Set up segment data structure
struct SegmentData {
    uint8_t brightness;
    uint8_t saturation;
    LightMode curMode;
    LightMode prevMode;

    void initialize(){
        brightness = 1;
        saturation = numBrightLevels-1;
        curMode = MODE_OFF;
        prevMode = MODE_OFF;
    }
};

// Create class for saving and storing the EEPROM data
class EepromData {
public:
    EepromData(uint8_t offset, uint8_t numSegs, SegmentData* seg,  uint8_t* curSeg){
        offsetAddress = offset;
        numSegments = numSegs;
        segData = seg;
        currentSegment = curSeg;
    };

    static void printAll(){
        Serial.printf("EEPROM Data (%d bytes): 0x ",EEPROM.length());
        byte tempVal;
        for(int i=0;i<EEPROM.length();i++){
            tempVal = EEPROM.read(i);
            Serial.printf("%02X ",tempVal);
        }
        Serial.println();
    }

    static void clearAll(){
        Serial.printf("Resetting all EEPROM data (%d bytes) to 'FF'...",EEPROM.length());
        for(int i=0;i<EEPROM.length();i++){
            EEPROM.write(i,0xFF);
        }
        Serial.printf("done.\n");
    }

    void saveHeader(){
        uint8_t offset = offsetAddress;
        EEPROM.put(offset, *currentSegment);
    }
    void saveBrightness(uint8_t segNum){
        uint8_t fieldLocation = 0; // Location of brightness field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.put(offset, segData[segNum].brightness);
    }
    void saveSaturation(uint8_t segNum){
        uint8_t fieldLocation = 1; // Location of saturation field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.put(offset, segData[segNum].saturation);
    }
    void saveCurMode(uint8_t segNum){
        uint8_t fieldLocation = 2; // Location of curMode field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.put(offset, segData[segNum].curMode);
    }
    void savePrevMode(uint8_t segNum){
        uint8_t fieldLocation = 3; // Location of prevMode field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.put(offset, segData[segNum].prevMode);
    }
    void saveSegment(uint8_t segNum){
        saveBrightness(segNum);
        saveSaturation(segNum);
        saveCurMode(segNum);
        savePrevMode(segNum);
    }
    void saveAllSegments(){
        for(int i=0; i<numSegments;i++){
            saveSegment(i);
        }
    }
    void saveAll(){
        saveHeader();
        saveAllSegments();
    }

    /*
     * Update only the EEPROM parameters that have changed.
     */
    void saveChanges(){
        Serial.printf("Checking EEPROM for changes...\n  Current ");
        EepromData::printAll();

        // Create a new segmentData structure to compare the current data against
        SegmentData compareSegData[numSegments];
        uint8_t tempCurSeg = 0;
        uint8_t* compareCurSeg = &tempCurSeg;
        EepromData tempData = EepromData(offsetAddress, numSegments, compareSegData, compareCurSeg);
        tempData.loadAll(); // load our new data structure with the current eeprom values

        bool valChanged = false;

        // Compare the EEPROM data to the current header and segmentData structure
        valChanged |= checkSaveHeader(compareCurSeg);
        valChanged |= checkSaveSegments(compareSegData);

        if(valChanged){
            Serial.printf("  Detected changes -> Updated ");
            EepromData::printAll();
        } else {
            Serial.printf("  No changes detected, no EEPROM updates.\n");
        }
    }

    void loadHeader(){
        uint8_t offset = offsetAddress;
        EEPROM.get(offset, *currentSegment);
    }
    void loadBrightness(uint8_t segNum){
        uint8_t fieldLocation = 0; // Location of brightness field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.get(offset, segData[segNum].brightness);
    }
    void loadSaturation(uint8_t segNum){
        uint8_t fieldLocation = 1; // Location of saturation field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.get(offset, segData[segNum].saturation);
    }
    void loadCurMode(uint8_t segNum){
        uint8_t fieldLocation = 2; // Location of curMode field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.get(offset, segData[segNum].curMode);
    }
    void loadPrevMode(uint8_t segNum){
        uint8_t fieldLocation = 3; // Location of prevMode field in data structure (different than the order that we write out)
        uint8_t offset = calcTotalOffset(fieldLocation, segNum);
        EEPROM.get(offset, segData[segNum].prevMode);
    }
    void loadSegment(uint8_t segNum){
        loadBrightness(segNum);
        loadSaturation(segNum);
        loadCurMode(segNum);
        loadPrevMode(segNum);
    }
    void loadAllSegments(){
        for(int i=0; i<numSegments;i++){
            loadSegment(i);
        }
    }
    void loadAll(){
        loadHeader();
        loadAllSegments();
    }
private:
    SegmentData* segData; // Pointer to segment data array structure
    uint8_t numSegments = 0; // Number of segments
    uint8_t offsetAddress = 0; // Offset (in bytes) of where to start the EEPROM save

    uint8_t headerBytes = 1; // Number of extra bytes to put at the beginning of the eeprom data for header data
    uint8_t* currentSegment; // currently selected segment

    static uint8_t segDataOrder[4];
    static uint8_t segDataSizes[4];
    static uint8_t segDataSize;

    uint8_t calcFieldOffset(uint8_t loc){
        uint8_t offset = 0;
        for(int i=0;i<loc;i++){
            offset += EepromData::segDataSizes[i];
//            offset += EepromData::segDataSizes[EepromData::segDataOrder[i]]; // Might be this one if segDataOrder isn't kept increasing from 0
        }
        return offset;
    }
    uint8_t calcTotalOffset(uint8_t fieldNum, uint8_t segNum){
        uint8_t segOffset = segNum * EepromData::segDataSize;

        uint8_t segDataLoc = EepromData::segDataOrder[fieldNum];
        uint8_t fieldOffset = calcFieldOffset(segDataLoc);

        uint8_t totalOffset = offsetAddress + headerBytes + segOffset + fieldOffset;
        return totalOffset;
    }

    bool checkSaveHeader(uint8_t* compareCurSeg){
        bool valChanged = false;
        if(*compareCurSeg != *currentSegment){
            Serial.printf("    Header (curSeg) changed from %d to %d -> saved to EEPROM.\n",*compareCurSeg,*currentSegment);
            saveHeader();
            valChanged = true;
        }
        return valChanged;
    }
    bool checkSaveSegments(SegmentData* compareSegData){
        bool valChanged = false;
        for(int i=0;i<numSegments;i++){
            // Compare 'brightness' field
            if(compareSegData[i].brightness != segData[i].brightness){
                Serial.printf("    Segment #%d: brightness changed from %d to %d -> saved to EEPROM.\n",i,compareSegData[i].brightness,segData[i].brightness);
                saveBrightness(i);
                valChanged = true;
            }
            // Compare 'saturation' field
            if(compareSegData[i].saturation != segData[i].saturation){
                Serial.printf("    Segment #%d: saturation changed from %d to %d -> saved to EEPROM.\n",i,compareSegData[i].saturation,segData[i].saturation);
                saveSaturation(i);
                valChanged = true;
            }
            // Compare 'curMode' field
            if(compareSegData[i].curMode != segData[i].curMode){
                Serial.printf("    Segment #%d: curMode changed from %d to %d -> saved to EEPROM.\n",i,compareSegData[i].curMode,segData[i].curMode);
                saveCurMode(i);
                valChanged = true;
            }
            // Compare 'prevMode' field
            if(compareSegData[i].prevMode != segData[i].prevMode){
                Serial.printf("    Segment #%d: prevMode changed from %d to %d -> saved to EEPROM.\n",i,compareSegData[i].prevMode,segData[i].prevMode);
                savePrevMode(i);
                valChanged = true;
            }
        }
        return valChanged;
    }
};

// Initialize EepromData static variables
uint8_t EepromData::segDataOrder[4] = {0,1,2,3}; // This is the order in which to save the SegmentData subfields (bright, sat, cur, prev);
uint8_t EepromData::segDataSizes[4] = {sizeof(uint8_t), sizeof(uint8_t), sizeof(LightMode), sizeof(LightMode)}; // This is an array of all the SegmentData subfield sizes
uint8_t EepromData::segDataSize = sizeof(SegmentData);

// Functions
void printSegmentData();

#endif /* _EepromTesting_H_ */
