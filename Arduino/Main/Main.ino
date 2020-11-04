#include <ArduinoBLE.h> // For BLE Stuff
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"

//#define LEFT_HAND 1 // Comment for right hand

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
#ifdef LEFT_HAND
#define GLOVE_BLE_NAME "Glove 1 (Left)"
#else
#define GLOVE_BLE_NAME "Glove 1 (Right)"
#endif
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

//
// Changes the status lights based on the current status mode
//
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

//
// Changes the status lights based on the current status mode
// newMode: new status mode code
//
void changeState(int newMode)
{
    Serial.print("Mode = ");
    Serial.println(newMode);

    currMode = newMode;
    changeStatusLights();

    if (currMode == RECORDING_MODE)
    {
        numReadings = 0;
        initSessionTime();
        generateSessionFile();
    }
    if (currMode == STANDBY_MODE)
    {
        Serial.println(numReadings);
        dataFile.close();
    }
}

//
// Gives a 3 second delay between button press and state change
// Blinks status LEDs to indicate intermission
//
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

//
// Reads voltage from given analog pin and converts voltage to force (in grams) using model
// pinNum: analog pin to read voltage from
// returns: force in grams
//
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
        // Apply model
        forceVal = 0.00498 * fsr;
        forceVal = 36.1 * (exp(forceVal));
    }
    return forceVal;
}

//
// Reads the voltage to force to send over BLE (very slow)
// returns: A JSON String
//
String getMonitoringDataString()
{
    return "{\"Thumb\":" + String(getVoltageToForce(THUMB)) + ",\"Palm\":" + String(getVoltageToForce(PALM)) + "}";
}

//
// Initializes the starting session time using RTC and stored in rtc_set_ms
//
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

//
// Convert Time of Day in MS Count to Military Time
//
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

//
// Gets the current time and updates the time stored in curr_time
//
void updateCurrentTime()
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

//
// Reads RTC, and force values and generates a data string to write to CSV file
// returns: String in format: "<Time in ISO>, <THUMB force in g>, <PALM force in g>, <HAND (Left or Right)>"
//
String getDataString()
{
    String toReturn = String(curr_time[0]) + ":" + String(curr_time[1]) + ":" + String(curr_time[2]) + ":" + String(curr_time[3]) + ":" // hour
                      + String(curr_time[4]) + ":"                                                                                      // min
                      + String(curr_time[5]) + ":"                                                                                      // sec
                      + String(curr_time[6]) + ",";                                                                                     // ms
    toReturn += String(getVoltageToForce(THUMB)) + ",";
    toReturn += String(getVoltageToForce(PALM)) + ",";
    toReturn += GLOVE_HAND;
    return toReturn;
}

/* -------------------------------------------------------------------------------------- */
/* ----------------------------------s-- DATA STORAGE ------------------------------------ */
/* -------------------------------------------------------------------------------------- */

//
// Creates a new session file based on RTC time and session number
//
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
        Serial.println(fileName + " exists");

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

    writeHeaderToFile();
}

//
// Writes column headers to the file
//
void writeHeaderToFile()
{
    // write data headers
    dataFile.println("RTC,Thumb,Palm,Hand");
    Serial.println("Writing headers");
}

//
// Writes a data string to the CSV file
//
void writeDataToFile()
{
    // Write data to file
    String toWrite = getDataString();
    dataFile.println(toWrite);
    Serial.println("Writing to file");
}

/* ---------------------------------------------------------------------------------------- */
/* ------------------------------------ BATTERY STATUS ------------------------------------ */
/* ---------------------------------------------------------------------------------------- */

//
// Generate a status and battery JSON string to be sent over BLE
// returns: string with status and battery in the format: '{ "state": <state>, "battery": <battery> }'
String getStatusBatteryJSONString()
{
    String statusString = "";
    if(currMode == RECORDING_MODE)
    {
        statusString = "Recording";
    }
    else if (currMode == STANDBY_MODE)
    {
        statusString = "Standby";
    }
    else if(currMode == RTC_ERROR)
    {
        statusString = "RTC Error";
    }
    else if(currMode == SD_ERROR)
    {
        statusString = "SD Error";
    }
    // TODO: Need to add battery reading
    return "{\"state\":\"" + statusString + "\", \"battery\":100}";
}

//
// Set RGB LED with given red and green values
// red_light_value: value to write to R value of RGB LED
// green_light_value: value to write to G value of RGB LED
//
void RGB_color(int red_light_value, int green_light_value)
{
    analogWrite(RGB_RED, red_light_value);
    analogWrite(RGB_GREEN, green_light_value);
}

//
// Read the battery value and set battery LED accordingly
//
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
    #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
    #endif

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

    /* --------- Initialize BLE --------- */
    // begin BLE
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed");
    }

    BLE.setLocalName(GLOVE_BLE_NAME);
    BLE.setAdvertisedService(theService);
    theService.addCharacteristic(monitorCharacteristic);
    theService.addCharacteristic(infoCharacteristic);
    theService.addCharacteristic(rtcCharacteristic);
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
        ++numReadings;
        // get RTC time + force sensor values
        updateCurrentTime();
        String vals = getDataString(); // Get Values
        Serial.println(vals);

        // Write toWrite to SD
        // Within function: check for SD card, write to sd card
        writeDataToFile();
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
        if (central.connected())
        {
            // Do this only every second
            if (last_ble_time - millis() >= BLE_TIME_INTERVAL_MS)
            {
                // Check if RTC Was written
                if (rtcCharacteristic.written())
                {
                    Serial.println("RTC Was written!");
                    char rtcString[128];
                    strncpy(rtcString, (char *)rtcCharacteristic.value(), rtcCharacteristic.valueLength());
                    if (currMode == RECORDING_MODE)
                    {
                        // rtcCharacteristic.writeValue("Can't reset RTC while recording");
                        Serial.println("Can't reset RTC while recording");
                    }
                    else
                    {
                        rtc.adjust(DateTime(String(rtcString).toInt()));
                    }
                }
                // Send the current status and battery
                infoCharacteristic.writeValue(getStatusBatteryJSONString().c_str());

                if (currMode == RECORDING_MODE)
                {
                    monitorCharacteristic.writeValue(getMonitoringDataString().c_str());
                }

                last_ble_time = millis();
            }
        }
    }

    /* --------- DELAY --------- */
    while (millis() - loopStartTime < DELAY_PER_LOOP)
        ; // takes time of exec into account
}
