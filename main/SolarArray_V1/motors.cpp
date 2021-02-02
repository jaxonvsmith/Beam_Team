/*
  motors.cpp - Library for managing the motors.
  Created by Jaxon Smith on 2/1/2021
*/

#include "motors.h"
#include "Arduino.h"

motors::motors() {
  pinMode(Azmuth_Motor_Pos, OUTPUT);
  pinMode(Azmuth_Motor_Neg, OUTPUT);
  pinMode(Horizon_Motor_Pos, OUTPUT);
  pinMode(Horizon_Motor_Neg, OUTPUT);
  pinMode(Deploy_Motor_Pos, OUTPUT);
  pinMode(Deploy_Motor_Neg, OUTPUT);
  //set motor speed
  //analogWrite(Azmuth_Motor_PWM, MOTOR_SPEED);
  //analogWrite(Horizon_Motor_PWM, MOTOR_SPEED);
  //analogWrite(Deploy_Motor_PWM, MOTOR_SPEED);
}

void motors::Azmuth_Motor_Up() {
  digitalWrite(Azmuth_Motor_Pos, HIGH);
  digitalWrite(Azmuth_Motor_Neg, LOW);
}
void motors::Azmuth_Motor_Down() {
  digitalWrite(Azmuth_Motor_Pos, LOW);
  digitalWrite(Azmuth_Motor_Neg, HIGH);
}
void motors::Azmuth_Motor_Off() {
  digitalWrite(Azmuth_Motor_Pos, LOW);
  digitalWrite(Azmuth_Motor_Neg, LOW);
}
void motors::Horizon_Motor_Right() {
  digitalWrite(Horizon_Motor_Pos, LOW);
  digitalWrite(Horizon_Motor_Neg, LOW);
}
void motors::Horizon_Motor_Left() {
  digitalWrite(Horizon_Motor_Pos, LOW);
  digitalWrite(Horizon_Motor_Neg, HIGH);
}
void motors::Horizon_Motor_Off() {
  digitalWrite(Horizon_Motor_Pos, LOW);
  digitalWrite(Horizon_Motor_Neg, LOW);
}
void motors::Deploy_Motor_Out() {
  digitalWrite(Horizon_Motor_Pos, HIGH);
  digitalWrite(Horizon_Motor_Neg, LOW);
}
void motors::Deploy_Motor_In() {
  digitalWrite(Horizon_Motor_Pos, LOW);
  digitalWrite(Horizon_Motor_Neg, HIGH);
}
void motors::Deploy_Motor_Off() {
  digitalWrite(Horizon_Motor_Pos, LOW);
  digitalWrite(Horizon_Motor_Neg, LOW);
}
