/*
  motors.cpp - Library for managing the motors.
  Created by Jaxon Smith on 2/1/2021
*/

#include "tracking.h"
#include "motors.h"

//motors
motors motors_t;

tracking::tracking() {

}

void tracking::INIT() {
  motors_t.INIT();
}

bool tracking::pos_correct() {
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
    return false;
  }

  if (-1 * tol > dhoriz || dhoriz > tol) { // check if the diffirence is in the tolerance
    return false;
  }
  return true;
}
void tracking::adj_pos() {
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
      motors_t.Servo_Left();
    }
    else if (avl < avr)
    {
      motors_t.Servo_Right();
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
      motors_t.Servo_Up();
    }
    else if (avt < avd)
    {
      motors_t.Servo_Down();
    }
    else if (avt == avd) {
      //nothing
    }
  }
}
