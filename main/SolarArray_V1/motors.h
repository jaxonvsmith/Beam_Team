/*
  motors.h - Library for the State Machine.
  Created by Jaxon Smith on 1/29/2021
*/
#ifndef motors_h
#define motors_h

#include "Arduino.h"

class motors
{
  public:
    motors();
    void Azmuth_Motor_Up();
    void Azmuth_Motor_Down();
    void Azmuth_Motor_Off();
    void Horizon_Motor_Right();
    void Horizon_Motor_Left();
    void Horizon_Motor_Off();
    void Deploy_Motor_Out();
    void Deploy_Motor_In();
    void Deploy_Motor_Off();
  private:
    //...motors...
    const uint8_t Azmuth_Motor_Pos = 33;
    const uint8_t Azmuth_Motor_Neg = 35;
    const uint8_t Azmuth_Motor_PWM = 13;
    const uint8_t Horizon_Motor_Pos = 37;
    const uint8_t Horizon_Motor_Neg = 39;
    const uint8_t Horizon_Motor_PWM =12;
    const uint8_t Deploy_Motor_Pos =41;
    const uint8_t Deploy_Motor_Neg =43;
    const uint8_t Deploy_Motor_PWM =11;
    const uint8_t MOTOR_SPEED = 150; //Pick a number 0-255
};

#endif
