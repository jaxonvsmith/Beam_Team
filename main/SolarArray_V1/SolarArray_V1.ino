#include "StateMachine.h"

StateMachine StateMachine;

//VARIABLE DECLORATION
//...motors...
uint8_t Azmuth_Motor_Pos;
uint8_t Azmuth_Motor_Neg;
uint8_t Azmuth_Motor_PWM;
uint8_t Horizon_Motor_Pos;
uint8_t Horizon_Motor_Neg;
uint8_t Horizon_Motor_PWM;
uint8_t Deploy_Motor_Pos;
uint8_t Deploy_Motor_Neg;
uint8_t Deploy_Motor_PWM;
//...limitswitch's...
uint8_t LimitSwitch_Azmuth_Upper;
uint8_t LimitSwitch_Azmuth_Lower;
uint8_t LimitSwitch_Horizon;
uint8_t LimitSwitch_Center;
uint8_t LimitSwitch_Deploy;
uint8_t LimitSwitch_Retract;


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  StateMachine.SM();
}
