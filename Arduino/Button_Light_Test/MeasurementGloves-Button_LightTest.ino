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

//  int buttonChanged = 0;
  // If button is pressed, change the state
  if(digitalRead(BUTTON) == HIGH) {
    Serial.print("State Change !!!");
    Serial.print("\n");
    changeState();
//    buttonChanged = 1;
//    delay(5000);
  }
//  Serial.print(buttonChanged);
  
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
