#define BATTERY A4

#define RGB_RED A7
#define RGB_GREEN A6
#define RGB_BLUE A5

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(RGB_RED, red_light_value);
  analogWrite(RGB_GREEN, green_light_value);
  analogWrite(RGB_BLUE, blue_light_value);
}

void set_battery(){
 // read analog voltage from battery
  int sensorValue = analogRead(BATTERY);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float V = sensorValue * (3.3 / 1023.0);
  Serial.println(voltage);
  
  float percentage = voltage / 3.0 * 100;
  // print out the value you read:
  Serial.println(String(percentage) + "%");

  if (V >= 2.65) { RGB_color(255,0,255); }
  if (V < 2.65 && V >= 2.5) { RGB_color(0,0,255); }
  if (V < 2.5) { RGB_color(0,255,255); }
}

void setup()
{
  Serial.begin(9600);

  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
}

void loop()
{
  set_battery();
  delay(200);
}
