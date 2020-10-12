//#include <Arduino_LSM9DS1.h> // For IMU Stuff
//#include <ArduinoBLE.h>      // For BLE Stuff

// Set constants for flex analog inputs
#define THUMB_1 A0
#define PALM_1 A1
#define PALM_2 A2

#define STATUS_LED_1 D2

String getForceValuesString()
{
    String toReturn = "{\"Time\":" + String(millis()) + ",";
    toReturn += "\"THUMB 1\":" + String(analogRead(THUMB_1)) + ",";
    toReturn += "\"PALM 1\":" + String(analogRead(PALM_1)) + ",";
    toReturn += "\"PALM 2\":" + String(analogRead(PALM_2)) + "}";
    return toReturn;
}

void niceDelay(long delayTime)
{
    delay(delayTime);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(STATUS_LED_1, OUTPUT);
}

void loop() {
  // Turn Status Light 1 On
  digitalWrite(STATUS_LED_1, OUTPUT);
  
  // Print Force Values
  String forceVals = getForceValuesString();
  Serial.print(forceVals);
  Serial.print("\n");
  Serial.print("\n");

  // Delay Loop
  niceDelay(50);
}
