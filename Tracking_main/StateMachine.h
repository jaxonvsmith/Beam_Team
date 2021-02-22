/*
  Statemachine.h - Library for the State Machine.
  Created by Jaxon Smith on 1/29/2021
*/
#ifndef StateMachine_h
#define StateMachine_h

#include "Arduino.h"

//STATES:
enum State_enum {INIT, RETRACT, DEPLOY, DEPLOY_STATUS, TRACKING_STATUS, FLAT, 
CHECK_POS, ADJ_POS, WAIT, MANUAL, UP, DOWN, CW, CCW, RETRACT_M, DEPLOY_M};

class StateMachine
{
  public:
    StateMachine();
    void SM();
  private:
    void Retract();
    void Deploy();
    void Return_Flat();
    bool OF_Status;
    bool Automatic_Status;
    uint8_t current_state;
    //...timer...//
    unsigned long time_now = 0;
    const uint32_t TRACKING_DELAY = 10000;
    //...flags...//
    //bool Prev_Deploy_Switch;
    //bool Prev_Track_Switch;
    bool Prev_OF_Switch;
    //bool Prev_Automatic_Switch;
    bool Deploy_flag;
    bool Track_flag;
    bool left_side;
    bool Print_flag;
    bool horizon_correct;
    bool vertical_correct;
    bool system_correct;
    bool closer_to_CW;
    //...switches...//
    const uint8_t OF_Switch = 45;
    const uint8_t Automatic_Switch = 43; 
    const uint8_t Up_Switch = 29;         
    const uint8_t Down_Switch = 31;       
    const uint8_t CW_Switch = 33;       
    const uint8_t CCW_Switch = 35;      
    const uint8_t Deploy_Switch = 37;
    const uint8_t Retract_Switch = 39;
    //...limitswitch's...
    uint8_t LimitSwitch_Azmuth_CW = 36;
    uint8_t LimitSwitch_Azmuth_CCW = 32;
    uint8_t LimitSwitch_Azmuth_Center = 34;
    uint8_t LimitSwitch_Elevation_Upper = 28;
    uint8_t LimitSwitch_Elevation_Lower = 30; 
    uint8_t LimitSwitch_Deploy = 24;
    uint8_t LimitSwitch_Retract = 26;
    //TAKEN FROM TRACKING_H
    //Tolerance
    int tol = 20;///Adjust as necessary
    //photo resistor pins
    const uint8_t ldrlt = A0; //LDR top left
    const uint8_t ldrrt = A1; //LDR top rigt
    const uint8_t ldrld = A2; //LDR down left
    const uint8_t ldrrd = A3 ; //ldr down right
    //photo resistor values
    int lt; // top left
    int rt; // top right
    int ld; // down left
    int rd; // down rigt
    //average resistor values
    int avt; // average value top
    int avd; // average value down
    int avl; // average value left
    int avr; // average value right
    int dvert; // check the difference of up and down
    int dhoriz;// check the difference of left and right
};

#endif
