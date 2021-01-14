#include <ESP32Servo.h>
#define button_1 13

bool button_flag = false;
bool tracking_flag = false;
bool tracking_flat = true;
bool horizontal_check = false;
bool vertical_check = false;
bool system_check = false;

Servo horizontal; // horizontal servo
int servoh = 90;     // stand horizontal servo

Servo vertical;   // vertical servo
int servov = 90;     // stand vertical servo

// LDR pin connections
//  name  = analogpin;
int ldrlt = A0; //LDR top left
int ldrrt = A1; //LDR top rigt
int ldrld = A2; //LDR down left
int ldrrd = A3 ; //ldr down rigt

void setup()
{
  Serial.begin(9600);
  // servo connections
  // name.attacht(pin);
  horizontal.attach(12);
  vertical.attach(27);
  pinMode(button_1, INPUT);
}
void loop()
{
  if (digitalRead(button_1) == HIGH) {
    button_flag = true;
    if (tracking_flag == false) {
      tracking_flag = true;
      Serial.print("...Tracking the sun...");
    }
    else {
      tracking_flag = false;
      Serial.print("...Off...");
    }
  }
  else {
    button_flag = false;
  }

  if (tracking_flag == true) {
    if(system_check == true) { //this will control the delay time between system changes. 
      delay(5000);
    }
    system_check = true; //reset the system check to see if any change is necessary
    tracking_flat = false;
    int lt = analogRead(ldrlt); // top left
    int rt = analogRead(ldrrt); // top right
    int ld = analogRead(ldrld); // down left
    int rd = analogRead(ldrrd); // down rigt



    //int dtime = analogRead(A4)/20; // read potentiometers
    int dtime = 50;
    int tol = analogRead(A5) / 4;

    int avt = (lt + rt) / 2; // average value top
    int avd = (ld + rd) / 2; // average value down
    int avl = (lt + ld) / 2; // average value left
    int avr = (rt + rd) / 2; // average value right

    int dvert = avt - avd; // check the diffirence of up and down
    int dhoriz = avl - avr;// check the diffirence og left and right
    horizontal_check = true;// reset to check if the horizontal is good
    vertical_check = true;//reset to check if the vertical is good

    if (-1 * tol > dhoriz || dhoriz > tol) // check if the diffirence is in the tolerance else change horizontal angle
    {
      horizontal_check = false;
      system_check = false;
      if (avl > avr)
      {
        servoh = --servoh;
        if (servoh < 0)
        {
          servoh = 0;
        }
      }
      else if (avl < avr)
      {
        servoh = ++servoh;
        if (servoh > 180)
        {
          servoh = 180;
        }
      }
      else if (avl = avr)
      {
        // nothing
      }
      horizontal.write(servoh);
    }

    if ((-1 * tol > dvert || dvert > tol) && horizontal_check == true) // check if the diffirence is in the tolerance else change vertical angle
    {
      vertical_check = false;
      system_check = false;
      if (avt > avd)
      {
        servov = --servov;
        if (servov > 180)
        {
          servov = 180;
        }
      }
      else if (avt < avd)
      {
        servov = ++servov;
        if (servov < 0)
        {
          servov = 0;
        }
      }
      vertical.write(servov);
    }
    delay(dtime);
  }
  else{
    if(tracking_flag == false){
      return_to_flat();
    }
  }
}

bool return_to_flat(){
  while(servoh != 90){
    if(servoh < 90){
      servoh = ++servoh;
    }
    if(servoh > 90){
      servoh = --servoh;
    }
    horizontal.write(servoh);
  }
 while(servov != 90){
    if(servov < 90){
      servov = ++servov;
    }
    if(servov > 90){
      servov = --servov;
    }
    vertical.write(servov);
  }
  tracking_flag = true;
}
