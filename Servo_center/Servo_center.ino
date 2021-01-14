#include <ESP32Servo.h>

Servo horizontal; // horizontal servo
int servoh = 90;     // stand horizontal servo

Servo vertical;   // vertical servo 
int servov = 90;     // stand vertical servo

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
// servo connections
// name.attacht(pin);
  horizontal.attach(12); 
  vertical.attach(27);
}

void loop() {
  vertical.write(servov);
  //delay(100);
  horizontal.write(servoh);
}
