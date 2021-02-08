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

  //Servo Stuff
  
  horizontal.attach(52);
  servoh = 90;     // stand horizontal servo
  horizontal.write(servoh);

  vertical.attach(50);
  servov = 90;     // stand vertical servo
  vertical.write(servov);
}

void motors::Azmuth_Motor_Up() {
  Serial.print("Motor Up\n");
  digitalWrite(Azmuth_Motor_Pos, HIGH);
  digitalWrite(Azmuth_Motor_Neg, LOW);
}
void motors::Azmuth_Motor_Down() {
    Serial.print("Motor Down\n");
  digitalWrite(Azmuth_Motor_Pos, LOW);
  digitalWrite(Azmuth_Motor_Neg, HIGH);
}
void motors::Azmuth_Motor_Off() {
  digitalWrite(Azmuth_Motor_Pos, LOW);
  digitalWrite(Azmuth_Motor_Neg, LOW);
}
void motors::Horizon_Motor_Right() {
    Serial.print("Motor Rigth\n");
  digitalWrite(Horizon_Motor_Pos, LOW);
  digitalWrite(Horizon_Motor_Neg, LOW);
}
void motors::Horizon_Motor_Left() {
    Serial.print("Motor Left\n");
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
void motors::Return_To_Flat() {
  while (servoh != 90) {
    if (servoh < 90) {
      servoh = ++servoh;
    }
    if (servoh > 90) {
      servoh = --servoh;
    }
    horizontal.write(servoh);
  }
  while (servov != 90) {
    if (servov < 90) {
      servov = ++servov;
    }
    if (servov > 90) {
      servov = --servov;
    }
    vertical.write(servov);
  }
}
void motors::Servo_Right() {
  Serial.print("Moving Right\n");
  servoh = ++servoh;
  if (servoh > 180)
  {
    servoh = 180;
  }
  horizontal.write(servoh);
}
void motors::Servo_Left() {
  Serial.print("Moving Left\n");
  servoh = --servoh;
  if (servoh < 0)
  {
    servoh = 0;
  }
  horizontal.write(servoh);
}
void motors::Servo_Up() {
  Serial.print("Moving Up\n");
  servov = ++servov;
  if (servov > 180)
  {
    servov = 180;
  }
  vertical.write(servov);
}
void motors::Servo_Down() {
  Serial.print("Moving Down\n");
  servov = --servov;
  if (servov < 0)
  {
    servov = 0;
  }
  vertical.write(servov);
}
