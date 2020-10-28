#define THUMB_1 A0
#define PALM_1 A1
#define PALM_2 A2

long getVoltageToForce(int pinNum)
{
  int fsr;                   // analog reading from fsr resistor divider
  double forceVal;
  fsr = analogRead(pinNum);  // voltage output mapped 0 - 1023
  // Force = 36.1e^(0.00498x) from model
  if (fsr == 0) {
    forceVal = 0;
    return forceVal;
  }
  else {
    forceVal = 0.00498*fsr;
    forceVal = 36.1*exp(forceVal);
  }
  return forceVal;
}

// ------------------------TEST--------------------------
String getForceValuesString()
{
    String toReturn = "{\"Time\":" + String(millis()) + ",";
    toReturn += "\"THUMB 1\":" + String(getVoltageToForce(THUMB_1)) + ",";
    toReturn += "\"PALM 1\":" + String(getVoltageToForce(PALM_1)) + ",";
    toReturn += "\"PALM 2\":" + String(getVoltageToForce(PALM_2)) + "}";
    return toReturn;
}

void printForceVals()
{
  String forceVals = getForceValuesString();
  Serial.print(forceVals);
  Serial.print("\n");
  Serial.print("\n");
}
// -------------------------------------------------------

void setup(void)
{
  Serial.begin(9600);
}

void loop(void)
{
  printForceVals();
  delay(50);
}
