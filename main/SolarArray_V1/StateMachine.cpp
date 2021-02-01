/*
  StateMachine.cpp - Library for managing the state machine.
  Created by Jaxon Smith on 1/29/2021
*/

#include "StateMachine.h"


StateMachine::StateMachine() {
  current_state = INIT;
  timer = 0;
}

void StateMachine::SM() {
  switch (current_state) {
    case INIT:
      //turn off Azmuth Motor
      //turn off Horizon Motor
      //turn off Deploy Motor
      Deploy_flag = false;
      Track_flag = false;
      s_state = false;
      timer = 0;
      left_side = false;
      Prev_Deploy_Switch = false;
      Prev_Track_Switch = false;
      current_state = DEPLOY_STATUS;
      break;
    case RETRACT:
      break;
    case DEPLOY:
      break;
    case DEPLOY_STATUS:
      s_state = digitalRead(Deploy_Switch);
      Prev_Deploy_Switch = s_state;
      if (s_state && !Deploy_flag) {
        current_state = DEPLOY;
      }
      if else(!s_state && Deploy_flag) {
          current_state = RETRACT;
        }
      else {
        current_state = TRACKING_STATUS
      }
      break;
    case TRACKING_STATUS:
      s_state = digitalRead(Track_Switch);
      Prev_Track_Switch = s_state;
      if (s_state) {
        Track_flag = true;
        current_state = CHECK_POS;
      }
      else if (!s_state && Track_flag) {
        current_state = FLAT;
      }
      else{
        current_state = WAIT;
      }
      break;
    case FLAT:
      while(!LimitSwitch_Center){
        if(left_side){
          //move motor right.....................................NOT IMPLIMENTED....................
          if(LimitSwitch_Horizontal){
            //You are moving the wrong way, move motor left......NOT IMPLIMENTED....................
          }
        }
        else{
          //move motor left......................................NOT IMPLIMENTED....................
          if(LimitSwitch_Horizontal){
            //You are moving the wrong way, move motor right......NOT IMPLIMENTED....................
          }
        }
      }
      Track_flag = false;
      current_state = WAIT;
      break;
    case CHECK_POS:
      break;
    case ADJ_POS:
      break;
    case WAIT:
      break;
    default:
      break;
  }
}
