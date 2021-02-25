/*
  NES_Controller.h - Library for detachable controller.
  Created by Jaxon Smith on 2/24/2021
    __________
   /          |
  /       O 1 | 1 - Ground
  |           | 2 - Clock
  | 7 O   O 2 | 3 - Latch
  |           | 4 - Data Out
  | 6 O   O 3 | 5 - No Connection
  |           | 6 - No Connection
  | 5 O   O 4 | 7 - 5V
  |___________|
*/
#ifndef NES_Controller_h
#define NES_Controller_h

#include "Arduino.h"

class NES_Controller
{
  public:
    NES_Controller();
    byte readNesController();
  private:
    //===============================================================================
    //  Constants
    //===============================================================================
    // Here we have a bunch of constants that will become clearer when we look at the
    // readNesController() function. Basically, we will use these contents to clear
    // a bit. These are chosen according to the table above.
    const int A_BUTTON         = 0;
    const int B_BUTTON         = 1;
    const int SELECT_BUTTON    = 2;
    const int START_BUTTON     = 3;
    const int UP_BUTTON        = 4;
    const int DOWN_BUTTON      = 5;
    const int LEFT_BUTTON      = 6;
    const int RIGHT_BUTTON     = 7;

    //===============================================================================
    //  Variables
    //===============================================================================
    byte nesRegister  = 0;    // We will use this to hold current button states

    //===============================================================================
    //  Pin Declarations
    //===============================================================================
    //Inputs:
    int nesData       = 4;    // The data pin for the NES controller

    //Outputs:
    int nesClock      = 2;    // The clock pin for the NES controller
    int nesLatch      = 3;    // The latch pin for the NES controller
};

#endif
