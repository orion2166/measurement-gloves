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
#define STATUS_LED_1 D2
#define STATUS_LED_2 D3
/* --------- BUTTON CONSTANTS --------- */
#define BUTTON D4
int buttonState = 0;
/* --------- STATE CONSTANTS --------- */
#define STANDBY_MODE 0
#define RECORDING_MODE 1
#define ERROR_MODE 2
int currMode = 0;

/* ------------------------------ Change State Function ------------------------------ */
void changeState()
{
    switch(currMode){
      case 0:
        currMode = RECORDING_MODE;
        Serial.print("Mode = Recording Mode\n");
        break;
      case 1:
        currMode = STANDBY_MODE;
        Serial.print("Mode = Standby Mode\n");
        Serial.println();
        break;
    }

    for (int i=0; i<3; i++) {
      digitalWrite(STATUS_LED_1,HIGH);
      digitalWrite(STATUS_LED_2,LOW);
      delay(500);
      digitalWrite(STATUS_LED_1,LOW);
      digitalWrite(STATUS_LED_2,HIGH);
      delay(500);
    }
}


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

void printForceVals()
{
  String Vals = getValuesString();
  Serial.print(Vals);
  Serial.print("\n");
  Serial.print("\n");
}

void niceDelay(long delayTime)
{
    delay(delayTime);
}

void setup() {
  Serial.begin(9600);
  
  /* --------- STATUS LED SETUP --------- */
  pinMode(STATUS_LED_1, OUTPUT);
  pinMode(STATUS_LED_2, OUTPUT);

  /* --------- RTC MODULE SETUP --------- */
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
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
  }
  
}

void loop() {
  
  // If button is pressed, change the state
  if(digitalRead(BUTTON) == HIGH) {
    Serial.print("State Change !!!");
    Serial.print("\n");
    changeState();
  }
  
  if(currMode == STANDBY_MODE) {
    digitalWrite(STATUS_LED_1,HIGH);
    digitalWrite(STATUS_LED_2,HIGH);
  }
  else if (currMode == RECORDING_MODE){
    digitalWrite(STATUS_LED_1,HIGH);
    digitalWrite(STATUS_LED_2,LOW);
    printForceVals();
  }
  else {
    digitalWrite(STATUS_LED_1,LOW);
    digitalWrite(STATUS_LED_2,LOW);
  }
  
  // Delay Loop
  niceDelay(50);
}
