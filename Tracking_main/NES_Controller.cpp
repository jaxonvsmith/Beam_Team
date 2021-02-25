/*
  NES_Controller.cpp - Library for managing the detatchable Controller.
  Created by Jaxon Smith on 2/24/2021
*/

#include "NES_Controller.h"
#include "Arduino.h"

NES_Controller::NES_Controller() {
  // Set appropriate pins to inputs
  pinMode(nesData, INPUT);

  // Set appropriate pins to outputs
  pinMode(nesClock, OUTPUT);
  pinMode(nesLatch, OUTPUT);

  // Set initial states
  digitalWrite(nesClock, LOW);
  digitalWrite(nesLatch, LOW);

}

///////////////////////
// readNesController //
///////////////////////
byte NES_Controller::readNesController() 
{  
  // Pre-load a variable with all 1's which assumes all buttons are not
  // pressed. But while we cycle through the bits, if we detect a LOW, which is
  // a 0, we clear that bit. In the end, we find all the buttons states at once.
  int tempData = 255;
    
  // Quickly pulse the nesLatch pin so that the register grab what it see on
  // its parallel data pins.
  digitalWrite(nesLatch, HIGH);
  delay(20);
  digitalWrite(nesLatch, LOW);
 
  // Upon latching, the first bit is available to look at, which is the state
  // of the A button. We see if it is low, and if it is, we clear out variable's
  // first bit to indicate this is so.
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, A_BUTTON);
    
  // Clock the next bit which is the B button and determine its state just like
  // we did above.
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, B_BUTTON);
  
  // Now do this for the rest of them!
  
  // Select button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, SELECT_BUTTON);

  // Start button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, START_BUTTON);

  // Up button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, UP_BUTTON);
    
  // Down button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, DOWN_BUTTON);

  // Left button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, LEFT_BUTTON);  
    
  // Right button
  digitalWrite(nesClock, HIGH);
  digitalWrite(nesClock, LOW);
  if (digitalRead(nesData) == LOW)
    bitClear(tempData, RIGHT_BUTTON);
    
  // After all of this, we now have our variable all bundled up
  // with all of the NES button states.*/
  return tempData;
}
