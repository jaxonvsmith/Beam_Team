/*
  motors.cpp - Library for managing the motors.
  Created by Jaxon Smith on 2/1/2021
*/

#include "motors.h"
#include "Arduino.h"

motors::motors() {
  pinMode(Azimuth_Motor_Pos, OUTPUT);
  pinMode(Azimuth_Motor_Neg, OUTPUT);
  pinMode(Elevation_Motor_Pos, OUTPUT);
  pinMode(Elevation_Motor_Neg, OUTPUT);
  pinMode(Deploy_Motor_Pos, OUTPUT);
  pinMode(Deploy_Motor_Neg, OUTPUT);
  //set motor speed
  //analogWrite(Azmuth_Motor_PWM, MOTOR_SPEED);
  //analogWrite(Horizon_Motor_PWM, MOTOR_SPEED);
  //analogWrite(Deploy_Motor_PWM, MOTOR_SPEED);
}
void motors::INIT() {
}

void motors::Azimuth_Motor_CW() {
  Serial.print("Motor CW\n");
  digitalWrite(Azimuth_Motor_Pos, HIGH);
  digitalWrite(Azimuth_Motor_Neg, LOW);
}
void motors::Azimuth_Motor_CCW() {
  Serial.print("Motor CCW\n");
  digitalWrite(Azimuth_Motor_Pos, LOW);
  digitalWrite(Azimuth_Motor_Neg, HIGH);
}
void motors::Azimuth_Motor_Off() {
  Serial.print("Azimuth Motor Off(3)\n");
  digitalWrite(Azimuth_Motor_Pos, LOW);
  digitalWrite(Azimuth_Motor_Neg, LOW);
}
void motors::Elevation_Motor_Up() {
  Serial.print("Motor Up\n");
  digitalWrite(Elevation_Motor_Pos, HIGH);
  digitalWrite(Elevation_Motor_Neg, LOW);
}
void motors::Elevation_Motor_Down() {
  Serial.print("Motor Down\n");
  digitalWrite(Elevation_Motor_Pos, LOW);
  digitalWrite(Elevation_Motor_Neg, HIGH);
}
void motors::Elevation_Motor_Off() {
  digitalWrite(Elevation_Motor_Pos, LOW);
  digitalWrite(Elevation_Motor_Neg, LOW);
}
void motors::Deploy_Motor_Out() {
  digitalWrite(Deploy_Motor_Pos, HIGH);
  digitalWrite(Deploy_Motor_Neg, LOW);
}
void motors::Deploy_Motor_In() {
  digitalWrite(Deploy_Motor_Pos, LOW);
  digitalWrite(Deploy_Motor_Neg, HIGH);
}
void motors::Deploy_Motor_Off() {
  digitalWrite(Deploy_Motor_Pos, LOW);
  digitalWrite(Deploy_Motor_Neg, LOW);
}
