#include <ArduinoBLE.h> // For BLE Stuff
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"

// #define LEFT_HAND 1 // Comment for right hand

/* --------- RTC CONSTANTS --------- */
RTC_PCF8523 rtc;
unsigned long rtc_set_ms; // the time (hr, min, sec) when rtc was set (in milliseconds)
unsigned long initial_ms; // ms passed until rtc was set
int curr_time[7];         // [0]: year, [1]: month, [2]: day, [3]: hour, [4]: min, [5]: sec, [6]: millisec

/* --------- FORCE SENSOR CONSTANTS --------- */
#define THUMB A0
#define PALM A1

/* --------- BATTERY CONSTANTS --------- */
#define BATTERY A4
#define RGB_RED A2
#define RGB_GREEN A3
#define RGB_BLUE A5

/* --------- STATUS LED CONSTANTS --------- */
#define STATUS_LED_WHITE D4
#define STATUS_LED_GREEN D2

/* --------- BUTTON CONSTANTS --------- */
#ifdef LEFT_HAND
#define BUTTON D9
#else
#define BUTTON D5
#endif

/* --------- STATE CONSTANTS --------- */
#define STANDBY_MODE 0   // White On | Green Off
#define RECORDING_MODE 1 // White Off | Green On
#define RTC_ERROR 2
#define SD_ERROR 3

/* --------- SD CONSTANTS --------- */
#ifdef LEFT_HAND
const uint8_t chipSelect = 10;
#else
const uint8_t chipSelect = 9;
#endif
SdFat sd;        // file system object
SdFile dataFile; // log file

/* --------- BLE CONSTANTS --------- */
#define GLOVE_BLE_NAME "Glove 1"
#define BLE_TIME_INTERVAL_MS 1000
#ifdef LEFT_HAND
BLEService theService("26548447-3cd0-4460-b683-43b332274c2b");
#else
BLEService theService("139d09c1-b45a-4c76-b4bd-778dc82a5d67");
#endif
BLECharacteristic monitorCharacteristic("43b513cf-08aa-4bd9-bc58-3f626a4248d8", BLERead | BLENotify, 512);
BLECharacteristic infoCharacteristic("b106d600-3ee1-4a10-8dd7-260074535086", BLERead | BLENotify, 512);
BLECharacteristic rtcCharacteristic("81600d69-4d48-4d19-b299-7ef5e3b21f69", BLERead | BLEWrite, 512);

/* --------- General CONSTANTS --------- */
#define DELAY_PER_LOOP 200
#ifdef LEFT_HAND
#define GLOVE_HAND "Left"
#else
#define GLOVE_HAND "Right"
#endif

/* -------------- GLOBALS -------------- */
int currMode;                   // Global State Variable
String fileName = String();     // Current file we will be writing to
unsigned int sessionNumber = 1; // Keep track of session number
unsigned int numReadings;       // Keep track of number of readings in each recording session
unsigned long last_ble_time;    // The last time BLE was accessed

/* ------------------------------------------------------------------------------------------------------------------------ */
/* --------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------ STATE MANAGEMENT ------------------------------------ */
/* ------------------------------------------------------------------------------------------ */

/* ------------------------------ Status Lights Handler ------------------------------ */
void changeStatusLights()
{
    switch (currMode)
    {
    /* --------------- Set Standby Mode --------------- */
    case STANDBY_MODE:
        // Set Status Lights
        digitalWrite(STATUS_LED_WHITE, HIGH);
        digitalWrite(STATUS_LED_GREEN, LOW);
        break;
    /* --------------- Set Recording Mode --------------- */
    case RECORDING_MODE:
        // Set Status Lights
        digitalWrite(STATUS_LED_WHITE, LOW);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        break;
    /* --------------- Generic RTC Error --------------- */
    case RTC_ERROR:
        digitalWrite(STATUS_LED_WHITE, HIGH);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        break;
    /* --------------- Generic SD Error --------------- */
    case SD_ERROR:
        digitalWrite(STATUS_LED_WHITE, LOW);
        digitalWrite(STATUS_LED_GREEN, LOW);
        break;
    }
}

/* ------------------------------ Glove State Handler ------------------------------ */
void changeState(int newMode)
{
    currMode = newMode;
    changeStatusLights();

    if (currMode == RECORDING_MODE)
    {
        initSessionTime();
    }
}

void changeState_Test()
{
    Serial.print("changeState() Test: ");

    unsigned long testTime = millis();

    changeState(RECORDING_MODE);

    if ((currMode == RECORDING_MODE) && (initial_ms > testTime))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ------------------------------ Glove State Intermission ------------------------------ */
void buttonIntermission()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(STATUS_LED_WHITE, HIGH);
        digitalWrite(STATUS_LED_GREEN, LOW);
        delay(500);
        digitalWrite(STATUS_LED_WHITE, LOW);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        delay(500);
    }
}

void buttonIntermission_Test()
{

    Serial.print("buttonIntermission() Test: ");

    unsigned long startTime = millis();
    buttonIntermission();
    unsigned long endTime = millis();

    if (endTime >= startTime + 2999)
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ----------------------------------------------------------------------------------------- */
/* ------------------------------------ DATA COLLECTION ------------------------------------ */
/* ----------------------------------------------------------------------------------------- */

/* ------------------------------ Voltage to Force Model ----------------------------------- */
long getVoltageToForce(int analogReadVal)
{
    int fsr; // analog reading from fsr resistor divider
    double forceVal;
    //fsr = analogRead(pinNum);  // voltage output mapped 0 - 1023
    fsr = analogReadVal;
    // Force = 36.1e^(0.00498x) from model
    if (fsr == 0)
    {
        forceVal = 0;
        return forceVal;
    }
    else
    {
        forceVal = 0.00498 * fsr;
        forceVal = 36.1 * (exp(forceVal));
    }
    return forceVal;
}

long getVoltageToForce_Test()
{
    Serial.print("getVoltageToForce() Test: ");

    long testForce = getVoltageToForce(600);
    if ((testForce > 690) && (testForce < 720))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}
/* ------------------------------ Initialize Session Time ----------------------------------- */
void initSessionTime()
{
    // check RTC for error
    if (!rtc.begin() || !rtc.initialized() || rtc.lostPower())
    {
        Serial.println("RTC ERROR");
        changeState(RTC_ERROR);
        return;
    }

    //convert RTC hr,min,sec to milliseconds
    DateTime start_time = rtc.now();                               //year|month|day|hour|min|sec
    unsigned long start_hr = start_time.hour() * 3.6 * pow(10, 6); //in milliseconds
    unsigned long start_min = start_time.minute() * 60000;
    unsigned long start_sec = start_time.second() * 1000;

    // set global rtc_set_ms
    rtc_set_ms = start_hr + start_min + start_sec;

    // set global intial_ms
    initial_ms = millis();
}

void initSessionTime_Test()
{
    Serial.print("initSessionTime() Test: ");

    rtc_set_ms = millis();
    initial_ms = millis();

    unsigned long old_rtc_set_ms = rtc_set_ms;
    unsigned long old_initial_ms = initial_ms;

    initSessionTime();

    if ((rtc_set_ms > old_rtc_set_ms) && (initial_ms > old_initial_ms))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ------------------------------ Convert Time of Day in MS Count to Military Time ----------------------------------- */
void getTimeFromMillis(unsigned long ms)
{
    // get total ms, sec, min, & hours from ms
    unsigned long currMs = ms;
    unsigned long seconds = currMs / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;

    // gets exact ms, sec, min, & hours
    currMs %= 1000;
    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    // set global current time variable
    curr_time[3] = hours;
    curr_time[4] = minutes;
    curr_time[5] = seconds;
    curr_time[6] = currMs;
}

void getTimeFromMillis_Test()
{
    Serial.print("getTimeFromMillis() Test: ");

    unsigned long testTime = 66630030; // 6:30:30:30 PM in ms

    getTimeFromMillis(testTime);

    if ((curr_time[3] == 18) && (curr_time[4] == 30) && (curr_time[5] == 30) && (curr_time[6] == 30))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ------------------------------ Set Current Time ----------------------------------- */
void getTime()
{
    // check RTC for error
    if (!rtc.begin() || !rtc.initialized() || rtc.lostPower())
    {
        Serial.println("RTC ERROR");
        changeState(RTC_ERROR);
        return;
    }

    DateTime currNow = rtc.now();
    // set global current time variable
    curr_time[0] = currNow.year();
    curr_time[1] = currNow.month();
    curr_time[2] = currNow.day();

    unsigned long curr_ms = rtc_set_ms + (millis() - initial_ms); //millis() - initial_ms = time passed after setting initial time
    getTimeFromMillis(curr_ms);
}

/* ------------------------------ Collect RTC + Force Values ------------------------------ */
String getDataString()
{
    getTime();
    String toReturn = "{\"Time\":" + String(curr_time[0]) + ":" + String(curr_time[1]) + ":" + String(curr_time[2]) + ":" + String(curr_time[3]) + ":" //hour
                      + String(curr_time[4]) + ":"                                                                                                     //min
                      + String(curr_time[5]) + ":"                                                                                                     //sec
                      + String(curr_time[6]) + ":"                                                                                                     //millisec
                      + String() + ":" + ",";
    toReturn += "\"THUMB\":" + String(analogRead(THUMB)) + ",";
    toReturn += "\"PALM\":" + String(analogRead(PALM)) + "}";
    return toReturn;
}

void getDataString_Test()
{
    Serial.print("getDataString() Test: ");

    String dataString = getDataString();

    if (dataString.length() > 0)
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* -------------------------------------------------------------------------------------- */
/* ------------------------------------ DATA STORAGE ------------------------------------ */
/* -------------------------------------------------------------------------------------- */

/* ------------------------------ Generates and stores session file on SD Card ------------------------------ */
void generateSessionFile()
{
    // Construct the filename
    DateTime now = rtc.now();
    fileName = String(now.year()) + "_" + String(now.month()) + "_" + String(now.day()) + "_#";
    fileName += sessionNumber;
    fileName += ".csv";

    // Keep generating new session file if already exists
    while (sd.exists(fileName.c_str()))
    {
        sessionNumber++;
        // Serial.println(fileName + " exists");

        fileName = String(now.year()) + "_" + String(now.month()) + "_" + String(now.day()) + "_#";
        fileName += sessionNumber;
    }

    // Try to open file
    if (!dataFile.open(fileName.c_str(), O_WRONLY | O_CREAT | O_EXCL))
    {
        //      error("File.open");
        Serial.println("Error opening file");
        changeState(SD_ERROR);
    }
}

void generateSessionFile_Test()
{
    Serial.print("generateSessionFile() Test: ");
    generateSessionFile();
     // Write some junk to file and close it
    dataFile.println("test");
    dataFile.close();

    // Make sure file exists
    if(sd.exists(fileName.c_str()))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- SETUP --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------- */
void setup()
{
    Serial.begin(9600);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif

    /* --------- Initialize Standy Mode --------- */
    changeState(STANDBY_MODE);

    /* --------- Initialize RTC Module --------- */

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        changeState(RTC_ERROR);
    }

    if (!rtc.initialized() || rtc.lostPower())
    {
        changeState(RTC_ERROR);
        Serial.println("RTC is NOT initialized, let's set the time!");
    }

    /* --------- Initialize SD card --------- */
    // call SD.begin once in setup
    if (!sd.begin(chipSelect, SD_SCK_MHZ(15)))
    {
        Serial.println("SD Card failed");
        changeState(SD_ERROR);
    }

    /* --------- Initialize BLE --------- */
    // begin BLE
    if (!BLE.begin())
    {
        Serial.println("Starting BLE failed");
        while (1)
            ;
    }

    BLE.setLocalName(GLOVE_BLE_NAME);
    BLE.setAdvertisedService(theService);
    theService.addCharacteristic(monitorCharacteristic);
    theService.addCharacteristic(infoCharacteristic);
    BLE.addService(theService);

    // start advertising
    BLE.advertise();
    last_ble_time = millis();
}

/* ----------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- MAIN LOOP --------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------------- */
void loop()
{
    Serial.println();

    // setup_test()
    Serial.print("setBatteryIndicator() Test: ");
    if (currMode == STANDBY_MODE)
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
    Serial.println();

    changeState_Test();
    Serial.println();

    buttonIntermission_Test();
    Serial.println();

    initSessionTime_Test();
    Serial.println();

    getTimeFromMillis_Test();
    Serial.println();

    getDataString_Test();
    Serial.println();

    getVoltageToForce_Test();
    Serial.println();

    generateSessionFile_Test();
    Serial.println();

    Serial.println("Tests Completed");
    while (1);

}
