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
    uint32_t timer;
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
};

#endif
