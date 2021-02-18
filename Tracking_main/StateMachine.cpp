/*
  StateMachine.cpp - Library for managing the state machine.
  Created by Jaxon Smith on 1/29/2021
*/

/*
   Impliment tracking switch
   Servos
*/
#include "StateMachine.h"
#include "tracking.h"
#include "motors.h"

tracking tracking;
motors motors_sm;

StateMachine::StateMachine() {
  current_state = INIT;
}

void StateMachine::SM() {
  switch (current_state) {
    case INIT:
      if (!Print_flag) {
        Serial.print("INIT\n");
        Print_flag = true;
      }
      motors_sm.INIT();
      tracking.INIT();
      motors_sm.Return_To_Flat();
      motors_sm.Azmuth_Motor_Off();//turn off Azmuth Motor
      motors_sm.Elevation_Motor_Off();//turn off Horizon Motor
      motors_sm.Deploy_Motor_Off();//turn off Deploy Motor
      Deploy_flag = false;
      Track_flag = false;
      OF_Status = false;
      Automatic_Status;
      left_side = false;
      //Prev_Deploy_Switch = false;
      //Prev_Track_Switch = false;
      horizon_correct = false;
      vertical_correct = false;
      system_correct = false;
      //pins
      //pinMode(Deploy_Switch, INPUT);
      //pinMode(Track_Switch, INPUT);
      pinMode(LimitSwitch_Azmuth_CW, INPUT);
      pinMode(LimitSwitch_Azmuth_CCW, INPUT);
      pinMode(LimitSwitch_Elevation_Upper, INPUT);
      pinMode(LimitSwitch_Elevation_Lower, INPUT);
      pinMode(LimitSwitch_Azmuth_Center, INPUT);
      pinMode(LimitSwitch_Deploy, INPUT);
      pinMode(LimitSwitch_Retract, INPUT);
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case RETRACT:
      if (!Print_flag) {
        Serial.print("RETRACT\n");
        Print_flag = true;
      }
      //Retract();.................................................UNCOMMENT FOR DC MOTORS........................................
      Deploy_flag = false;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY:
      if (!Print_flag) {
        Serial.print("DEPLOY\n");
        Print_flag = true;
      }
      //Deploy();.................................................UNCOMMENT FOR DC MOTORS........................................
      Deploy_flag = true;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY_STATUS:
      OF_Status = digitalRead(OF_Switch);
      Automatic_Status = digitalRead(Automatic_Switch);
      if (!Print_flag) {
        Serial.print("DEPLOY STATUS\n");
        Print_flag = true;
      }
      Prev_OF_Switch = OF_Status;
      if (!OF_Status && Deploy_flag) {
        current_state = RETRACT;
        Print_flag = false;
        break;
      }
      if ((OF_Status && !Deploy_flag) && Automatic_Status) {
        current_state = DEPLOY;
        Print_flag = false;
        break;
      }
      current_state = TRACKING_STATUS;
      Print_flag = false;
      break;
    case TRACKING_STATUS:
      OF_Status = digitalRead(OF_Switch);
      Automatic_Status = digitalRead(Automatic_Switch);
      if (!Print_flag) {
        Serial.print("TRACKING STATUS\n");
        Print_flag = true;
      }
      if (OF_Status && Automatic_Status) {
        Track_flag = true;
        current_state = CHECK_POS;
        Print_flag = false;
      }
      else if (!OF_Status && Automatic_Status && Track_flag == true) {
        //Return_Flat();.......................................................NOT IMPLIMENTED YET......................................
        current_state = FLAT;
        Print_flag = false;
      }
      else {
        current_state = WAIT;
        Print_flag = false;
      }
      break;
    case FLAT:
      if (!Print_flag) {
        Serial.print("FLAT\n");
        Print_flag = true;
      }
      motors_sm.Return_To_Flat();
      Track_flag = false;
      current_state = WAIT;
      Print_flag = false;
      break;
    case CHECK_POS:
      if (!Print_flag) {
        Serial.print("CHECK_POS\n");
        Print_flag = true;
      }

      lt = analogRead(ldrlt); // top left
      rt = analogRead(ldrrt); // top right
      ld = analogRead(ldrld); // down left
      rd = analogRead(ldrrd); // down rigt

      avt = (lt + rt) / 2; // average value top
      avd = (ld + rd) / 2; // average value down
      avl = (lt + ld) / 2; // average value left
      avr = (rt + rd) / 2; // average value right

      dvert = avt - avd; // check the diffirence of up and down
      dhoriz = avl - avr;// check the diffirence og left and right

      if ((-1 * tol > dvert || dvert > tol)) { // check if the diffirence is in the tolerance
        current_state = ADJ_POS;
        vertical_correct = false;
        system_correct = false;
        Print_flag = false;
      }

      else if (-1 * tol > dhoriz || dhoriz > tol) { // check if the diffirence is in the tolerance
        current_state = ADJ_POS;
        horizon_correct = false;
        system_correct = false;
        Print_flag = false;
      }
      else {
        current_state = WAIT;
        Print_flag = false;
      }
      break;
    case ADJ_POS:
      if (!Print_flag) {
        Serial.print("ADJ_POS\n");
        Print_flag = true;
      }
      while (!system_correct) {
        lt = analogRead(ldrlt); // top left
        rt = analogRead(ldrrt); // top right
        ld = analogRead(ldrld); // down left
        rd = analogRead(ldrrd); // down rigt

        avt = (lt + rt) / 2; // average value top
        avd = (ld + rd) / 2; // average value down
        avl = (lt + ld) / 2; // average value left
        avr = (rt + rd) / 2; // average value right

        dvert = avt - avd; // check the diffirence of up and down
        dhoriz = avl - avr;// check the diffirence og left and right

        if ((-1 * tol > dvert || dvert > tol)) { // check if the diffirence is in the tolerance
          vertical_correct = false;
          system_correct = false;
        }
        else {
          vertical_correct = true;
        }

        if (-1 * tol > dhoriz || dhoriz > tol) { // check if the diffirence is in the tolerance
          horizon_correct = false;
          system_correct = false;
        }
        else {
          horizon_correct = true;
        }
        if (horizon_correct && vertical_correct) {
          system_correct = true;
        }
        if (!horizon_correct) {
          if (avl > avr)
          {
            motors_sm.Servo_Left();
          }
          else if (avl < avr)
          {
            motors_sm.Servo_Right();
          }
          else if (avl = avr)
          {
            // nothing
          }
        }
        if (!vertical_correct && horizon_correct) {
          if (avt > avd)
          {
            motors_sm.Servo_Up();
          }
          else if (avt < avd)
          {
            motors_sm.Servo_Down();
          }
          else if (avt == avd) {
            //nothing
          }
        }
      }
      current_state = WAIT;
      Print_flag = false;
      break;
    case WAIT:
      OF_Status = digitalRead(OF_Switch);
      Automatic_Status = digitalRead(Automatic_Switch);
      if (!Print_flag) {
        Serial.print("WAIT\n");
        Print_flag = true;
      }
      if (millis() >= time_now + TRACKING_DELAY) {
        time_now += TRACKING_DELAY;
        current_state = TRACKING_STATUS;
        Print_flag = false;
      }
      if (OF_Status != Prev_OF_Switch) {
        current_state = DEPLOY_STATUS;
        Print_flag = false;
      }
      if (!Automatic_Status) {
        current_state = MANUAL;
        Print_flag = false;
        Deploy_flag = false;
      }
      break;
    case MANUAL:
      if (!Print_flag) {
        Serial.print("MANUAL\n");
        Print_flag = true;
      }
      if (digitalRead(Automatic_Switch)) {
        current_state = DEPLOY_STATUS;
        Print_flag = false;
      }
      if (digitalRead(Up_Switch)) {
        current_state = UP;
        Print_flag = false;
      }
      if (digitalRead(Down_Switch)) {
        current_state = DOWN;
        Print_flag = false;
      }
      if (digitalRead(CW_Switch)) {
        current_state = CW;
        Print_flag = false;
      }
      if (digitalRead(CCW_Switch)) {
        current_state = CCW;
        Print_flag = false;
      }
      if (digitalRead(Retract_Switch)) {
        current_state = RETRACT_M;
        Print_flag = false;
      }
      if (digitalRead(Deploy_Switch)) {
        current_state = DEPLOY_M;
        Print_flag = false;
      }
      break;
    case UP:
      bool up_motor_flag = false;
      if (!Print_flag) {
        Serial.print("UP\n");
        Print_flag = true;
      }
      while (digitalRead(Up_Switch)) {
        if (!up_motor_flag) {
          motors_sm.Elevation_Motor_Up();
        }
        up_motor_flag = true;
        if (digitalRead(LimitSwitch_Elevation_Upper)) {
          motors_sm.Elevation_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
      }
      motors_sm.Elevation_Motor_Off();
      current_state = MANUAL;
      Print_flag = false;
      break;
    case DOWN:
      bool down_motor_flag = false;
      if (!Print_flag) {
        Serial.print("DOWN\n");
        Print_flag = true;
      }
      while (digitalRead(Down_Switch)) {
        if (!down_motor_flag) {
          motors_sm.Elevation_Motor_Down();
        }
        down_motor_flag = true;
        if (digitalRead(LimitSwitch_Elevation_Lower)) {
          motors_sm.Elevation_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
      }
      motors_sm.Elevation_Motor_Off();
      current_state = MANUAL;
      Print_flag = false;
      break;
    case CW:
      bool CW_motor_flag = false;
      if (!Print_flag) {
        Serial.print("CW\n");
        Print_flag = true;
      }
      while (digitalRead(CW_Switch)) {
        if (!CW_motor_flag) {
          motors_sm.Azmuth_Motor_CW();
        }
        CW_motor_flag = true;
        if (digitalRead(LimitSwitch_Azmuth_CW)) {
          motors_sm.Azmuth_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
      }
      motors_sm.Azmuth_Motor_Off();
      current_state = MANUAL;
      Print_flag = false;
      break;
    case CCW:
      bool CCW_motor_flag = false;
      if (!Print_flag) {
        Serial.print("CCW\n");
        Print_flag = true;
      }
      while (digitalRead(CCW_Switch)) {
        if (!CCW_motor_flag) {
          motors_sm.Azmuth_Motor_CCW();
        }
        CCW_motor_flag = true;
        if (digitalRead(LimitSwitch_Azmuth_CCW)) {
          motors_sm.Azmuth_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
      }
      motors_sm.Azmuth_Motor_Off();
      current_state = MANUAL;
      Print_flag = false;
      break;
    case RETRACT_M:
      if (!Print_flag) {
        Serial.print("RETRACT_M\n");
        Print_flag = true;
      }
      motors_sm.Deploy_Motor_Off();
      break;
    case DEPLOY_M:
      if (!Print_flag) {
        Serial.print("DEPLOY_M\n");
        Print_flag = true;
      }
      motors_sm.Deploy_Motor_Off();
      break;
    default:
      break;
  }
}

void StateMachine::Retract() {
  while (digitalRead(!LimitSwitch_Retract)) {
    motors_sm.Deploy_Motor_In();
  }
  motors_sm.Deploy_Motor_Off();
}

void StateMachine::Deploy() {
  while (digitalRead(!LimitSwitch_Deploy)) {
    motors_sm.Deploy_Motor_Out();
  }
  motors_sm.Deploy_Motor_Off();
}
void StateMachine::Return_Flat() {
  bool moving_CW;
  while (digitalRead(LimitSwitch_Azmuth_Center) == false) {
    if (closer_to_CW) {
      motors_sm.Azmuth_Motor_CCW();
      moving_CW = false;
    }
    else {
      motors_sm.Azmuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azmuth_CCW) && !moving_CW) {
      //you were going the wrong direction;
      motors_sm.Azmuth_Motor_Off();
      delay(1000); //turn off to improve transition
      motors_sm.Azmuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azmuth_CW) && moving_CW) {
      //you were going the wrong direction;
      motors_sm.Azmuth_Motor_Off();
      delay(1000); //turn off to improve transition
      motors_sm.Azmuth_Motor_CW();
      moving_CW = false;
    }
  }
  motors_sm.Azmuth_Motor_Off();
  while (digitalRead(LimitSwitch_Elevation_Lower) == false) {
    motors_sm.Elevation_Motor_Down();
  }
  motors_sm.Elevation_Motor_Off();
}
