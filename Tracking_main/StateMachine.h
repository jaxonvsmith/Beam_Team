/*
  Statemachine.h - Library for the State Machine.
  Created by Jaxon Smith on 1/29/2021
*/
#ifndef StateMachine_h
#define StateMachine_h

#include "Arduino.h"

//STATES:
enum State_enum {INIT, RETRACT, DEPLOY, DEPLOY_STATUS, TRACKING_STATUS, FLAT, CHECK_POS, ADJ_POS, WAIT};

class StateMachine
{
  public:
    StateMachine();
    void SM();
  private:
    bool s_state;
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
    const uint8_t OF_Switch = 29;
    const uint8_t Automatic_Switch = 27; //................................................................REFER TO AARONS PINOUT........................................................
    const uint8_t Up_Switch = 1;         //................................................................REFER TO AARONS PINOUT........................................................
    const uint8_t Down_Switch = 1;       //................................................................REFER TO AARONS PINOUT........................................................
    const uint8_t Left_Switch = 1;       //................................................................REFER TO AARONS PINOUT........................................................
    const uint8_t Right_Switch = 1;      //................................................................REFER TO AARONS PINOUT........................................................
    //const uint8_t Deploy_Switch = 27;
    //const uint8_t Track_Switch = 29;
    //...limitswitch's...
    uint8_t LimitSwitch_Azmuth_CW = 31;
    uint8_t LimitSwitch_Azmuth_CCW = 45;
    uint8_t LimitSwitch_Azmuth_Center = 49;
    uint8_t LimitSwitch_Elevation_Upper = 47;
    uint8_t LimitSwitch_Elevation_Lower = 1; //............................................................REFER TO AARONS PINOUT........................................................
    uint8_t LimitSwitch_Deploy = 51;
    uint8_t LimitSwitch_Retract = 53;
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
