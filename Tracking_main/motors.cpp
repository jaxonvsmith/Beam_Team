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

  //Servo Stuff
  servov = 90;     // stand vertical servo
  servoh = 90;     // stand horizontal servo
}
void motors::INIT() {
  horizontal.attach(52);
  vertical.attach(50);
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
  digitalWrite(Azimuth_Motor_Pos, HIGH);
  digitalWrite(Azimuth_Motor_Neg, LOW);
}
void motors::Deploy_Motor_In() {
  digitalWrite(Azimuth_Motor_Pos, LOW);
  digitalWrite(Azimuth_Motor_Neg, HIGH);
}
void motors::Deploy_Motor_Off() {
  digitalWrite(Azimuth_Motor_Pos, LOW);
  digitalWrite(Azimuth_Motor_Neg, LOW);
}
void motors::Return_To_Flat() {
  servov = 90;     // stand vertical servo
  servoh = 90;     // stand horizontal servo
  vertical.write(servov);
  delay(50);
  horizontal.write(servoh);
  Serial.print("SHOULD BE FLAT\n");
}
void motors::Servo_Right() {
  Serial.print("Moving Right: ");
  Serial.println(servoh);
  servoh = ++servoh;
  if (servoh > 180)
  {
    servoh = 180;
  }
  horizontal.write(servoh);
  delay(50);
}
void motors::Servo_Left() {
  Serial.print("Moving Left: ");
  Serial.println(servoh);
  servoh = --servoh;
  if (servoh < 0)
  {
    servoh = 0;
  }
  horizontal.write(servoh);
  delay(50);
}
void motors::Servo_Up() {
  Serial.print("Moving Up: ");
  Serial.println(servov);
  servov = --servov;
  if (servov < 0)
  {
    servov = 0;
  }
  vertical.write(servov);
  delay(50);
}
void motors::Servo_Down() {
  Serial.print("Moving Down: ");
  Serial.println(servov);
  servov = ++servov;
  if (servov > 180)
  {
    servov = 180;
  }
  vertical.write(servov);
  delay(50);
}
