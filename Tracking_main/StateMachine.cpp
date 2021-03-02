/*
  StateMachine.cpp - Library for managing the state machine.
  Created by Jaxon Smith on 1/29/2021
*/

/*
   Impliment tracking switch
   Servos
*/
#include "StateMachine.h"
#include "motors.h"
#include "NES_Controller.h"

motors motors_sm;
NES_Controller NES_Controller;

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
      motors_sm.Return_To_Flat();
      motors_sm.Azimuth_Motor_Off();//turn off Azmuth Motor
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
      pinMode(LimitSwitch_Azimuth_CW, INPUT);
      pinMode(LimitSwitch_Azimuth_CCW, INPUT);
      pinMode(LimitSwitch_Elevation_Upper, INPUT);
      pinMode(LimitSwitch_Elevation_Lower, INPUT);
      pinMode(LimitSwitch_Azimuth_Center, INPUT);
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
      Retract();
      Deploy_flag = false;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY:
      if (!Print_flag) {
        Serial.print("DEPLOY\n");
        Print_flag = true;
      }
      Deploy();
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
        Return_Flat();
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

        if ((-1 * tol > dvert || dvert > tol)) { // check if the diffirence is smaller than the tolerance
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
          motors_sm.Azimuth_Motor_Off();
          motors_sm.Elevation_Motor_Off();
        }
        if (!horizon_correct) {
          if (avl > avr)
          {
            motors_sm.Servo_Left();
            motors_sm.Azimuth_Motor_CW();
            if (digitalRead(LimitSwitch_Azimuth_Center)) {
              closer_to_CW = true;
            }
            if (digitalRead(LimitSwitch_Azimuth_CW)) { //if you hit the limit then rotate past center and keep tracking
              motors_sm.Azimuth_Motor_Off();//turn off motor
              delay(1000);//small delay to make changing rotation easier on the motor
              while (digitalRead(LimitSwitch_Azimuth_Center) == LOW) { //Move past center then keep tracking
                motors_sm.Azimuth_Motor_CCW();
                if (LimitSwitch_Azimuth_CCW) {
                  motors_sm.Azimuth_Motor_Off();//Went too far, something is wrong
                }
              }
              delay(5000); //delay to get farther past center
              motors_sm.Azimuth_Motor_Off();// turn off the motor
              closer_to_CW = false;
            }
            //if the array moves all the way to the CCW switch then just wait for the next check
          }
          else if (avl < avr)
          {
            motors_sm.Servo_Right();
            motors_sm.Azimuth_Motor_CCW();
            if (digitalRead(LimitSwitch_Azimuth_Center)) {
              closer_to_CW = false;
            }
            if (digitalRead(LimitSwitch_Azimuth_CCW)) {
              motors_sm.Azimuth_Motor_Off();//turn off motor
              delay(1000);//small delay to make changing rotation easier on the motor
              while (digitalRead(LimitSwitch_Azimuth_Center) == LOW) { //Move past center then keep tracking
                motors_sm.Azimuth_Motor_CW();
                if (LimitSwitch_Azimuth_CW) {
                  motors_sm.Azimuth_Motor_Off();//Went too far, something is wrong
                }
              }
              delay(5000); //delay to get farther past center
              motors_sm.Azimuth_Motor_Off();// turn off the motor
              closer_to_CW = true;
            }
            else if (avl = avr)
            {
              motors_sm.Azimuth_Motor_Off();
            }
          }
          if (!vertical_correct && horizon_correct) {
            if (avt > avd) {
              if (digitalRead(LimitSwitch_Elevation_Upper)) {
                motors_sm.Elevation_Motor_Off(); //if right is equal to left but top is greater than bottom rotate 180 and keep tracking.
              }
              motors_sm.Servo_Up();
              motors_sm.Elevation_Motor_Up();
            }
            else if (avt < avd)
            {
              if (digitalRead(LimitSwitch_Elevation_Lower)) {
                motors_sm.Elevation_Motor_Off();
                if (-1 * tol < dhoriz && dhoriz < tol) { //if within tolerance rotate 180 and keep tracking.
                  if (closer_to_CW) {
                    motors_sm.Azimuth_Motor_CCW();
                    delay(20000); //NEED TO FIGURE OUT THIS VALUE>> TIME TO ROTATE 180 DEGREES
                    motors_sm.Azimuth_Motor_Off();
                  }
                  else {
                    motors_sm.Azimuth_Motor_CW();
                    delay(20000); //NEED TO FIGURE OUT THIS VALUE>> TIME TO ROTATE 180 DEGREES
                    motors_sm.Azimuth_Motor_Off();
                  }
                }
              }
              motors_sm.Servo_Down();
              motors_sm.Elevation_Motor_Down();
            }
            else if (avt == avd) {
              motors_sm.Elevation_Motor_Off();
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
        nesRegister = NES_Controller.readNesController();
        if (digitalRead(Up_Switch) || bitRead(nesRegister, UP_BUTTON) == 0) {
          current_state = UP;
          Print_flag = false;
        }
        if (digitalRead(Down_Switch) || bitRead(nesRegister, DOWN_BUTTON) == 0) {
          current_state = DOWN;
          Print_flag = false;
        }
        if (digitalRead(CW_Switch) || bitRead(nesRegister, LEFT_BUTTON) == 0) {
          current_state = CW;
          Print_flag = false;
        }
        if (digitalRead(CCW_Switch) || bitRead(nesRegister, RIGHT_BUTTON) == 0) {
          current_state = CCW;
          Print_flag = false;
        }
        if (digitalRead(Retract_Switch) || bitRead(nesRegister, B_BUTTON) == 0) {
          current_state = RETRACT_M;
          Print_flag = false;
        }
        if (digitalRead(Deploy_Switch) || bitRead(nesRegister, A_BUTTON) == 0) {
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
        nesRegister = NES_Controller.readNesController();
        while (digitalRead(Up_Switch) || bitRead(nesRegister, UP_BUTTON) == 0) {
          nesRegister = NES_Controller.readNesController();
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
        nesRegister = NES_Controller.readNesController();
        while (digitalRead(Down_Switch) || bitRead(nesRegister, DOWN_BUTTON) == 0) {
          nesRegister = NES_Controller.readNesController();
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
        nesRegister = NES_Controller.readNesController();
        while (digitalRead(CW_Switch) || bitRead(nesRegister, LEFT_BUTTON) == 0) {
          nesRegister = NES_Controller.readNesController();
          if (!CW_motor_flag) {
            motors_sm.Azimuth_Motor_CW();
          }
          CW_motor_flag = true;
          if (digitalRead(LimitSwitch_Azimuth_CW)) {
            motors_sm.Azimuth_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
        }
        motors_sm.Azimuth_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
        break;
      case CCW:
        bool CCW_motor_flag = false;
        if (!Print_flag) {
          Serial.print("CCW\n");
          Print_flag = true;
        }
        nesRegister = NES_Controller.readNesController();
        while (digitalRead(CCW_Switch) || bitRead(nesRegister, RIGHT_BUTTON) == 0) {
          nesRegister = NES_Controller.readNesController();
          if (!CCW_motor_flag) {
            motors_sm.Azimuth_Motor_CCW();
          }
          CCW_motor_flag = true;
          if (digitalRead(LimitSwitch_Azimuth_CCW)) {
            motors_sm.Azimuth_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
        }
        motors_sm.Azimuth_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
        break;
      case RETRACT_M:
        bool Retract_motor_flag = false;
        if (!Print_flag) {
          Serial.print("RETEACT_M\n");
          Print_flag = true;
        }
        nesRegister = NES_Controller.readNesController();
        while (digitalRead(Retract_Switch) || bitRead(nesRegister, B_BUTTON) == 0) {
          nesRegister = NES_Controller.readNesController();
          if (!Retract_motor_flag) {
            motors_sm.Deploy_Motor_In();
          }
          Retract_motor_flag = true;
          if (digitalRead(LimitSwitch_Retract)) {
            motors_sm.Deploy_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
        }
        motors_sm.Deploy_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
        break;
      case DEPLOY_M:
        bool Deploy_motor_flag = false;
        if (!Print_flag) {
          Serial.print("DEPLOY_M\n");
          Print_flag = true;
        }
        nesRegister = NES_Controller.readNesController();
        while (digitalRead(Deploy_Switch) || bitRead(nesRegister, B_BUTTON) == 0) {
          nesRegister = NES_Controller.readNesController();
          if (!Deploy_motor_flag) {
            motors_sm.Deploy_Motor_Out();
          }
          Deploy_motor_flag = true;
          if (digitalRead(LimitSwitch_Deploy)) {
            motors_sm.Deploy_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
        }
        motors_sm.Deploy_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
        break;
      default:
        break;
      }
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
  while (digitalRead(LimitSwitch_Azimuth_Center) == false) {
    if (closer_to_CW) {
      motors_sm.Azimuth_Motor_CCW();
      moving_CW = false;
    }
    else {
      motors_sm.Azimuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azimuth_CCW) && !moving_CW) {
      //you were going the wrong direction;
      motors_sm.Azimuth_Motor_Off();
      delay(1000); //turn off to improve transition
      motors_sm.Azimuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azimuth_CW) && moving_CW) {
      //you were going the wrong direction;
      motors_sm.Azimuth_Motor_Off();
      delay(1000); //turn off to improve transition
      motors_sm.Azimuth_Motor_CW();
      moving_CW = false;
    }
    //ADD AN ERROR CHECK IN CASE IT CANT FIND THE CENTER.
  }
  motors_sm.Azimuth_Motor_Off();
  while (digitalRead(LimitSwitch_Elevation_Lower) == false) {
    motors_sm.Elevation_Motor_Down();
  }
  motors_sm.Elevation_Motor_Off();
}
