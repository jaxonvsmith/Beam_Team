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

bool OF_State = false;
bool Manual_State = false;
bool Up_State = false;
bool Down_State = false;
bool CW_State = false;
bool CCW_State = false;
bool Deploy_State = false;
bool Retract_State = false;
bool CW_ON = false;
bool CCW_ON = false;

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
      safety_state = SUNNY;
      Print_flag = false;
      break;
    case RETRACT:
      OF_Status = digitalRead(OF_Switch);
      Manual_Status = digitalRead(Manual_Switch);
      if (!Print_flag) {
        Serial.print("RETRACT\n");
        Print_flag = true;
      }
      if (OF_Status == HIGH) {
        current_state = DEPLOY;
        Print_flag = false;
        break;
      }
      if (Manual_Status) {
        current_state = MANUAL;
        Print_flag = false;
        break;
      }
      while (digitalRead(LimitSwitch_Retract) == LOW) {
        motors_sm.Deploy_Motor_In();
        if (digitalRead(Manual_Switch)) {
          motors_sm.Deploy_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
        if (digitalRead(OF_Switch)) {
          motors_sm.Deploy_Motor_Off();
          current_state = DEPLOY;
          Print_flag = false;
          break;
        }
      }
      motors_sm.Deploy_Motor_Off();
      Return_Flat();
      Deploy_flag = false;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY:
      OF_Status = digitalRead(OF_Switch);
      Manual_Status = digitalRead(Manual_Switch);
      if (!Print_flag) {
        Serial.print("DEPLOY\n");
        Print_flag = true;
      }
      if (OF_Status == LOW) {
        current_state = RETRACT;
        Print_flag = false;
        break;
      }
      if (Manual_Status) {
        current_state = MANUAL;
        Print_flag = false;
        break;
      }
      Serial.println("Deploy out");
      while (digitalRead(LimitSwitch_Deploy) == LOW) {
        motors_sm.Deploy_Motor_Out();
        if (digitalRead(Manual_Switch)) {
          motors_sm.Deploy_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
        if (digitalRead(OF_Switch) == LOW) {
          motors_sm.Deploy_Motor_Off();
          current_state = RETRACT;
          Print_flag = false;
          break;
        }
      }
      Serial.println("Turn off deploy motor");
      motors_sm.Deploy_Motor_Off();
      Deploy_flag = true;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY_STATUS:
      OF_Status = digitalRead(OF_Switch);
      Automatic_Status = !digitalRead(Manual_Switch);
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
      if ((OF_Status && !Deploy_flag) && Automatic_Status && (safety_state != NIGHT)) {
        current_state = DEPLOY;
        Print_flag = false;
        break;
      }
      current_state = TRACKING_STATUS;
      Print_flag = false;
      break;
    case TRACKING_STATUS:
      OF_Status = digitalRead(OF_Switch);
      Automatic_Status = !digitalRead(Manual_Switch);
      if (!Print_flag) {
        Serial.print("TRACKING STATUS\n");
        Print_flag = true;
      }
      Serial.print("OF_Status: ");
      Serial.println(OF_Status);
      Serial.print("Automatic_Status: ");
      Serial.println(Automatic_Status);
      if (OF_Status && Automatic_Status) {
        if(safety_status == SUNNY) {
          Track_flag = true;
          current_state = CHECK_POS;
          Print_flag = false;
        }
        else if(safety_status == OVERCAST) {
          current_state = FLAT;
          Print_flag = false;
        }
        else {
          current_state = WAIT;//night time, don't do anything
          Print_flag = false;
        }
      }
      else if (!OF_Status && Automatic_Status && Track_flag == true) {
        //Return_Flat(); not necessary
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
      Return_Flat();
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
        OF_Status = digitalRead(OF_Switch);
        Manual_Status = digitalRead(Manual_Switch);
        if (OF_Status == LOW) {
          current_state = DEPLOY;
          Print_flag = false;
          break;
        }
        if (Manual_Status) {
          current_state = MANUAL;
          motors_sm.Azimuth_Motor_Off();
          motors_sm.Elevation_Motor_Off();
          CW_ON = false;
          CCW_ON = false;
          Print_flag = false;
          break;
        }
        lt = analogRead(ldrlt); // top left
        rt = analogRead(ldrrt); // top right
        ld = analogRead(ldrld); // down left
        rd = analogRead(ldrrd); // down rigt
        //
        //        Serial.print("\n\n\n\n\nTop Left: ");
        //        Serial.println(lt);
        //        Serial.print("Bottom Left: ");
        //        Serial.println(ld);
        //        Serial.print("Top Right: ");
        //        Serial.println(rt);
        //        Serial.print("Bottom Right: ");
        //        Serial.println(rd);


        avt = (lt + rt) / 2; // average value top
        avd = (ld + rd) / 2; // average value down
        avl = (lt + ld) / 2; // average value left
        avr = (rt + rd) / 2; // average value right

        Serial.print("\n\n\n\n\nAverage Top: ");
        Serial.println(avt);
        Serial.print("Average Bottom: ");
        Serial.println(avd);
        Serial.print("Average Left: ");
        Serial.println(avl);
        Serial.print("Average Right: ");
        Serial.println(avr);

        dvert = avt - avd; // check the diffirence of up and down
        dhoriz = avl - avr;// check the diffirence og left and right

        if ((-1 * tol > dvert || dvert > tol)) { // check if the diffirence is smaller than the tolerance
          Serial.println("Elevation Incorrect");
          vertical_correct = false;
          system_correct = false;
        }
        else {
          Serial.println("Elevation Correct");
          motors_sm.Elevation_Motor_Off();
          vertical_correct = true;
        }

        if (-1 * tol > dhoriz || dhoriz > tol) { // check if the diffirence is in the tolerance
          Serial.println("Azimuth Incorrect");
          horizon_correct = false;
          system_correct = false;
        }
        else {
          Serial.println("Azimuth Correct");
          motors_sm.Azimuth_Motor_Off();
          horizon_correct = true;
        }
        if (horizon_correct && vertical_correct) {
          Serial.println("System Correct");
          system_correct = true;
          Serial.println("Azimuth Motor Off (8)");
          motors_sm.Azimuth_Motor_Off();
          motors_sm.Elevation_Motor_Off();
          CW_ON = false;
          CCW_ON = false;
        }
        if (!horizon_correct) {
          if (avl > avr)
          {
            if (CCW_ON) {
              motors_sm.Azimuth_Motor_Off();
              CCW_ON = false;
              delay(100);
            }
            motors_sm.Azimuth_Motor_CW();
            CW_ON = true;
            if (digitalRead(LimitSwitch_Azimuth_Center)) {
              closer_to_CW = true;
            }
            if (digitalRead(LimitSwitch_Azimuth_CW)) { //if you hit the limit then rotate past center and keep tracking
              Serial.println("Azimuth Motor Off (9)");
              motors_sm.Azimuth_Motor_Off();//turn off motor
              CW_ON = false;
              delay(1000);//small delay to make changing rotation easier on the motor
              while (digitalRead(LimitSwitch_Azimuth_Center) == LOW) { //Move past center then keep tracking
                motors_sm.Azimuth_Motor_CCW();
                CCW_ON = true;
                if (digitalRead(LimitSwitch_Azimuth_CCW)) {
                  Serial.println("Azimuth Motor Off (10)");
                  motors_sm.Azimuth_Motor_Off();//Went too far, something is wrong
                  CCW_ON = false;
                }
              }
              delay(5000); //delay to get farther past center
              Serial.println("Azimuth Motor Off (11)");
              motors_sm.Azimuth_Motor_Off();// turn off the motor
              CCW_ON = false;
              closer_to_CW = false;
            }
            //if the array moves all the way to the CCW switch then just wait for the next check
          }
          else if (avl < avr)
          {
            if (CW_ON) {
              motors_sm.Azimuth_Motor_Off();
              CW_ON = false;
              delay(100);
            }
            motors_sm.Azimuth_Motor_CCW();
            CCW_ON = true;
            if (digitalRead(LimitSwitch_Azimuth_Center)) {
              closer_to_CW = false;
            }
            if (digitalRead(LimitSwitch_Azimuth_CCW)) {
              if (CCW_ON) {
                Serial.println("Azimuth Motor Off (12)");
                motors_sm.Azimuth_Motor_Off();//turn off motor
                CCW_ON = false;
                delay(1000);//small delay to make changing rotation easier on the motor
              }
              while (digitalRead(LimitSwitch_Azimuth_Center) == LOW) { //Move past center then keep tracking
                motors_sm.Azimuth_Motor_CW();
                CW_ON = true;
                if (digitalRead(LimitSwitch_Azimuth_CW)) {
                  Serial.println("Azimuth Motor Off (13)");
                  motors_sm.Azimuth_Motor_Off();//Went too far, something is wrong
                  CW_ON = false;
                }
              }
              delay(5000); //delay to get farther past center
              Serial.println("Azimuth Motor Off (14)");
              motors_sm.Azimuth_Motor_Off();// turn off the motor
              CW_ON = false;
              closer_to_CW = true;
            }
          }
          else
          {
            Serial.println("Azimuth Position Correct!!");
            motors_sm.Azimuth_Motor_Off();
            CW_ON = false;
            CCW_ON = false;
            horizon_correct = true;
          }
        }

        if (!vertical_correct && horizon_correct) {
          Serial.println("Adjusting the elevation");
          if (avt > avd) {
            if (digitalRead(LimitSwitch_Elevation_Lower)) {
              motors_sm.Elevation_Motor_Off();
              if (-1 * tol < dhoriz && dhoriz < tol) { //if within tolerance rotate 180 and keep tracking.
                if (closer_to_CW) {
                  motors_sm.Azimuth_Motor_CCW();
                  Serial.print("made it here in if");
                  CCW_ON = true;
                  cntr = 0;
                  while (cntr <= TICK_COUNT_180) { // delay for enought time to rotate 180 degrees.
                    OF_Status = digitalRead(OF_Switch);//check to see if any switches change position.
                    Manual_Status = digitalRead(Manual_Switch);//check to see if any switches change position.
                    if (OF_Status == LOW) {
                      current_state = DEPLOY;
                      Print_flag = false;
                      break;
                    }
                    if (Manual_Status) {
                      current_state = MANUAL;
                      motors_sm.Azimuth_Motor_Off();
                      motors_sm.Elevation_Motor_Off();
                      CW_ON = false;
                      CCW_ON = false;
                      Print_flag = false;
                      break;
                    }
                    if (digitalRead(LimitSwitch_Azimuth_CCW)) { //Make sure to turn off if hits a limit switch
                      Serial.println("Limit Switch Hit");
                      motors_sm.Azimuth_Motor_Off();
                      CCW_ON = false;
                      break;
                    }
                    cntr++;
                  }

                  Serial.println("Azimuth Motor Off (16)");
                  motors_sm.Azimuth_Motor_Off();
                  CCW_ON = false;
                }
                else {
                  motors_sm.Azimuth_Motor_CW();
                  Serial.print("made it here in else");
                  CW_ON = true;
                  cntr = 0;
                  while (cntr <= TICK_COUNT_180) { // delay for enought time to rotate 180 degrees.
                    OF_Status = digitalRead(OF_Switch);//check to see if any switches change position.
                    Manual_Status = digitalRead(Manual_Switch);//check to see if any switches change position.
                    if (OF_Status == LOW) {
                      current_state = DEPLOY;
                      Print_flag = false;
                      break;
                    }
                    if (Manual_Status) {
                      current_state = MANUAL;
                      motors_sm.Azimuth_Motor_Off();
                      motors_sm.Elevation_Motor_Off();
                      CW_ON = false;
                      CCW_ON = false;
                      Print_flag = false;
                      break;
                    }
                    if (digitalRead(LimitSwitch_Azimuth_CW)) { //Make sure to turn off if hits a limit switch
                      Serial.println("Limit Switch Hit");
                      motors_sm.Azimuth_Motor_Off();
                      CW_ON = false;
                      break;
                    }
                    cntr++;
                  }
                  Serial.println("Azimuth Motor Off (17)");
                  motors_sm.Azimuth_Motor_Off();
                  CCW_ON = false;
                }
              }
            }
            motors_sm.Elevation_Motor_Down();
          }
          else if (avt < avd)
          {
            if (digitalRead(LimitSwitch_Elevation_Upper)) {
              motors_sm.Elevation_Motor_Off(); //if right is equal to left but top is greater than bottom rotate 180 and keep tracking.
            }
            else {
              motors_sm.Elevation_Motor_Up();
            }
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
      OF_State = false;
      Manual_State = false;
      /* Read the voltage of the solar panel
          safety_state 0 - Sunny, track as normal
          safety_state 1 - Overcast, Go Flat
          safety_state 2 - Night, Close up
      */
      value = analogRead(voltageSensor);
      vOUT = (value * 5.0) / 1024.0;
      vIN = vOUT / (R2 / (R1 + R2));
      Serial.print("vIN: ");
      Serial.println(vIN);
      /* Safety state:
          This will read the voltage from the solar panel and determine which state will
          be most effective for the environment.
         ------------------------------------- sunny upper (100%)
                         SUNNY
         - - - - - - - - - - - - - - - - - - -  overcast upper
         -------------------------------------- sunny lower
                       OVERCAST
         -------------------------------------- night upper
         - - - - - - - - - - - - - - - - - - - overcast lower
                         NIGHT
         -------------------------------------- night lower (5%)
      */
      if (safety_state == SUNNY) {
        if (vIN <= sunny_threashold_lower) {
          safety_state = OVERCAST;
        }
        else {
          safety_state = SUNNY;
        }
      }
      else if (safety_state == OVERCAST) {
        if (vIN >= overcast_threashold_upper) {
          safety_state = SUNNY;
        }
        else if (vIN <= overcast_threashold_lower) {
          safety_state = NIGHT;
        }
        else {
          safety_state = OVERCAST;
        }
      }
      else {
        if (vIN >= night_threashold_lower) {
          safety_state = OVERCAST;
        }
        else {
          safety_state = NIGHT;
        }
      }

      if (digitalRead(Manual_Switch)) { //Check to see if user switched to Manual
        delay(20);
        Manual_State = digitalRead(Manual_Switch);
      }
      if (digitalRead(OF_Switch)) { //Check to see if user switched OF_State
        delay(20);
        OF_State = digitalRead(OF_Switch);
      }
      if (!Print_flag) {
        Serial.print("WAIT\n");
        Print_flag = true;
      }
      if (millis() >= time_now + TRACKING_DELAY) {
        time_now += TRACKING_DELAY;
        current_state = DEPLOY_STATUS;
        Print_flag = false;
        break;
      }
      if (OF_State != Prev_OF_Switch) {
        current_state = DEPLOY_STATUS;
        Print_flag = false;
        break;
      }
      if (Manual_State) {
        current_state = MANUAL;
        Print_flag = false;
        Deploy_flag = false;
        break;
      }
      break;
    case MANUAL:
      //Debounce all of the switches
      Up_State = false;
      Down_State = false;
      CW_State = false;
      CCW_State = false;
      Deploy_State = false;
      Retract_State = false;
      Manual_State = false;
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
      if (digitalRead(Manual_Switch)) {
        delay(20);
        Manual_State = digitalRead(Manual_Switch);
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
      if (Manual_State == false) {
        Serial.println("Switching to Auto mode");
        if (digitalRead(OF_Switch)) {
          Deploy_flag = false;
        }
        else {
          Deploy_flag = true;
        }
        current_state = DEPLOY_STATUS;
        Print_flag = false;
        break;
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
        Serial.println("Azimuth Motor Off(1)");
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
        Serial.println("Azimuth Motor Off(2)");
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

void StateMachine::Return_Flat() {
  bool moving_CW;
  while (digitalRead(LimitSwitch_Azimuth_Center) == false) {
    if (digitalRead(Manual_Switch)) {
      motors_sm.Azimuth_Motor_Off();
      current_state = MANUAL;
      Print_flag = false;
      break;
    }
    if (digitalRead(OF_Switch)) {
      motors_sm.Azimuth_Motor_Off();
      current_state = DEPLOY;
      Print_flag = false;
      break;
    }
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
      Serial.println("Azimuth Motor Off(4)");
      motors_sm.Azimuth_Motor_Off();
      delay(1000); //turn off motor for a sec to improve transition
      motors_sm.Azimuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azimuth_CW) && moving_CW) {
      //you were going the wrong direction;
      Serial.println("Azimuth Motor Off (5)");
      motors_sm.Azimuth_Motor_Off();
      delay(1000); //turn off to improve transition
      motors_sm.Azimuth_Motor_CW();
      moving_CW = false;
    }
    //ADD AN ERROR CHECK IN CASE IT CANT FIND THE CENTER.
  }
  Serial.println("Azimuth Motor Off(6)");
  motors_sm.Azimuth_Motor_Off();
  while (digitalRead(LimitSwitch_Elevation_Lower) == false) {
    motors_sm.Elevation_Motor_Down();
    if (digitalRead(Manual_Switch)) {
      motors_sm.Elevation_Motor_Off();
      current_state = MANUAL;
      Print_flag = false;
      break;
    }
    if (digitalRead(OF_Switch)) {         ////////////////UNLESS IT IS DOWN FOR ENVIRONMENTAL REASONS (HEAVY RAIN)
      motors_sm.Elevation_Motor_Off();
      current_state = DEPLOY;
      Print_flag = false;
      break;
    }
  }
  motors_sm.Elevation_Motor_Off();
}
