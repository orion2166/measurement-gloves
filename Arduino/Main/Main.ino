
/**
 * @file Main.ino
 *
 * @mainpage Measurement Gloves (Arduino)
 *
 * @section description Description
 * This is a sketch loaded onto the Arduino for Measurement Gloves to control the board
 * LEDs and buttons, read force sensor and RTC (Real Time Clock) values, and write them
 * to the onboard SD card
 *
 * @section circuit Circuit
 * - Red LED connected to pin D2.
 * - Momentary push button connected to pin D3.
 *
 * @section libraries Libraries
 * - ArduinoBLE.h
 *   - Used for BLE communications with the mobile application
 * - SPI.h
 *   - Used for modules that use the SPI bus
 * - SdFat.h
 *   - Used for SD Card reading and writing
 * - RTCLib.h
 *   - Used to read RTC timing
 */

#include <ArduinoBLE.h>
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"

/** Indicates left hand*/
#define LEFT_HAND 1 // Comment for right hand

/* --------- RTC CONSTANTS --------- */
/** (Global) RTC Variable from RTClib.h*/
RTC_PCF8523 rtc;
/** (Global) The time (hr, min, sec) when rtc was set (in milliseconds)*/
unsigned long rtc_set_ms;
/** (Global) milliseconds passed until rtc was set*/
unsigned long initial_ms;
/** (Global) Stores the current time as array with the following format –
 * [0]: year, [1]: month, [2]: day, [3]: hour, [4]: min, [5]: sec, [6]: millisec*/
int curr_time[7];

/* --------- FORCE SENSOR CONSTANTS --------- */
/** Thumb force sensor analog pin*/
#define THUMB A0
/** Palm force sensor analog pin*/
#define PALM A1

/* --------- BATTERY CONSTANTS --------- */
/** Battery analog pin (to read value)*/
#define BATTERY A7
/** RGB Red Analog pin*/
#define RGB_RED A2
/** RGB Green Analog pin*/
#define RGB_GREEN A3
/** RGB Blue Analog pin*/
#define RGB_BLUE A5

/* --------- STATUS LED CONSTANTS --------- */
/** Red LED Digital Pin*/
#define STATUS_LED_RED D4
/** Green LED Digital Pin*/
#define STATUS_LED_GREEN D2

/* --------- BUTTON CONSTANTS --------- */
/** Button Digital Pin*/
#ifdef LEFT_HAND
#define BUTTON D9
#else
#define BUTTON D3
#endif

/* --------- STATE CONSTANTS --------- */
#define STANDBY_MODE 0
#define RECORDING_MODE 1
#define RTC_ERROR 2
#define SD_ERROR 3

/* --------- SD CONSTANTS --------- */
/** Chip Select Pin for the SD Card*/
#ifdef LEFT_HAND
const uint8_t chipSelect = 10;
#else
const uint8_t chipSelect = 9;
#endif
/** (Global) SD Card object*/
SdFat sd;
/** (Global) data file stored on SD Card*/
SdFile dataFile;

/* --------- BLE CONSTANTS --------- */
/** Glove to name to advertise on BLE*/
#ifdef LEFT_HAND
#define GLOVE_BLE_NAME "Glove 1 (Left)"
#else
#define GLOVE_BLE_NAME "Glove 1 (Right)"
#endif
#define BLE_TIME_INTERVAL_MS 1000
#ifdef LEFT_HAND
/** BLE Service UUID*/
BLEService theService("26548447-3cd0-4460-b683-43b332274c2b");
#else
BLEService theService("139d09c1-b45a-4c76-b4bd-778dc82a5d67");
#endif
/** BLE Monitoring Characteristic UUID*/
BLECharacteristic monitorCharacteristic("43b513cf-08aa-4bd9-bc58-3f626a4248d8", BLERead | BLENotify, 512);
/** BLE Information (Status and Battery) Characteristic UUID*/
BLECharacteristic infoCharacteristic("b106d600-3ee1-4a10-8dd7-260074535086", BLERead | BLENotify, 512);
/** BLE RTC Characteristic UUID*/
BLECharacteristic rtcCharacteristic("81600d69-4d48-4d19-b299-7ef5e3b21f69", BLERead | BLEWrite, 512);

/* --------- General CONSTANTS --------- */
/** Time to delay everytime loop occurs in milliseconds*/
#define DELAY_PER_LOOP 200
/** Stores "Left" or "Right"*/
#ifdef LEFT_HAND
#define GLOVE_HAND "Left"
#else
#define GLOVE_HAND "Right"
#endif

/* -------------- GLOBALS -------------- */
/** (Global) Current Status Mode – one of STANDBY_MODE, RECORDING_MODE, SD_ERROR, or RTC_ERROR*/
int currMode;
/** (Global) Stores the current filename being written to*/
String fileName = String();
/** (Global) Stores the current session number*/
unsigned int sessionNumber = 1;
/** (Global) Keeps track of the number of readings in one recording session*/
unsigned int numReadings;
/** (Global) Keeps track of the last time BLE was accessed in loop()*/
unsigned long last_ble_time;

/* ------------------------------------------------------------------------------------------------------------------------ */
/* --------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------ STATE MANAGEMENT ------------------------------------ */
/* ------------------------------------------------------------------------------------------ */

/**
 * @brief Changes the status lights based on the current status mode.
 * Uses the global variable currMode and sets the status LEDs accordingly:
 * STANDBY_MODE : GREEN
 * RECORDING_MODE : RED
 * RTC_ERROR: GREEN and RED
 * SD_ERROR: Both off
 */
void changeStatusLights()
{
    switch (currMode)
    {
    /* --------------- Set Standby Mode --------------- */
    case STANDBY_MODE:
        // Set Status Lights
        digitalWrite(STATUS_LED_RED, LOW);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        break;
    /* --------------- Set Recording Mode --------------- */
    case RECORDING_MODE:
        // Set Status Lights
        digitalWrite(STATUS_LED_RED, HIGH);
        digitalWrite(STATUS_LED_GREEN, LOW);
        break;
    /* --------------- Generic RTC Error --------------- */
    case RTC_ERROR:
        digitalWrite(STATUS_LED_RED, HIGH);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        break;
    /* --------------- Generic SD Error --------------- */
    case SD_ERROR:
        digitalWrite(STATUS_LED_RED, LOW);
        digitalWrite(STATUS_LED_GREEN, LOW);
        break;
    }
}

/**
 * @brief Changes the status lights based on the current status mode.
 * This is done by first changing the global currMode variable, changing the status lights
 * and then doing mode specific tasks
 * @param newMode new status mode code
 */
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
        updateCurrentTime();
    }
}

/**
 * @brief Gives a 3 second delay between button press and state change while also blinking the status LEDs
 *
 */
void buttonIntermission()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(STATUS_LED_RED, HIGH);
        digitalWrite(STATUS_LED_GREEN, LOW);
        delay(500);
        digitalWrite(STATUS_LED_RED, LOW);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        delay(500);
    }
}

/* ----------------------------------------------------------------------------------------- */
/* ------------------------------------ DATA COLLECTION ------------------------------------ */
/* ----------------------------------------------------------------------------------------- */

/**
 * @brief Given a pin number, reads the voltage on it using analogRead
 * and then converts it to force in grams using voltage to force model.
 *
 * @param pinNum Analog pin to read voltage from
 * @return long Force value in grams
 */
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

/**
 * @brief Constructs the Monitoring Data String by reading the force on the THUMB and PALM using the
 * getVoltageToForce function, and stores it in a JSON string of the following format:\n
 * { THUMB: <THUMB force value in g>, PALM: <PALM force value in g> }
 *
 * @return JSON string with THUMB and PALM force value
 */
String getMonitoringDataString()
{
    return "{\"Thumb\":" + String(getVoltageToForce(THUMB)) + ",\"Palm\":" + String(getVoltageToForce(PALM)) + "}";
}

//
// Initializes the starting session time using RTC and stored in rtc_set_ms
//
/**
 * @brief Initializes the session time by doing the following:\n
 * Read the RTC Time. Set the year, month and day. Then, use millis() to see how many milliseconds() have passed since
 * turn on and store it in initial_ms, and save the rtc_set_ms global variable to be the current time in milliseconds
 *
 */
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

/**
 * @brief Given a time in milliseconds, convert it into hours, minutes, seconds and milliseconds
 * and store each of them in the corresponding indices of the global array curr_ms
 *
 * @param ms Current time as a total of milliseconds
 */
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

/**
 * @brief Gets the current time from the RTC Module and updates the global
 * curr_time variable accordingly
 *
 */
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

/**
 * @brief Uses the global variables rtc_set_ms and initial_ms with millis() to find the current time in milliseconds.
 * Then, uses getTimeFromMillis to update the global variable curr_time accordingly
 *
 */
void updateCurrentMs()
{
    unsigned long curr_ms = rtc_set_ms + (millis() - initial_ms); //millis() - initial_ms = time passed after setting initial time
    getTimeFromMillis(curr_ms);
}


/**
 * @brief Constructs and returns a data string with the time, THUMB force in grams, PALM force in grams, and the glove hand in the following format:\n
 * <YYYY:MM:DD:HH:MM:SS:MS>, <THUMB force in g>, <PALM force in g>, <"LEFT" or "RIGHT">
 *
 * @return String The Data String
 */
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
/* ------------------------------------ DATA STORAGE ------------------------------------ */
/* -------------------------------------------------------------------------------------- */

/**
 * @brief Generates a session file for data in the following format:\n
 * YYYY_MM_DD_#<sessionNumber>.csv\n
 * and then writes the header to the session file
 *
 */
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

/**
 * @brief Writes headers to the file for the columns (RTC, Thumb, Palm, Hand)
 *
 */
void writeHeaderToFile()
{
    // write data headers
    dataFile.println("RTC,Thumb,Palm,Hand");
    Serial.println("Writing headers");
}

/**
 * @brief Writes a data string to the file. Gets the data string using the getDataString() function
 *
 */
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

/**
 * @brief Generate a status and battery JSON string to be sent over BLE
 *
 * @return String String with status and battery in the format: '{ "state": <state>, "battery": <battery> }'
 */
String getStatusBatteryJSONString()
{
    String statusString = "";
    if (currMode == RECORDING_MODE)
    {
        statusString = "Recording";
    }
    else if (currMode == STANDBY_MODE)
    {
        statusString = "Standby";
    }
    else if (currMode == RTC_ERROR)
    {
        statusString = "RTC Error";
    }
    else if (currMode == SD_ERROR)
    {
        statusString = "SD Error";
    }
    // TODO: Need to add battery reading
    return "{\"state\":\"" + statusString + "\", \"battery\":100}";
}

/**
 * @brief Sets RGB LED with given red and green values
 *
 * @param red_light_value value to write to R value of RGB LED
 * @param green_light_value value to write to G value of RGB LED
 */
void RGB_color(int red_light_value, int green_light_value)
{
    analogWrite(RGB_RED, red_light_value);
    analogWrite(RGB_GREEN, green_light_value);
}

/**
 * @brief Reads the battery value and set battery LED accordingly
 *
 */
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
/**
 * @brief Arduino setup() function called once on startup
 *
 *
 * The following steps are performed:\n
 * - State is changed to STANDBY_MODE
 * - RTC (Real Time Clock) is initialized. If any errors occured, mode is set to RTC_ERROR
 * - Arduino is advertised over BLE, and services and characteristics are added
 * - SD Card is initialized. If SD Card is not present, or there is any error in initialization, mode is set to SD_ERROR
 *
 */
void setup()
{
    Serial.begin(9600);
#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
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

    // Reset RTC Time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

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
/**
 * @brief The main infinite Arduino loop which is constantly being run
 *
 *
 * The following steps are performed:
 * - Read button state, and change state when button is pressed
 * - If current mode is RECORDING_MODE, write data to the file onto the SD card
 * - Update the battery indicator LED
 * - Using the global variable last_ble_time, do the following BLE related tasks whenever BLE_TIME_INTERVAL_MS milliseconds pass:
 *  - Check if central device is connected
 *  - If new value of RTC was written, update the RTC time (as long as not currently recording)
 *  - Send current status and battery to central device
 *  - Send monitoring data to central device if currently recording
 * - Using the loopStartTime stored in the beginning of every loop, wait until DELAY_PER_LOOP milliseconds have passed
 */
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
        updateCurrentMs();
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
                        rtc.adjust(DateTime(rtcString)); // in ISO 8061 format
                        Serial.println("RTC Time Reset to: " + String(rtc.now().timestamp(DateTime::TIMESTAMP_FULL)));
                        updateCurrentTime();
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
