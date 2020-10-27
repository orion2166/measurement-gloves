//#include <Arduino_LSM9DS1.h> // For IMU Stuff
//#include <ArduinoBLE.h>      // For BLE Stuff

// Set constants for flex analog inputs
#define THUMB_1 A0
#define PALM_1 A1
#define PALM_2 A2

#define STATUS_LED_1 D2
#define STATUS_LED_2 D3

#define BUTTON D4
int buttonState = 0;

#define STANDBY_MODE 0
#define RECORDING_MODE 1
#define ERROR_MODE 2
int currMode = 0;

void changeState(currMode)
{
    switch(currMode){
      case 0:
        currMode = RECORDING_MODE;
        break;
      case 1:
        currMode = STANDBY_MODE;
        break;
//       case 2:
//        break;
    }
}

String getForceValuesString()
{
    String toReturn = "{\"Time\":" + String(millis()) + ",";
    toReturn += "\"THUMB 1\":" + String(analogRead(THUMB_1)) + ",";
    toReturn += "\"PALM 1\":" + String(analogRead(PALM_1)) + ",";
    toReturn += "\"PALM 2\":" + String(analogRead(PALM_2)) + "}";
    return toReturn;
}

void printForceVals()
{
  String forceVals = getForceValuesString();
  Serial.print(forceVals);
  Serial.print("\n");
  Serial.print("\n");
}

void niceDelay(long delayTime)
{
    delay(delayTime);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(STATUS_LED_1, OUTPUT);
  pinMode(STATUS_LED_2, OUTPUT);
}

void loop() {
  // LED Test
  // Turn Status Light 1 On
//  digitalWrite(STATUS_LED_1, OUTPUT);
//  digitalWrite(STATUS_LED_2, OUTPUT);

  // Force Value Test
  // Print Force Values
//  String forceVals = getForceValuesString();
//  Serial.print(forceVals);
//  Serial.print("\n");
//  Serial.print("\n");

  


  // Button Test
  buttonState = digitalRead(BUTTON);
  
  if(buttonState == HIGH) {
    changeState();
    delay(5000);
  }
  
  if(currMode = STANDBY_MODE) {
    digitalWrite(STATUS_LED_1,HIGH);
    digitalWrite(STATUS_LED_2,HIGH);
  }
  else if (currMode = RECORDING_MODE){
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
