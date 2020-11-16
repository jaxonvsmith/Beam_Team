/*
  buttons.cpp - Library for managing button input.
  Created by Jaxon Smith on 10/16/2020
*/

#include "Arduino.h"
#include "buttons.h"

buttons::buttons(int pin_1,int pin_2, int pin_3, int pin_4, int pin_5){
  pinMode(pin_1, INPUT);
  pinMode(pin_2, INPUT);
  pinMode(pin_3, INPUT);
  pinMode(pin_4, INPUT);
  pinMode(pin_5, INPUT);
  button_1 = pin_1;
  button_2 = pin_2;
  button_3 = pin_3;
  button_4 = pin_4;
  button_5 = pin_5;
}
int buttons::check_buttons(){
  if(digitalRead(button_1)){
    return button_1;
  }
  else if(digitalRead(button_2)){
    return button_2;
  }
  else if(digitalRead(button_3)){
    return button_3;
  }
  else if(digitalRead(button_4)){
    return button_4;
  }
  else if(digitalRead(button_5)){
    return button_5;
  }
  else{
    return 0; 
  }
}
