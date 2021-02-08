//#include <ESP32Servo.h>
#include <Servo.h>

Servo horizontal; // horizontal servo
int servoh = 90;     // stand horizontal servo

Servo vertical;   // vertical servo 
int servov = 90;     // stand vertical servo

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
// servo connections
// name.attacht(pin);
  horizontal.attach(52);
  vertical.attach(50);
}

void loop() {
  vertical.write(servov);
  //delay(100);
  horizontal.write(servoh);
}
