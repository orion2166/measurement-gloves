//#include <Arduino_LSM9DS1.h> // For IMU Stuff
//#include <ArduinoBLE.h>      // For BLE Stuff
#include "RTClib.h"
#include <SD.h>

/* --------SD CONSTANTS --------- */
#define SD_CARD D9
/* --------- RTC CONSTANTS --------- */
RTC_PCF8523 rtc;
/* --------- FORCE SENSOR CONSTANTS --------- */
#define THUMB_1 A0
#define PALM_1 A1
#define PALM_2 A2
/* --------- STTUS LED CONSTANTS --------- */
#define STATUS_LED_WHITE D2
#define STATUS_LED_GREEN D3
/* --------- BUTTON CONSTANTS --------- */
#define BUTTON D4
int buttonState = 0;
/* --------- STATE CONSTANTS --------- */
#define STANDBY_MODE 0          // White On | Green Off
#define RECORDING_MODE 1        // White Off | Green On
// ERRORS
#define RTC_ERROR 2
#define SD_ERROR 3

// Global State Variable
int currMode;
int curr_time[4]; //[0]: hour, [1]: min, [2]: sec, [3]: millisec
unsigned long initial_ms; //ms passed until rtc was set
unsigned long rtc_set_ms; //the time (hr, min, sec) when rtc was set (in milliseconds)
String fileName = String();	// current file we will be writing to
unsigned int sessionNumber = 1;	// keep track of session number
/* ------------------------------------------------------------------------------------------------------------------------ */
/* --------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------ Glove State Intermission ------------------------------ */
void buttonIntermission()
{
    for (int i=0; i<3; i++) {
      digitalWrite(STATUS_LED_WHITE,HIGH);
      digitalWrite(STATUS_LED_GREEN,LOW);
      delay(500);
      digitalWrite(STATUS_LED_WHITE,LOW);
      digitalWrite(STATUS_LED_GREEN,HIGH);
      delay(500);
    }
}

/* ------------------------------ Glove State Handler ------------------------------ */
void changeState(int newMode) {

    Serial.print("Mode = ");
    Serial.print(newMode);
    Serial.println();

    currMode = newMode;

    switch(currMode){
      /* --------------- Set Standby Mode --------------- */
      case STANDBY_MODE:
        // Set Status Lights
        digitalWrite(STATUS_LED_WHITE,HIGH);
        digitalWrite(STATUS_LED_GREEN,LOW);
        break;
      /* --------------- Set Recording Mode --------------- */
      case RECORDING_MODE:
        // Set Status Lights
        digitalWrite(STATUS_LED_WHITE, LOW);
        digitalWrite(STATUS_LED_GREEN, HIGH);
        // Create new session file
        generateSessionFile();
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

/* ------------------------------ Collect RTC + Force Values ------------------------------ */
String getValuesString()
{
    unsigned long curr_ms = rtc_set_ms + (millis() - initial_ms); //millis() - initial_ms = time passed after setting initial time
    formatMS(curr_ms);
    String toReturn = "{\"Time\":" + String(now.year()) + ":"
      + String(now.month()) + ":"
      + String(now.day()) + ":"
      + String(curr_time[0]) + ":"  //hour
      + String(curr_time[1]) + ":" //min
      + String(curr_time[2]) + ":"  //sec
      + String(curr_time[3]) + ":"  //millisec
      + String() + ":"
      + ",";
    toReturn += "\"THUMB 1\":" + String(analogRead(THUMB_1)) + ",";
    toReturn += "\"PALM 1\":" + String(analogRead(PALM_1)) + ",";
    toReturn += "\"PALM 2\":" + String(analogRead(PALM_2)) + "}";
    return toReturn;
}

/* ------------------------------ Collect RTC + Force Values ------------------------------ */
void printVals()
{
  String Vals = getValuesString();
  Serial.print(Vals);
  Serial.print("\n");
  Serial.print("\n");
}
/* ------------------------------ Append milliseconds to RTC ----------------------------------- */
/*call set_time when starting recording*/
void set_time(){
  //Set initial time with RTC
  DateTime start_time = rtc.now(); //year|month|day|hour|min|sec
  initial_ms = millis();

  //convert hr,min,sec to milliseconds
  unsigned long start_hr = start_time.hour() * 3.6 * pow(10, 6); //in milliseconds
  unsigned long start_min = start_time.minute() * 60000;
  unsigned long start_sec = start_time.second() * 1000;
  rtc_set_ms = start_hr + start_min + start_sec;
}
void formatMS(unsigned long ms){ //converts milliseconds to format of hours, minutes, seconds, milliseconds
  unsigned long currMs = ms;
  unsigned long seconds = currMs / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  currMs %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  curr_time[0] = hours;
  curr_time[1] = minutes;
  curr_time[2] = seconds;
  curr_time[3] = currMs;
}

/* ------------------------------ Create new session file  ------------------------------ */
void generateSessionFile() {
// check there is SD card connected
  if (SD.begin(SD_CARD)) {
    String message = String();
    DateTime now = rtc.now();

    fileName = String(now.year()) + "_"
      + String(now.month()) + "_"
      + String(now.day()) + "_#";
    fileName += sessionNumber;
    fileName += ".csv";
    message = fileName;
    char charFileName[fileName.length() + 1];
    fileName.toCharArray(charFileName, sizeof(charFileName));

    // generate new session file if already exists
    if (SD.exists(charFileName)) {
      message += " exists.";
      sessionNumber++;
    }
    else {
      File dataFile = SD.open(charFileName, FILE_WRITE);
      // write headers to file
      dataFile.println("RTC,Thumb,Grip,Hand");
      message += " created.";
      dataFile.close();
    }
    Serial.println(message);
  }
  else {
    Serial.println("SD card missing or failure");
    // throw error
    changeState(SD_ERROR);
  }
}
void writeToFile() {
	char charFileName[fileName.length() + 1];
  fileName.toCharArray(charFileName, sizeof(charFileName));
  dataFile = SD.open(charFileName, FILE_WRITE);
  if (dataFile) {
      String Vals = getValuesString();
      // convert string to char array before writing
      char charVals[Vals.length() + 1];
      Vals.toCharArray(charVals, sizeof(charVals));
      dataFile.println(charVals);
      dataFile.close();
    }
  else {
    Serial.println("Error opening file");
    changeState(SD_ERROR);
  }
}
/* ------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- SETUP --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------- */
void setup() {
  Serial.begin(9600);

  /* --------- Initialize Standy Mode --------- */
  changeState(STANDBY_MODE);

  /* --------- Initialize RTC Module --------- */
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {

    // Console Print
    Serial.println("Couldn't find RTC");

    // Caller Error Handler - RTC WIRING
    changeState(RTC_ERROR);
    Serial.flush();
    abort();

  }

  if (! rtc.initialized() || rtc.lostPower()) {

    changeState(RTC_ERROR);

    Serial.println("RTC is NOT initialized, let's set the time!");
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
  }
  pinMode(SD_CARD, OUTPUT);
}


/* ----------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- MAIN LOOP --------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------------- */
void loop() {

  /* --------- Change State on Button Press --------- */
  if(digitalRead(BUTTON) == HIGH && currMode != RTC_ERROR && currMode != SD_ERROR) {
    // Console Print
    Serial.println("\nState Change !!!");
    // Hold Button Input
    buttonIntermission();
    // Toggle State
    if (currMode == STANDBY_MODE) { changeState(RECORDING_MODE); }
    else { changeState(STANDBY_MODE); }
  }

  /* --------- Collect Values if in Recording Mode --------- */
  if (currMode == RECORDING_MODE){  // Recording Mode
    set_time();
    //printVals();
    writeToFile();
  }

  // Delay Loop
//  delay(50);
  delay(200); // 5 readings a second
}
