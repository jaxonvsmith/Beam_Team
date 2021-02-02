/*
  StateMachine.cpp - Library for managing the state machine.
  Created by Jaxon Smith on 1/29/2021
*/

#include "StateMachine.h"
#include "motors.h"

motors motors;

StateMachine::StateMachine() {
  current_state = INIT;
}

void StateMachine::SM() {
  switch (current_state) {
    case INIT:
      motors.Azmuth_Motor_Off();//turn off Azmuth Motor
      motors.Horizon_Motor_Off();//turn off Horizon Motor
      motors.Deploy_Motor_Off();//turn off Deploy Motor
      Deploy_flag = false;
      Track_flag = false;
      s_state = false;
      left_side = false;
      Prev_Deploy_Switch = false;
      Prev_Track_Switch = false;
      current_state = DEPLOY_STATUS;
      break;
    case RETRACT:
      motors.Deploy_Motor_In();
      while (digitalRead(LimitSwitch_Retract) == LOW) {

      }
      motors.Deploy_Motor_Off();
      Deploy_flag = false;
      current_state = DEPLOY_STATUS;
      break;
    case DEPLOY:
      motors.Deploy_Motor_Out();
      while (digitalRead(LimitSwitch_Deploy) == LOW) {
        //wait for system to deploy out
      }
      motors.Deploy_Motor_Off();
      Deploy_flag = true;
      current_state = DEPLOY_STATUS;
      break;
    case DEPLOY_STATUS:
      s_state = digitalRead(Deploy_Switch);
      Prev_Deploy_Switch = s_state;
      if (s_state && !Deploy_flag) {
        current_state = DEPLOY;
      }
      else if(!s_state && Deploy_flag) {
          current_state = RETRACT;
        }
      else {
        current_state = TRACKING_STATUS;
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
      else {
        current_state = WAIT;
      }
      break;
    case FLAT:
      while (digitalRead(LimitSwitch_Center) == false) {
        if (left_side) {
          motors.Horizon_Motor_Right();//move motor right
          if (digitalRead(LimitSwitch_Horizontal)) {
            motors.Horizon_Motor_Left();//You are moving the wrong way, move motor left
          }
        }
        else {
          motors.Horizon_Motor_Left();//move motor left
          if (digitalRead(LimitSwitch_Horizontal)) {
            motors.Horizon_Motor_Right();//You are moving the wrong way, move motor right
          }
        }
      }
      motors.Horizon_Motor_Off();
      Track_flag = false;
      current_state = WAIT;
      break;
    case CHECK_POS:
      //      if(tracking.pos_correct()){...........................NOT IMPLIMENTED.......................
      //        current_state = ADJ_POS;
      //      }
      //      else{
      //        current_state = WAIT;
      //      }
      break;
    case ADJ_POS:
      //      tracking.adj_pos();...................................NOT IMPLIMENTED.......................
      current_state = WAIT;
      break;
    case WAIT:
      if (millis() >= time_now + TRACKING_DELAY) {
        time_now += TRACKING_DELAY;
        current_state = TRACKING_STATUS;
      }
      if (digitalRead(Track_Switch) != Prev_Track_Switch) {
        current_state = TRACKING_STATUS;
      }
      if (digitalRead(Deploy_Switch) != Prev_Deploy_Switch) {
        current_state = DEPLOY_STATUS;
      }
      break;
    default:
      break;
  }
}
