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
    const uint32_t TRACKING_DELAY = 5000;
    //...flags...//
    bool Prev_Deploy_Switch;
    bool Prev_Track_Switch;
    bool Deploy_flag;
    bool Track_flag;
    bool left_side;
    //...switches...//
    const uint8_t Deploy_Switch = 27;
    const uint8_t Track_Switch = 29;
    //...limitswitch's...
    uint8_t LimitSwitch_Azmuth_Upper = 31;
    uint8_t LimitSwitch_Azmuth_Lower = 45;
    uint8_t LimitSwitch_Horizontal = 47;
    uint8_t LimitSwitch_Center = 49;
    uint8_t LimitSwitch_Deploy = 51;
    uint8_t LimitSwitch_Retract = 53;
};

#endif
