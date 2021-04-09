/*
  Statemachine.h - Library for the State Machine.
  Created by Jaxon Smith on 1/29/2021
*/
#ifndef StateMachine_h
#define StateMachine_h

#include "Arduino.h"

class StateMachine
{
  public:
    StateMachine();
    void SM();
  private:
    //STATES:
    enum State_enum {
      INIT,
      RETRACT,
      DEPLOY,
      DEPLOY_STATUS,
      TRACKING_STATUS,
      FLAT,
      CHECK_POS,
      ADJ_POS,
      WAIT,
      MANUAL
    };
    enum Safety_enum {
      SUNNY,
      OVERCAST,
      NIGHT
    };
    void Retract();
    void Deploy();
    void Return_Flat();
    bool OF_Status;
    bool Automatic_Status;
    bool Manual_Status;
    uint8_t current_state;
    uint8_t safety_state;
    //...timer...//
    unsigned long time_now = 0;
    unsigned long time_delay = 0;
    const uint32_t TRACKING_DELAY = 450000; //5 minutes
    const uint32_t ROTATE_180 = 10000;
    //...flags...//
    //bool Prev_Deploy_Switch;
    //bool Prev_Track_Switch;
    bool Prev_OF_Switch;
    //bool Prev_Automatic_Switch;
    bool Deploy_flag;
    bool Track_flag;
    bool left_side;
    bool Print_flag;
    bool horizon_correct;
    bool vertical_correct;
    bool system_correct;
    bool closer_to_CW;
    bool safety_status;
    //...switches...//
    const uint8_t OF_Switch = 45; // This is the Deploy/Retract Switch. This only works while in "AUTO MODE"
    const uint8_t Manual_Switch = 41;
    const uint8_t Automatic_Switch = 43; // THIS IS THE ON/OFF SWITCH FOR THE SYSTEM maybe
    const uint8_t Up_Switch = 29;
    const uint8_t Down_Switch = 31;
    const uint8_t CW_Switch = 33;
    const uint8_t CCW_Switch = 35;
    const uint8_t Deploy_Switch = 37;
    const uint8_t Retract_Switch = 39;
    //..delay timer..
    const uint32_t TICK_COUNT_180 = 300;
    int cntr;//counter for delays
    //...limitswitch's...
    uint8_t LimitSwitch_Azimuth_CW = 36;
    uint8_t LimitSwitch_Azimuth_CCW = 32;
    uint8_t LimitSwitch_Azimuth_Center = 34;
    uint8_t LimitSwitch_Elevation_Upper = 28;//PCB Diagram has it labeled as 28 but in testing we found it was actually 36
    uint8_t LimitSwitch_Elevation_Lower = 30;
    uint8_t LimitSwitch_Deploy = 24;
    uint8_t LimitSwitch_Retract = 26;
    //TAKEN FROM TRACKING_H
    //Tolerance
    int tol = 40;///Adjust as necessary
    //photo resistor pins
    const uint8_t ldrlt = A3; //LDR top left
    const uint8_t ldrrt = A2; //LDR top rigt
    const uint8_t ldrld = A1; //LDR down left
    const uint8_t ldrrd = A0 ; //ldr down right
    //photo resistor values
    int lt; // top left
    int rt; // top right
    int ld; // down left
    int rd; // down rigt
    //fudge factor to account for difference in values
    double lt_fudge = 1.15; 
    double rt_fudge = 1.07;
    double ld_fudge = 1.45;
    double rd_fudge = 1.0;
    //average resistor values
    int avt; // average value top
    int avd; // average value down
    int avl; // average value left
    int avr; // average value right
    int dvert; // check the difference of up and down
    int dhoriz;// check the difference of left and right
    //..Voltage Detection..
    const int voltageSensor = A10;
    float vOUT = 0.0;
    float vIN = 0.0;
    float R1 = 30000.0;
    float R2 = 7500.0;
    int value = 0;
    const uint8_t sunny_threashold_lower = 0;
    const uint8_t sunny_threashold_upper = 0; //.........................ADJUST THESE ONCE WE HAVE THE DATA.........................
    const uint8_t overcast_threashold_lower = 0;
    const uint8_t overcast_threashold_upper = 0;
    const uint8_t night_threashold_lower = 0;
    const uint8_t night_threashold_upper = 0;
    //Wind sensor
    int wind_reading = 0;
    const uint8_t wind_sensor = A4; //.......................................CHANGE TO MATCH THE WIND SENSOR PIN......................
    const uint16_t WIND_THREASHOLD = 6000; //................................DIAL THIS IN.............................................
    //NES Controller
    byte nesRegister  = 0;    // We will use this to hold current button states
    //These values will corrilate to the values returned so we know which button is pressed
    const int A_BUTTON         = 0; //Deploy
    const int B_BUTTON         = 1; //Retract
    const int SELECT_BUTTON    = 2;
    const int START_BUTTON     = 3;
    const int UP_BUTTON        = 4; //Up
    const int DOWN_BUTTON      = 5; //Down
    const int LEFT_BUTTON      = 6; //CW
    const int RIGHT_BUTTON     = 7; //CCW
};

#endif
