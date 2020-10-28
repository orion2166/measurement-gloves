#define THUMB A0
#define PALM A1


//long getVoltageToForcePalm(int pinNum)
//{
//  int fsrPalm;                   // analog reading from fsr resistor divider
//  int voltPalm;                  // the analog reading converted to voltage
//  unsigned long resistancePalm;  // the voltage converted to resistance
//  unsigned long conductancePalm;
//  long forcePalm;
//
//  fsrPalm = analogRead(pinNum);
//
//  // palm voltage reading ranges from 0 to 300 which maps to 0V to 3.3V (3300mV)
//  voltPalm = map(fsrPalm, 0, 300, 0, 3300);
//
//  // calculate resistance = ((Vcc - V) * R) / V)
//  // Vcc = 3300mV, R = 1000 Ohms
//  if (voltPalm == 0)
//  {
//    forcePalm = 0;
//    return forcePalm;
//  }
//  else
//  {
//    resistancePalm = 3300 - voltPalm;
//    resistancePalm *= 1000;
//    resistancePalm /= voltPalm;
//
//    // conductance = 1/resistance, linear correlation with force
//    conductancePalm = 1000000;    // since we measured in microOhms
//    conductancePalm /= resistancePalm;
//
//    forcePalm = calibrateConductance(conductancePalm);
//    return forcePalm;
//  }
//}
//
//long getVoltageToForceThumb(int pinNum)
//{
//  int fsrThumb;                   // analog reading from fsr resistor divider
//  int voltThumb;                  // the analog reading converted to voltage
//  unsigned long resistanceThumb;  // the voltage converted to resistance
//  unsigned long conductanceThumb;
//  long forceThumb;
//
//  fsrThumb = analogRead(pinNum);
//
//  // thumb voltage reading ranges from 0 to 1023 which maps to 0V to 3.3V (3300mV)
//  voltThumb = map(fsrThumb, 0, 1023, 0, 3300);
//
//  // calculate resistance = ((Vcc - V) * R) / V)
//  // Vcc = 3300mV, R = 1000 Ohms
//  if (voltThumb == 0)
//  {
//    forceThumb = 0;
//    return forceThumb;
//  }
//  else
//  {
//    resistanceThumb = 3300 - voltThumb;
//    resistanceThumb *= 1000;
//    resistanceThumb /= voltThumb;
//
//    // conductance = 1/resistance, linear correlation with force
//    conductanceThumb = 1000000;
//    conductanceThumb /= resistanceThumb;
//
//    forceThumb = calibrateConductance(conductanceThumb);
//    return forceThumb;
//  }
//}
//
//long calibrateConductance(unsigned long conductanceVal)
//{
//  long forceVal;
//  forceVal = conductanceVal / 100;   // needs testing
//  return forceVal;
//}
//
void setup(void)
{
  Serial.begin(9600);
}

void loop(void)
{
//  Serial.println(getVoltageToForceThumb(THUMB_1));

  Serial.println(analogRead(THUMB));
  Serial.println(analogRead(PALM));
  delay(200);
}
