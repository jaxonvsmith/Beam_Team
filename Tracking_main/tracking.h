/*
  motors.h - Library for photo resistor tracking system.
  Created by Jaxon Smith on 1/29/2021
*/
#ifndef tracking_h
#define tracking_h

#include "Arduino.h"

class tracking
{
  public:
    tracking();
    bool pos_correct();
    void adj_pos();
    void INIT();
  private:
    //Tolerance
    int tol = 5;///Adjust as necessary
    //photo resistor pins
    const uint8_t ldrlt = A0; //LDR top left
    const uint8_t ldrrt = A1; //LDR top rigt
    const uint8_t ldrld = A2; //LDR down left
    const uint8_t ldrrd = A3 ; //ldr down rigt
    //photo resistor values
    int lt; // top left
    int rt; // top right
    int ld; // down left
    int rd; // down rigt
    //average resistor values
    int avt; // average value top
    int avd; // average value down
    int avl; // average value left
    int avr; // average value right
    int dvert; // check the difference of up and down
    int dhoriz;// check the difference of left and right

};

#endif
