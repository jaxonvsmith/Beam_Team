/*
  motors.h - Library for the State Machine.
  Created by Jaxon Smith on 1/29/2021
*/
#ifndef motors_h
#define motors_h

#include "Arduino.h"
#include "Servo.h"


class motors
{
  public:
    motors();
    void Azimuth_Motor_CW();
    void Azimuth_Motor_CCW();
    void Azimuth_Motor_Off();
    void Elevation_Motor_Up();
    void Elevation_Motor_Down();
    void Elevation_Motor_Off();
    void Deploy_Motor_Out();
    void Deploy_Motor_In();
    void Deploy_Motor_Off();
    void Return_To_Flat();
    void Servo_Right();
    void Servo_Left();
    void Servo_Up();
    void Servo_Down();
    void INIT();
    void Retract();
    void Retract_Off();
    void Deploy();
    void Deploy_Off();
  private:
    //...motors...
    const uint8_t Azimuth_Motor_Pos = 44;//38;
    const uint8_t Azimuth_Motor_Neg = 40;
    const uint8_t Azimuth_Motor_PWM = 11;
    const uint8_t Elevation_Motor_Pos = 38;//44; 
    const uint8_t Elevation_Motor_Neg = 42;
    const uint8_t Elevation_Motor_PWM = 12;
    const uint8_t Deploy_Motor_Pos = 46;
    const uint8_t Deploy_Motor_Neg = 48;
    const uint8_t Deploy_Motor_PWM = 13;
    const uint8_t MOTOR_SPEED = 150; //Pick a number 0-255
    Servo horizontal; // horizontal servo
    Servo vertical;   // vertical servo
    int servoh;
    int servov;
};

#endif
