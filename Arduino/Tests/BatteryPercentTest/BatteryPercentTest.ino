int battery_pin = A7;

float battery_read()
{
    //read battery voltage per %
    long sum = 0;                   // sum of samples taken
    float voltage = 0.0;            // calculated voltage
    float output = 0.0;             //output value
    const float battery_max = 4.20; //maximum voltage of battery
    const float battery_min = 3.0;  //minimum voltage of battery before shutdown

    for (int i = 0; i < 500; i++)
    {
        sum += analogRead(battery_pin);
        delayMicroseconds(1000);
    }
    // calculate the voltage
    voltage = sum / (float)500;
    // voltage = (voltage * 5.0) / 1023.0; //for default reference voltage
    voltage = (voltage * 1.1) / 1023.0; //for internal 1.1v reference
    //round value by two precision
    voltage = roundf(voltage * 100) / 100;
    Serial.print("voltage: ");
    Serial.println(voltage, 2);
    output = ((voltage - battery_min) / (battery_max - battery_min)) * 100;
    if (output < 100)
        return output;
    else
        return 100.0f;
}

void setup()
{
//    analogReference(INTERNAL); //set reference voltage to internal
    Serial.begin(9600);
}

void loop()
{
  // read the input on analog pin 0:
  int sensorValue = analogRead(battery_pin);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (9.0 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);
    delay(1000);
}
