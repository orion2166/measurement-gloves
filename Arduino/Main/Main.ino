//#include <Arduino_LSM9DS1.h> // For IMU Stuff
#include <ArduinoBLE.h> // For BLE Stuff
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"

/* --------- RTC CONSTANTS --------- */
RTC_PCF8523 rtc;
unsigned long rtc_set_ms;    // the time (hr, min, sec) when rtc was set (in milliseconds)
unsigned long initial_ms;    // ms passed until rtc was set
int curr_time[7];            // [0]: year, [1]: month, [2]: day, [3]: hour, [4]: min, [5]: sec, [6]: millisec
unsigned long last_ble_time; // The last time BLE was accessed
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
#define BUTTON D8
/* --------- STATE CONSTANTS --------- */
#define STANDBY_MODE 0   // White On | Green Off
#define RECORDING_MODE 1 // White Off | Green On
#define RTC_ERROR 2
#define SD_ERROR 3
/* --------- SD CONSTANTS --------- */
const uint8_t chipSelect = 10;
SdFat sd;        // file system object
SdFile dataFile; // log file
/* --------- BLE CONSTANTS --------- */
BLEService theService("26548447-3cd0-4460-b683-43b332274c2b"); // LEFT HAND
//BLEService theService("139d09c1-b45a-4c76-b4bd-778dc82a5d67"); // RIGHT HAND
BLECharacteristic monitorCharacteristic("43b513cf-08aa-4bd9-bc58-3f626a4248d8", BLERead | BLENotify, 512);
BLECharacteristic infoCharacteristic("b106d600-3ee1-4a10-8dd7-260074535086", BLERead | BLENotify, 512);
BLECharacteristic rtcCharacteristic("81600d69-4d48-4d19-b299-7ef5e3b21f69", BLERead | BLEWrite, 512);
#define GLOVE_BLE_NAME "Glove 1"
#define BLE_TIME_INTERVAL_MS 1000
/* --------- General CONSTANTS --------- */
#define DELAY_PER_LOOP 200
/* --------- GLOBALS --------- */
int currMode;                   // Global State Variable
String fileName = String();     // current file we will be writing to
unsigned int sessionNumber = 1; // keep track of session number

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

    if (currMode == RECORDING_MODE)
    {
        initSessionTime();
        generateSessionFile();
    }
    if (currMode == STANDBY_MODE)
    {
        // check if there is an open file and close it
        //        if (dataFile) {
        dataFile.close();
        //        }
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

/* ----------------------------------------------------------------------------------------- */
/* ------------------------------------ DATA COLLECTION ------------------------------------ */
/* ----------------------------------------------------------------------------------------- */

/* ------------------------------ Voltage to Force Model ----------------------------------- */
long getVoltageToForce(int pinNum)
{
    int fsr; // analog reading from fsr resistor divider
    double forceVal;
    fsr = analogRead(pinNum); // voltage output mapped 0 - 1023
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
    String toReturn = "{\"Time\":" + String(curr_time[0]) + ":" + String(curr_time[1]) + ":" + String(curr_time[2]) + ":" + String(curr_time[3]) + ":" //hour
                      + String(curr_time[4]) + ":"                                                                                                     //min
                      + String(curr_time[5]) + ":"                                                                                                     //sec
                      + String(curr_time[6]) + ":"                                                                                                     //millisec
                      + String() + ":" + ",";
    toReturn += "\"THUMB\":" + String(getVoltageToForce(THUMB)) + ",";
    toReturn += "\"PALM\":" + String(getVoltageToForce(PALM)) + "}";
    return toReturn;
}

/* -------------------------------------------------------------------------------------- */
/* ------------------------------------ DATA STORAGE ------------------------------------ */
/* -------------------------------------------------------------------------------------- */

/* ------------------------------ Create new session file  ------------------------------ */
void generateSessionFile()
{
    String message = String();
    DateTime now = rtc.now();

    fileName = String(now.year()) + "_" + String(now.month()) + "_" + String(now.day()) + "_#";
    fileName += sessionNumber;
    fileName += ".csv";
    message = fileName;

    // generate new session file if already exists
    while (sd.exists(fileName.c_str()))
    {
        message += " exists.";
        sessionNumber++;
        Serial.println(message);

        fileName = String(now.year()) + "_" + String(now.month()) + "_" + String(now.day()) + "_#";
        fileName += sessionNumber;
    }

    if (!dataFile.open(fileName.c_str(), O_WRONLY | O_CREAT | O_EXCL))
    {
        //      error("File.open");
        Serial.println("Error opening file");
        changeState(SD_ERROR);
        while (1)
            ;
    }

    writeHeader();
}

void writeHeader()
{
    // write data headers
    dataFile.println("RTC,Thumb,Palm");
    Serial.println("Writing headers");
}

void logData()
{
    // write data to file
    String Vals = getDataString();
    dataFile.println(Vals);
    Serial.println("Writing to file");
}

/* ---------------------------------------------------------------------------------------- */
/* ------------------------------------ BATTERY STATUS ------------------------------------ */
/* ---------------------------------------------------------------------------------------- */

//String getStatusBatteryString()
//{
//    // Need to add battery reading
//    return "{\"state\":" + String(currMode) + "}";
//}

///* ------------------------------ Set Battery LED Color ------------------------------ */
//void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
//{
//    analogWrite(RGB_RED, red_light_value);
//    analogWrite(RGB_GREEN, green_light_value);
//    analogWrite(RGB_BLUE, blue_light_value);
//}

/* ------------------------------ Set Battery LED Color ------------------------------ */
void RGB_color(int red_light_value, int green_light_value)
{
    analogWrite(RGB_RED, red_light_value);
    analogWrite(RGB_GREEN, green_light_value);
}

///* ------------------------------ Calculate & Set LED Color ------------------------------ */
//void setBatteryIndicator()
//{
//
//    // read analog voltage from battery
//    int sensorValue = analogRead(BATTERY);
//
//    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
//    float V = sensorValue * (3.3 / 1023.0);
//
//    // set battery LEDs
//    if (V >= 2.65)
//    {
//        RGB_color(255, 0, 255);
//    }
//    if (V < 2.65 && V >= 2.5)
//    {
//        RGB_color(0, 0, 255);
//    }
//    if (V < 2.5)
//    {
//        RGB_color(0, 255, 255);
//    }
//}

/* ------------------------------ Calculate & Set LED Color ------------------------------ */
void setBatteryIndicator()
{

    // read analog voltage from battery
    int sensorValue = analogRead(BATTERY);

    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    float V = sensorValue * (3.3 / 1023.0);

    // set battery LEDs
    if (V >= 2.65)
    {
        RGB_color(255, 0);
    }
    if (V < 2.65 && V >= 2.5)
    {
        RGB_color(0, 0);
    }
    if (V < 2.5)
    {
        RGB_color(0, 255);
    }
}

/* ------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- SETUP --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------- */
void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;
    /* --------- Initialize Standby Mode --------- */
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

    /* NEED FOR RTC RESET

    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.

  */

    /* --------- Initialize BLE --------- */
    // begin BLE
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed");
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

    /* --------- Initialize SD card --------- */
    // call SD.begin once in setup
    if (!sd.begin(chipSelect, SD_SCK_MHZ(15)))
    {
        Serial.println("Card failed");
        changeState(SD_ERROR);
    }
}

/* ----------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- MAIN LOOP --------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------------- */
void loop()
{
    unsigned long loopStartTime = millis();

    Serial.println(digitalRead(BUTTON));
    /* --------- Change State on Button Press --------- */
    if (digitalRead(BUTTON) == HIGH && currMode != RTC_ERROR && currMode != SD_ERROR)
    {
        // Console Print
        Serial.println("\nState Change !!!");
        // Hold Button Input
        buttonIntermission();
        // Toggle State
        if (currMode == STANDBY_MODE)
        {
            changeState(RECORDING_MODE);
        }
        else
        {
            changeState(STANDBY_MODE);
        }
    }

    /* --------- Collect Values if in Recording Mode --------- */
    if (currMode == RECORDING_MODE)
    {
        // get RTC time + force sensor values
        getTime();
        String vals = getDataString(); // Get Values
        Serial.println(vals);

        // Write Vals to SD
        // Within function: check for SD card, write to sd card
        logData();
        // Force data to SD and update the directory entry to avoid data loss.
        if (!dataFile.sync() || dataFile.getWriteError())
        {
            Serial.println("write error");
            changeState(SD_ERROR);
        }
    }

    /* ---------- Set Battery Indicator ---------- */
    setBatteryIndicator();

    /* --------- BLE Stuff --------- */
    BLEDevice central = BLE.central();
    // If there is a connected central device
    if (central)
    {
        // Do this only every second
        if (last_ble_time - millis() >= BLE_TIME_INTERVAL_MS)
        {
            // Check if RTC Was written
            if (rtcCharacteristic.written)
            {
                char rtcString[128];
                strncpy(rtcString, (char *)rtcCharacteristic.value(), rtcCharacteristic.valueLength());
                if (currMode == RECORDING_MODE)
                {
                    rtcCharacteristic.writeValue("Can't reset RTC while recording");
                }
                else
                {
                    // Reset RTC
                }
            }
            // Send the current status and battery
            infoCharacteristic.writeValue(getStatusBatteryString().c_str());
            if (currMode == RECORDING_MODE)
            {
                // Send monitoring data
                // monitorCharacteristic(getMonitoringDataString().c_str());
            }
            last_ble_time = millis();
        }
    }

    while (millis() - loopStartTime < DELAY_PER_LOOP)
        ;
}
