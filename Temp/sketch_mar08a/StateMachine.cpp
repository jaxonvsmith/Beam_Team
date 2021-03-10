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
      pinMode(OF_Switch, INPUT);
      pinMode(Automatic_Switch, INPUT);
      pinMode(Up_Switch, INPUT);
      pinMode(Down_Switch, INPUT);
      pinMode(CW_Switch, INPUT);
      pinMode(CCW_Switch, INPUT);
      pinMode(Deploy_Switch, INPUT);
      pinMode(Retract_Switch, INPUT);
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
      //Debounce all of the switches
      int Up_State = LOW;
      int Down_State = LOW;
      int CW_State = LOW;
      int CCW_State = LOW;
      int Deploy_State = LOW;
      int Retract_State = LOW;
      if (digitalRead(Up_Switch)) {
        delay(20);
        Up_State = digitalRead(Up_Switch);
      }
      if (digitalRead(Down_Switch)) {
        delay(20);
        Down_State = digitalRead(Down_Switch);
      }
      if (digitalRead(CW_Switch)) {
        delay(20);
        CW_State = digitalRead(CW_Switch);
      }
      if (digitalRead(CCW_Switch)) {
        delay(20);
        CCW_State = digitalRead(CCW_Switch);
      }
      if (digitalRead(Deploy_Switch)) {
        delay(20);
        Deploy_State = digitalRead(Deploy_Switch);
      }
      if (digitalRead(Retract_Switch)) {
        delay(20);
        Retract_State = digitalRead(Retract_Switch);
      }
      //nesRegister = NES_Controller.readNesController();
      //        Serial.print("UP: ");
      //        Serial.println(Up_State);
      //        Serial.print("DOWN: ");
      //        Serial.println(Down_State);
      //        Serial.print("CW: ");
      //        Serial.println(CW_State);
      //        Serial.print("CCW: ");
      //        Serial.println(CCW_State);
      //        Serial.print("DEPLOY: ");
      //        Serial.println(Deploy_State);
      //        Serial.print("RETRACT: ");
      //        Serial.println(Retract_State);
      if (!Print_flag) {
        Serial.print("MANUAL\n");
        Print_flag = true;
      }
      if (digitalRead(Automatic_Switch)) {
        Serial.println("Switching to Auto mode");
        current_state = DEPLOY_STATUS;
        Print_flag = false;
      }
      else if ((Up_State == HIGH)) {
        Serial.println("Made it into the Up");
        bool up_motor_flag = false;
        while (digitalRead(Up_Switch)) {
          if (digitalRead(LimitSwitch_Elevation_Upper)) {
            Serial.println("Upper Limit Switch Triggered");
            motors_sm.Elevation_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
          if (!up_motor_flag) {
            Serial.println("Up Motor On");
            motors_sm.Elevation_Motor_Up();
          }
          up_motor_flag = true;
        }
        Serial.println("Up Motor Off");
        motors_sm.Elevation_Motor_Off();
        current_state = MANUAL;
      }
      else if ((Down_State == HIGH)) {
        Serial.println("Made it into the Down");
        bool down_motor_flag = false;
        while (digitalRead(Down_Switch)) {
          if (digitalRead(LimitSwitch_Elevation_Lower)) {
            Serial.println("Lower Limit Switch Triggered");
            motors_sm.Elevation_Motor_Off();
            current_state = MANUAL;
            break;
          }
          if (!down_motor_flag) {
            Serial.println("Down Motor On");
            motors_sm.Elevation_Motor_Down();
          }
          down_motor_flag = true;
        }
        Serial.println("Down Motor Off");
        motors_sm.Elevation_Motor_Off();
        current_state = MANUAL;
      }
      else if ((CW_State == HIGH)) {
        delay(20); //debounce time
        Serial.println("Made it into the CW");
        bool CW_motor_flag = false;
        if (!Print_flag) {
          Serial.print("CW\n");
          Print_flag = true;
        }
        while (digitalRead(CW_Switch)) {
          delay(20);  //debounce
          if (digitalRead(LimitSwitch_Azimuth_CW)) {
            Serial.println("CW Limit Switch Triggered");
            motors_sm.Azimuth_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
          if (!CW_motor_flag) {
            Serial.println("Azimuth Motor CW");
            motors_sm.Azimuth_Motor_CW();
          }
          CW_motor_flag = true;
        }
        Serial.println("Azimuth Motor Off");
        motors_sm.Azimuth_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
      }
      else if ((CCW_State == HIGH)) {
        Serial.println("Made it into the CCW");
        bool CCW_motor_flag = false;
        if (!Print_flag) {
          Serial.print("CCW\n");
          Print_flag = true;
        }
        while (digitalRead(CCW_Switch)) {
          if (digitalRead(LimitSwitch_Azimuth_CCW)) {
            Serial.println("CCW Limit Switch Triggered");
            motors_sm.Azimuth_Motor_Off();
            current_state = MANUAL;
            break;
          }
          if (!CCW_motor_flag) {
            Serial.println("Azimuth Motor CCW");
            motors_sm.Azimuth_Motor_CCW();
          }
          CCW_motor_flag = true;
        }
        Serial.println("Azimuth Motor Off");
        motors_sm.Azimuth_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
      }
      else if ((Retract_State == HIGH)) {
        Serial.println("Made it into the Retract");
        bool Retract_motor_flag = false;
        if (!Print_flag) {
          Serial.print("RETEACT_M\n");
          Print_flag = true;
        }
        while (digitalRead(Retract_Switch)) {
          if (digitalRead(LimitSwitch_Retract)) {
            Serial.println("Retract Limit Swich Triggered");
            motors_sm.Deploy_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
          if (!Retract_motor_flag) {
            Serial.println("Retract Motor On");
            motors_sm.Deploy_Motor_In();
          }
          Retract_motor_flag = true;
        }
        Serial.println("Retract Motor Off");
        motors_sm.Deploy_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
      }
      else if ((Deploy_State == HIGH)) {
        Serial.println("Made it into the Deploy");
        bool Deploy_motor_flag = false;
        if (!Print_flag) {
          Serial.print("Deploy_M\n");
          Print_flag = true;
        }
        while (digitalRead(Deploy_Switch)) {
          if (digitalRead(LimitSwitch_Deploy)) {
            Serial.println("Deploy Limit Swich Triggered");
            motors_sm.Deploy_Motor_Off();
            current_state = MANUAL;
            Print_flag = false;
            break;
          }
          if (!Deploy_motor_flag) {
            Serial.println("Deploy Motor On");
            motors_sm.Deploy_Motor_Out();
          }
          Deploy_motor_flag = true;
        }
        Serial.println("Deploy Motor Off");
        motors_sm.Deploy_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
      }
      break;
    default:
      Serial.println("WENT INTO THE DEFAULT CASE :(");
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
