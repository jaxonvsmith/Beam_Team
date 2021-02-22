/*
  buttons.h - Library for managing button input.
  Created by Jaxon Smith on 10/16/2020
*/
#ifndef buttons_h
#define buttons_h

#include "Arduino.h"

class buttons
{
  public:
    buttons(int pin_1, int pin_2, int pin_3, int pin_4, int pin_5);
    int check_buttons();
  private:
    int button_1;
    int button_2;
    int button_3;
    int button_4;
    int button_5;
};

#endif
