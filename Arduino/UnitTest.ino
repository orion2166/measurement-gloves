//#include <Arduino_LSM9DS1.h> // For IMU Stuff
#include <ArduinoBLE.h> // For BLE Stuff
#include "RTClib.h"

/* --------- RTC CONSTANTS --------- */
RTC_PCF8523 rtc;
unsigned long rtc_set_ms;    // the time (hr, min, sec) when rtc was set (in milliseconds)
unsigned long initial_ms;    // ms passed until rtc was set
int curr_time[4];            // [0]: hour, [1]: min, [2]: sec, [3]: millisec
unsigned long last_ble_time; // The last time BLE was accessed
/* --------- FORCE SENSOR CONSTANTS --------- */
#define THUMB A0
#define PALM A1
/* --------- STTUS LED CONSTANTS --------- */
#define STATUS_LED_WHITE D2
#define STATUS_LED_GREEN D3
/* --------- BUTTON CONSTANTS --------- */
#define BUTTON D4
/* --------- STATE CONSTANTS --------- */
#define STANDBY_MODE 0   // White On | Green Off
#define RECORDING_MODE 1 // White Off | Green On
#define RTC_ERROR 2
#define SD_ERROR 3
int currMode; // Global State Variable
/* --------- BLE CONSTANTS --------- */
BLEService theService("26548447-3cd0-4460-b683-43b332274c2b"); // LEFT HAND
//BLEService theService("139d09c1-b45a-4c76-b4bd-778dc82a5d67"); // RIGHT HAND
BLECharacteristic monitorCharacteristic("43b513cf-08aa-4bd9-bc58-3f626a4248d8", BLERead | BLENotify, 512);
BLECharacteristic infoCharacteristic("b106d600-3ee1-4a10-8dd7-260074535086", BLERead | BLENotify, 512);
BLECharacteristic rtcCharacteristic("81600d69-4d48-4d19-b299-7ef5e3b21f69", BLERead | BLEWrite, 512);
#define GLOVE_BLE_NAME "Glove 1"
#define BLE_TIME_INTERVAL_MS 1000

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
    Serial.print("Mode = ");
    Serial.println(newMode);

    currMode = newMode;
    changeStatusLights();
}

void changeState_Test()
{
    Serial.println("changeState() Test: ");
    bool passed = true;

    changeState(STANDBY_MODE);
    if (currMode == STANDBY_MODE)
    {
        Serial.print("Passed");
    }
    else
    {
        passed = false;
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

    Serial.println("buttonIntermission() Test: ");

    startTime = millis();
    buttonIntermission();

    if (startTime > 1000)
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

/* ------------------------------ Initialize Session Time ----------------------------------- */
void initSessionTime()
{
    // check RTC for error
    if (!rtc.begin() || !rtc.initialized() || rtc.lostPower())
    {
        Serial.println("RTC ERROR");
        changeState(RTC_ERROR);
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
    Serial.println("initSessionTime() Test: ");

    rtc_set_ms = millis();
    initial_ms = millis();

    old_rtc_set_ms = rtc_set_ms;
    old_initial_ms = initial_ms;

    initSessionTime();

    if ((rtc_set_ms != old_rtc_set_ms) && (initial_ms != old_initial_ms) && (rtc_set_ms > old_rtc_set_ms) && (initial_ms > old_initial_ms))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ------------------------------ Convert MS Count to STD Time ----------------------------------- */
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
    curr_time[0] = hours;
    curr_time[1] = minutes;
    curr_time[2] = seconds;
    curr_time[3] = currMs;
}

void getTimeFromMillis_Test()
{
    Serial.println("getTimeFromMillis() Test: ");

    unsigned long testTime = 66630030; // 6:30:30:30 PM in ms

    getTimeFromMillis(testTime);

    if ((curr_time[0] != 18) && (curr_time[1] != 30) && (curr_time[2] != 30) && (curr_time[3] != 30))
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
}

/* ------------------------------ Collect RTC + Force Values ------------------------------ */
String getDataString()
{
    unsigned long curr_ms = rtc_set_ms + (millis() - initial_ms); //millis() - initial_ms = time passed after setting initial time
    getTimeFromMillis(curr_ms);
    String toReturn = "{\"Time\":" + String(now.year()) + ":" + String(now.month()) + ":" + String(now.day()) + ":" + String(curr_time[0]) + ":" //hour
                      + String(curr_time[1]) + ":"                                                                                               //min
                      + String(curr_time[2]) + ":"                                                                                               //sec
                      + String(curr_time[3]) + ":"                                                                                               //millisec
                      + String() + ":" + ",";
    toReturn += "\"THUMB\":" + String(analogRead(THUMB)) + ",";
    toReturn += "\"PALM\":" + String(analogRead(PALM)) + "}";
    return toReturn;
}

void getDataString_Test()
{
    Serial.println("getDataString_Test() Test: ");

    string dataString = getDataString();

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

/* write SD stuff here */

/* ---------------------------------------------------------------------------------------- */
/* ------------------------------------ BATTERY STATUS ------------------------------------ */
/* ---------------------------------------------------------------------------------------- */

//String getStatusBatteryString()
//{
//    // Need to add battery reading
//    return "{\"state\":" + String(currMode) + "}";
//}

/* ------------------------------ Set Battery LED Color ------------------------------ */
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
    analogWrite(RGB_RED, red_light_value);
    analogWrite(RGB_GREEN, green_light_value);
    analogWrite(RGB_BLUE, blue_light_value);
}

/* ------------------------------ Calculate & Set LED Color ------------------------------ */
float setBatteryIndicator()
{

    // read analog voltage from battery
    int sensorValue = analogRead(BATTERY);

    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    float V = sensorValue * (3.3 / 1023.0);

    // set battery LEDs
    if (V >= 2.65)
    {
        RGB_color(255, 0, 255);
    }
    if (V < 2.65 && V >= 2.5)
    {
        RGB_color(0, 0, 255);
    }
    if (V < 2.5)
    {
        RGB_color(0, 255, 255);
    }

    return V;
}

void setBatteryIndicator_Test()
{
    Serial.println("setBatteryIndicator() Test: ");

    float V = setBatteryIndicator();

    if ((V >= 0) && (V <= 3.3))
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

    /* --------- Initialize BLE --------- */
    // begin BLE
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed");
        while (1);
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
    // setup_test()
    Serial.println("setBatteryIndicator() Test: ");
    if (currMode == STANDBY_MODE)
    {
        Serial.println("Passed");
    }
    else
    {
        Serial.println("Failed");
    }
    Serial.println();

    changeState_Test()
    Serial.println();

    buttonIntermission_Test();
    Serial.println();

    initSessionTime_Test();
    Serial.println();

    getTimeFromMillis_Test();
    Serial.println();

    getDataString_Test();
    Serial.println();

    setBatteryIndicator_Test();
    Serial.println();

    while(1);
}
