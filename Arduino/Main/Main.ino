//#include <Arduino_LSM9DS1.h> // For IMU Stuff
//#include <ArduinoBLE.h>      // For BLE Stuff
#include "RTClib.h"

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
#define SENSOR_ERROR_WIRING 2     // White Blinking | Green Blinking
#define RTC_ERROR_WIRING 3      // White On | Green On
#define RTC_ERROR_NO_TIME 4     // White Off | Green Off
#define SD_ERROR_WIRING 5       // White Blinking | Green On
#define SD_ERROR_NO_CARD 6      // White On | Green Blinking

// Global State Variable
int currMode;

/* ------------------------------------------------------------------------------------------------------------------------ */
/* --------------------------------------------------- HELPER FUNCTIONS --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------------------ */

/* ------------------------------ Glove State Handler ------------------------------ */
void changeState()
{
    for (int i=0; i<3; i++) {
      digitalWrite(STATUS_LED_WHITE,HIGH);
      digitalWrite(STATUS_LED_GREEN,LOW);
      delay(500);
      digitalWrite(STATUS_LED_WHITE,LOW);
      digitalWrite(STATUS_LED_GREEN,HIGH);
      delay(500);
    }
    
    switch(currMode){
      case 0:                                             // Recording Mode
        // Set Global Variable
        currMode = RECORDING_MODE;
        // Set Status Lights
        digitalWrite(STATUS_LED_WHITE,LOW);
        digitalWrite(STATUS_LED_GREEN,HIGH);
        // Print to Console
        Serial.print("Mode = Recording Mode\n");
        break;
      case 1:                                             // Standby Mode
        // Set Global Variable
        currMode = STANDBY_MODE;
        // Set Status Lights
        digitalWrite(STATUS_LED_WHITE,HIGH);
        digitalWrite(STATUS_LED_GREEN,LOW);
        // Print to Console
        Serial.print("Mode = Standby Mode\n");
        Serial.println();
        break;
      case 2:                                             // Sensor Error
        while(1){
          // White Blink
          // Green Blink
        }
        break;
      case 3:                                             // RTC Error - Wiring
        while(1){
          digitalWrite(STATUS_LED_WHITE,HIGH);
          digitalWrite(STATUS_LED_GREEN,HIGH);
        }
        break;
      case 4:                                             // RTC Error - Missing Time
        while(1){
          digitalWrite(STATUS_LED_WHITE,LOW);
          digitalWrite(STATUS_LED_GREEN,LOW);
        }
        break;
      case 5:                                             // SD Error - Wiring
        while(1){
          // White Blinking | Green On
        }
        break;
      case 6:                                             // SD Error - Missing Card
        while(1){
          // White On | Green Blinking
        }
        break;
    }
}

/* ------------------------------ Collect RTC + Force Values ------------------------------ */
String getValuesString()
{
    DateTime now = rtc.now();
//    String toReturn = "{\"Time\":" + String(millis()) + ",";
    String toReturn = "{\"Time\":" + String(now.year()) + ":" 
      + String(now.month()) + ":"
      + String(now.day()) + ":"
      + String(now.hour()) + ":"
      + String(now.minute()) + ":"
      + String(now.second()) + ":"
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

/* ------------------------------ Error Handler ------------------------------ */


/* ------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- SETUP --------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------------------- */
void setup() {
  Serial.begin(9600);
  
  /* --------- Initialize Standy Mode --------- */
  currMode = STANDBY_MODE;
  changeState();

  /* --------- Initialize RTC Module --------- */
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    // Console Stuff
    Serial.println("Couldn't find RTC");
    // Caller Error Handler - RTC WIRING
    currMode = RTC_ERROR_WIRING;
    changeState();
    Serial.flush();
    abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
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

    // Caller Error Handler - RTC TIME
//    currMode = RTC_ERROR_NO_TIME;
//    changeState();
  }
  
}


/* ----------------------------------------------------------------------------------------------------------------- */
/* --------------------------------------------------- MAIN LOOP --------------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------------- */
void loop() {
  
  /* --------- Change State on Button Press --------- */
  if(digitalRead(BUTTON) == HIGH) {
    Serial.print("State Change !!!");
    Serial.print("\n");
    changeState();
  }
  /* --------- Collect Values if in Recording Mode --------- */
  if (currMode == RECORDING_MODE){  // Recording Mode
    printVals();
  }
  
  // Delay Loop
  delay(50);
}
