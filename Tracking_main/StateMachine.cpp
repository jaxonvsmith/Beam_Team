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
      motors_sm.Horizon_Motor_Off();//turn off Horizon Motor
      motors_sm.Deploy_Motor_Off();//turn off Deploy Motor
      Deploy_flag = false;
      Track_flag = false;
      s_state = false;
      left_side = false;
      Prev_Deploy_Switch = false;
      Prev_Track_Switch = false;
      //pins
      pinMode(Deploy_Switch, INPUT);
      pinMode(Track_Switch, INPUT);
      pinMode(LimitSwitch_Azmuth_Upper, INPUT);
      pinMode(LimitSwitch_Azmuth_Lower, INPUT);
      pinMode(LimitSwitch_Horizontal, INPUT);
      pinMode(LimitSwitch_Center, INPUT);
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
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY:
      if (!Print_flag) {
        Serial.print("DEPLOY\n");
        Print_flag = true;
      }
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY_STATUS:
      if (!Print_flag) {
        Serial.print("DEPLOY STATUS\n");
        Print_flag = true;
      }
      current_state = TRACKING_STATUS;
      Print_flag = false;
      break;
    case TRACKING_STATUS:
      if (!Print_flag) {
        Serial.print("TRACKING STATUS\n");
        Print_flag = true;
      }
      s_state = digitalRead(Track_Switch);
      Serial.print("Track_Switch: ");
      Serial.println(s_state);
      Prev_Track_Switch = s_state;
      if (s_state) {
        Track_flag = true;
        current_state = CHECK_POS;
        Print_flag = false;
      }
      else if (!s_state && Track_flag) {
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
//      if (!tracking.pos_correct()) {
//        current_state = ADJ_POS;
//        Print_flag = false;
//      }
//      else {
//        current_state = WAIT;
//        Print_flag = false;
//      }

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
       Print_flag = false;
      }

      if (-1 * tol > dhoriz || dhoriz > tol) { // check if the diffirence is in the tolerance
        current_state = WAIT;
        Print_flag = false;
      }
      break;
    case ADJ_POS:
      if (!Print_flag) {
        Serial.print("ADJ_POS\n");
        Print_flag = true;
      }
      //tracking.adj_pos();
      ///REPLACE THE ADJ POS FUNCTION
      while (-1 * tol > dhoriz || dhoriz > tol) { // check if the diffirence is in the tolerance
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
      Serial.print("dhoriz: ");
      Serial.println(dvert);
      Serial.print("avt: ");
      Serial.println(avt);
      Serial.print("avd: ");
      Serial.println(avd);
      while ((-1 * tol > dvert || dvert > tol)) { // check if the diffirence is in the tolerance
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
      current_state = WAIT;
      Print_flag = false;
      break;
    case WAIT:
      if (!Print_flag) {
        Serial.print("WAIT\n");
        Print_flag = true;
      }
      if (millis() >= time_now + TRACKING_DELAY) {
        time_now += TRACKING_DELAY;
        current_state = TRACKING_STATUS;
        Print_flag = false;
      }
      if (digitalRead(Track_Switch) != Prev_Track_Switch) {
        current_state = TRACKING_STATUS;
        Print_flag = false;
      }
      if (digitalRead(Deploy_Switch) != Prev_Deploy_Switch) {
        current_state = DEPLOY_STATUS;
        Print_flag = false;
      }
      break;
    default:
      break;
  }
}
