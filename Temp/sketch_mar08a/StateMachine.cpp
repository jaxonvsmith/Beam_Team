/*
  StateMachine.cpp - Library for managing the state machine.
  Created by Jaxon Smith on 1/29/2021
*/

#include "StateMachine.h"
#include "motors.h"

bool OF_State = false; //Retract/Deploy switch
bool Manual_State = false; //Manual/Auto Switch
bool Up_State = false; //Up Switch
bool Down_State = false; //Down Switch
bool CW_State = false; //CW Switch(Left)
bool CCW_State = false; //CCW Switch(Right)
bool Deploy_State = false; //Deploy Switch
bool Retract_State = false; //Retract Switch

//These flags are used to control the logic of the software
bool CW_ON = false; //Keep track of if the Solar array is moving Clock-Wise
bool CCW_ON = false; //Keep track of if the Solar array is moving Counter-Clock-Wise

motors motors_sm; //create an object to be able to call the motors functions

StateMachine::StateMachine() {
  current_state = INIT; //Initialize the state machine
}

void StateMachine::SM() {
  switch (current_state) {
    case INIT:
      if (!Print_flag) {
        Serial.print("INIT\n");
        Print_flag = true;
      }
      motors_sm.INIT();
      motors_sm.Azimuth_Motor_Off();//turn off Azmuth Motor
      motors_sm.Elevation_Motor_Off();//turn off Horizon Motor
      motors_sm.Deploy_Motor_Off();//turn off Deploy Motor
      /* Initialaze all of the flags */
      Deploy_flag = false;
      Track_flag = false;
      OF_Status = false;
      Adjust_turn = AZIMUTH;
      Automatic_Status;
      left_side = false;
      horizon_correct = false;
      vertical_correct = false;
      system_correct = false;
      /* set the appropriate pins */
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
      
      current_state = DEPLOY_STATUS; //move to the next state
      safety_state = SUNNY; //Assume sunny but adjust otherwise
      Print_flag = false;
      break;
    case RETRACT:
      OF_Status = digitalRead(OF_Switch); //Check the Deploy/Retract Switch
      Manual_Status = digitalRead(Manual_Switch); //Check the Manual Auto Switch
      if (!Print_flag) { //Print out the current state
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
      Return_Flat();
      while (digitalRead(LimitSwitch_Retract) == LOW) { //Retract panels in
        motors_sm.Deploy_Motor_In();
        if (digitalRead(Manual_Switch)) { //check if the manual switch is flipped while retracting
          motors_sm.Deploy_Motor_Off();
          current_state = MANUAL;
          Print_flag = false;
          break;
        }
        if (digitalRead(OF_Switch)) { //Check to see if the deploy switch is flipped
          motors_sm.Deploy_Motor_Off();
          current_state = DEPLOY;
          Print_flag = false;
          break;
        }
      }
      motors_sm.Deploy_Motor_Off();
      Deploy_flag = false;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;
    case DEPLOY:
      OF_Status = digitalRead(OF_Switch); //Check the Deploy/Retract Switch
      Manual_Status = digitalRead(Manual_Switch); //Check the Manual/Automatic Switch
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
      while (digitalRead(LimitSwitch_Deploy) == LOW) { //Deploy motors out
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
      motors_sm.Deploy_Motor_Off();
      Deploy_flag = true;
      current_state = DEPLOY_STATUS;
      Print_flag = false;
      break;

    case DEPLOY_STATUS:
      OF_Status = digitalRead(OF_Switch); //Check Deploy/Retract Switch
      Automatic_Status = !digitalRead(Manual_Switch); //Check Manual/Auto switch
      if (!Print_flag) {
        Serial.print("DEPLOY STATUS\n");
        Print_flag = true;
      }
      Prev_OF_Switch = OF_Status;
      /* Read the voltage of the solar panel
          safety_state 0 - Sunny, track as normal
          safety_state 1 - Overcast, Go Flat
          safety_state 2 - Night, Close up
      */
      value = analogRead(voltageSensor); //Read the value on the voltage sensor
      vOUT = (value * 5.0) / 1024.0; 
      vIN = vOUT / (R2 / (R1 + R2)); //convert to actual voltage and use as safety status threasholds
      Serial.print("vIN: ");
      Serial.println(vIN);
      /* Safety state:
          This will read the voltage from the solar panel and determine which state will
          be most effective for the environment. The idea is that while it is sunny the solar array will track
          normally. While overcast it will return to flat, and at night it will close completely. A historesis
          has been put in place so it won't bounce between states. This will only check during the regular 
          checks that it automatically preforms periodically. 
                     Limits for checking

         ------------------------------------- sunny upper (100%)
                         SUNNY
         - - - - - - - - - - - - - - - - - - -  overcast upper
         -------------------------------------- sunny lower
                       OVERCAST
         -------------------------------------- night upper
         - - - - - - - - - - - - - - - - - - - overcast lower
                         NIGHT
         -------------------------------------- night lower (5%)

         If the wind is too strong the solar array will act as if it is overcast and
         return to flat for one cycle.
      */
      //     THE SAFETY STATE METHOD BELOW HAS NOT BEEN TESTED YET. VALUES NEED TO BE ADDED FOR THE THREASHOLDS
      if (safety_state == SUNNY) { 
        if (vIN <= sunny_threashold_lower) { //If threashold is below a certain level move to OVERCAST
          safety_state = OVERCAST;
        }
        else {
          safety_state = SUNNY;
        }
      }
      else if (safety_state == OVERCAST) {
        if (vIN >= overcast_threashold_upper) { //If threashold is above a certain level move to SUNNY
          safety_state = SUNNY;
        }
        else if (vIN <= overcast_threashold_lower) {//If threashold is below a certain level move to NIGHT
          safety_state = NIGHT;
        }
        else {
          safety_state = OVERCAST;
        }
      }
      else {
        if (vIN >= night_threashold_lower) { //If trheashold is above a certain level move to OVERCAST
          safety_state = OVERCAST;
        }
        else {
          safety_state = NIGHT;
        }
      }

      if (!OF_Status && Deploy_flag) { //Check to see if the Retract Switch has been triggered
        current_state = RETRACT;
        Print_flag = false;
        break;
      }
      if ((OF_Status && !Deploy_flag) && Automatic_Status && (safety_state != NIGHT)) { //Deploy
        current_state = DEPLOY;
        Print_flag = false;
        break;
      }
      current_state = TRACKING_STATUS;
      Print_flag = false;
      break;
    case TRACKING_STATUS:
      OF_Status = digitalRead(OF_Switch); //Check Deploy/Retract Switch
      Automatic_Status = !digitalRead(Manual_Switch); //Check Manual/Auto Switch
      if (!Print_flag) {
        Serial.print("TRACKING STATUS\n");
        Print_flag = true;
      }
      if (OF_Status && Automatic_Status) { //If array is deployed and in Auto mode do the appropriate action
        if (safety_status == SUNNY) {
          Track_flag = true;
          current_state = CHECK_POS;
          Print_flag = false;
        }
        else if (safety_status == OVERCAST) {
          current_state = FLAT;
          Print_flag = false;
        }
        else {
          current_state = WAIT;//night time, don't do anything
          Print_flag = false;
        }
      }
      else if (!OF_Status && Automatic_Status && Track_flag == true) { 
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

      //Not all sensors read equal values so a fudge factor is multiplied to account for this
      lt = analogRead(ldrlt); // top left
      lt = lt * lt_fudge; 
      rt = analogRead(ldrrt); // top right
      rt = rt * rt_fudge;
      ld = analogRead(ldrld); // down left
      ld = ld * ld_fudge;
      rd = analogRead(ldrrd); // down rigt
      rd = rd * rd_fudge;

      avt = (lt + rt) / 2; // average value top
      avd = (ld + rd) / 2; // average value down
      avl = (lt + ld) / 2; // average value left
      avr = (rt + rd) / 2; // average value right

      dvert = avt - avd; // check the diffirence of up and down
      dhoriz = avl - avr;// check the diffirence og left and right

      if ((-1 * tol > dvert || dvert > tol)) { // check if the vertical difference is within the tolerance
        current_state = ADJ_POS;
        vertical_correct = false;
        system_correct = false;
        Print_flag = false;
      }

      else if (-1 * tol > dhoriz || dhoriz > tol) { // check if the difference is within the tolerance
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
        OF_Status = digitalRead(OF_Switch); //Check Deploy
        Manual_Status = digitalRead(Manual_Switch); //Check Manual 
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
        lt = lt * lt_fudge;
        rt = analogRead(ldrrt); // top right
        rt = rt * rt_fudge;
        ld = analogRead(ldrld); // down left
        ld = ld * ld_fudge;
        rd = analogRead(ldrrd); // down rigt
        rd = rd * rd_fudge;

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

        //        Serial.print("\n\n\n\n\nAverage Top: ");
        //        Serial.println(avt);
        //        Serial.print("Average Bottom: ");
        //        Serial.println(avd);
        //        Serial.print("Average Left: ");
        //        Serial.println(avl);
        //        Serial.print("Average Right: ");
        //        Serial.println(avr);

        dvert = avt - avd; // check the diffirence of up and down
        dhoriz = avl - avr;// check the diffirence og left and right

        if ((-1 * tol > dvert || dvert > tol)) { // check if the diffirence is smaller than the tolerance
          Serial.println("\n\n\nElevation Incorrect");
          vertical_correct = false;
          system_correct = false;
        }
        else {
          Serial.println("Elevation Correct");
          if(Adjust_turn == ELEVATION){
            Adjust_turn = AZIMUTH;
          }
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
          if(Adjust_turn == AZIMUTH){
            Adjust_turn = ELEVATION;
          }
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
        if (!horizon_correct && (Adjust_turn == AZIMUTH)) {
          Serial.println("TURNING ELEVATION MOTOR OFF(3)");
          motors_sm.Elevation_Motor_Off(); //Make sure elevation motor is off
          if (avl > avr) //If the left is greater than the right then move CCW
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
              Serial.println("CW Limit Switch Hit... Rotate past center and keep tracking (Make sure interrupt is long enough)");
              Serial.println("Azimuth Motor Off (9) THIS ONE");
              motors_sm.Azimuth_Motor_Off();//turn off motor
              CW_ON = false;
              delay(1000);//small delay to make changing rotation easier on the motor
              while (digitalRead(LimitSwitch_Azimuth_Center) == LOW) { //Move past center then keep tracking
                if (CCW_ON == false) {
                  motors_sm.Azimuth_Motor_CCW();
                  CCW_ON = true;
                }
                if (digitalRead(LimitSwitch_Azimuth_CCW)) {
                  Serial.println("Azimuth Motor Off (10)");
                  Serial.println("Error: Make sure center limit switch is working");
                  motors_sm.Azimuth_Motor_Off();//Went too far, something is wrong
                  closer_to_CW = false;
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
              Serial.println("CCW Limit Switch Triggered... rotate past center and keep tracking");
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
                  Serial.println("Error: Check center limit switch and make sure it is working");
                  Serial.println("Azimuth Motor Off (13)");
                  closer_to_CW = true;
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
            Adjust_turn = ELEVATION;
            motors_sm.Azimuth_Motor_Off();
            CW_ON = false;
            CCW_ON = false;
            horizon_correct = true;
          }
        }

        if (!vertical_correct && (Adjust_turn == ELEVATION)) {
          motors_sm.Azimuth_Motor_Off(); //MAKE SURE AZIMUTH MOTOR IS OFF
          CW_ON = false;
          Serial.println("Adjusting the elevation");
          if (avt > avd) {
            if (digitalRead(LimitSwitch_Elevation_Lower)) {
              motors_sm.Elevation_Motor_Off();
              Serial.println("LOWER LIMIT SWITCH HIT. MOTOR SHOULD BE OFF");
              if (-1 * tol < dhoriz && dhoriz < tol) { //if within tolerance rotate 180 and keep tracking.
                Serial.println("TOP IS GREATER BUT THE RIGHT AND LEFT ARE NOT ALLIGNED, ROTATE 180 and KEEP TRACKING");
                if (closer_to_CW) {
                  motors_sm.Azimuth_Motor_CCW();
                  Serial.print("made it here in if");
                  CCW_ON = true;
                  cntr = 0;
                  while (cntr <= TICK_COUNT_180) { // delay for enough time to rotate 180 degrees.
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
                }
              }
              Serial.println("Azimuth Motor Off (17)"); ///NOT SURE ABOUT THIS
              motors_sm.Azimuth_Motor_Off();
              CCW_ON = false;
            }
            else if (-1 * tol < dvert && dvert < tol) {
              Serial.println("TURNING ELEVATION MOTOR OFF(3)");
              Adjust_turn = AZIMUTH;
              vertical_correct = true;
              motors_sm.Elevation_Motor_Off();
            }
            else {
              Serial.println("MOVING ELEVATION MOTOR DOWN");
              motors_sm.Elevation_Motor_Down();
            }
          }
          else if (avt < avd)
          {
            if (digitalRead(LimitSwitch_Elevation_Upper)) {
              Serial.println("UPPER LIMIT SWITCH TRIGGERED... TURN MOTOR OFF");
              motors_sm.Elevation_Motor_Off(); //if right is equal to left but top is greater than bottom rotate 180 and keep tracking.
            }
            else if (-1 * tol < dvert && dvert < tol) {
              Serial.println("TURNING ELEVATION MOTOR OFF(2)");
              Adjust_turn = AZIMUTH;
              vertical_correct = true;
              motors_sm.Elevation_Motor_Off();
            }
            else {
              Serial.println("UPPER LIMIT NOT MET");
              motors_sm.Elevation_Motor_Up();
            }
          }
          else {
            Serial.println("TURNING ELEVATION MOTOR OFF(1)");
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
      if (digitalRead(Manual_Switch)) { //Check to see if user switched to Manual
        delay(20);
        Manual_State = digitalRead(Manual_Switch);
      }
      wind_reading = analogRead(wind_sensor);
      //      Serial.print("Wind Sensor Reading: ");
      //      Serial.println(wind_reading);
      if (digitalRead(OF_Switch)) { //Check to see if user switched OF_State
        delay(20);
        OF_State = digitalRead(OF_Switch);
      }
      if (!Print_flag) {
        Serial.print("WAIT\n");
        Print_flag = true;
      }
      if (wind_reading >= WIND_THREASHOLD) {       //If the wind passes a safe limit then go flat for one sleep cycle
        Serial.println("Wind Too Fast... Returning to flat");
        time_now += (TRACKING_DELAY);
        current_state = FLAT;
        Print_flag = false;
        break;
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
          if (digitalRead(LimitSwitch_Azimuth_Center)) {
            closer_to_CW = true;
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
          if (digitalRead(LimitSwitch_Azimuth_Center)) {
            closer_to_CW = false;
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
  Serial.println("\n\n\nReturn to flat");
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
    while (digitalRead(LimitSwitch_Elevation_Lower) == false) {
      motors_sm.Elevation_Motor_Down();
      if (digitalRead(Manual_Switch)) {
        motors_sm.Elevation_Motor_Off();
        current_state = MANUAL;
        Print_flag = false;
        break;
      }
      if (digitalRead(OF_Switch)) {        
        motors_sm.Elevation_Motor_Off();
        current_state = DEPLOY;
        Print_flag = false;
        break;
      }
    }
    motors_sm.Elevation_Motor_Off();
    if (closer_to_CW && !digitalRead(LimitSwitch_Azimuth_CCW)) {
      motors_sm.Azimuth_Motor_CCW();
      moving_CW = false;
    }
    else if (!closer_to_CW && !digitalRead(LimitSwitch_Azimuth_CW)) {
      motors_sm.Azimuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azimuth_CCW) && !moving_CW) {
      Serial.println("Going the wrong direction, switch directions");//you were going the wrong direction;
      Serial.println("Azimuth Motor Off(4)");
      motors_sm.Azimuth_Motor_Off();
      closer_to_CW = false;
      delay(1000); //turn off motor for a sec to improve transition
      motors_sm.Azimuth_Motor_CW();
      moving_CW = true;
    }
    if (digitalRead(LimitSwitch_Azimuth_CW) && moving_CW) {
      Serial.println("Going the wrong direction, switch directions");//you were going the wrong direction;
      Serial.println("Azimuth Motor Off (5)");
      motors_sm.Azimuth_Motor_Off();
      closer_to_CW = true;
      delay(1000); //turn off to improve transition
      motors_sm.Azimuth_Motor_CCW();
      moving_CW = false;
    }
    //ADD AN ERROR CHECK IN CASE IT CANT FIND THE CENTER.
  }
  Serial.println("Azimuth Motor Off(6)");
  motors_sm.Azimuth_Motor_Off();
}
