#include "StateMachine.h"

StateMachine StateMachine;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
}

void loop() {
  // Run the state machine
  StateMachine.SM();
}
