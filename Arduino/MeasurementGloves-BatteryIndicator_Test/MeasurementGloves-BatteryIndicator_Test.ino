#define cellPin A0

const float mvpc = 4.55 ; //measured voltage of arduino through voltmeter
float counts = 0;  //battery volts in millivolts
float mv = 0;
float multiplier = 2;
float output = 0;
int charge = 0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
counts = analogRead(cellPin);
Serial.println(counts);

mv = counts * mvpc;
Serial.println(mv);

output = (mv * multiplier)/1000 ;
Serial.print(output);
Serial.println("V");

charge = (counts/1024)*100;
Serial.print(charge);
Serial.println("%");

delay(1000);

}
