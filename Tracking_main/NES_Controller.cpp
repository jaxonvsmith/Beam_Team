/*
  NES_Controller.cpp - Library for managing the detatchable Controller.
  Created by Jaxon Smith on 2/24/2021
*/

#include "motors.h"
#include "Arduino.h"

motors::motors() {
  pinMode(Azmuth_Motor_Pos, OUTPUT);
  pinMode(Azmuth_Motor_Neg, OUTPUT);
  pinMode(Elevation_Motor_Pos, OUTPUT);
  pinMode(Elevation_Motor_Neg, OUTPUT);
  pinMode(Deploy_Motor_Pos, OUTPUT);
  pinMode(Deploy_Motor_Neg, OUTPUT);
  //set motor speed
  //analogWrite(Azmuth_Motor_PWM, MOTOR_SPEED);
  //analogWrite(Horizon_Motor_PWM, MOTOR_SPEED);
  //analogWrite(Deploy_Motor_PWM, MOTOR_SPEED);

  //Servo Stuff
  servov = 90;     // stand vertical servo
  servoh = 90;     // stand horizontal servo
}
