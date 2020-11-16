// solarTimeDemo

// Arduino example sketch for SolarPosition library
//
// Calculate solar position from time and location information
// using Time library functions (relying on CPU clock based timer) to keep time.

// 2017 Ken Willmott
// Arduino library based on the program "Arduino Uno and Solar Position Calculations"
// (c) David R. Brooks, which can be found at http://www.instesre.org/ArduinoDocuments.htm
// and issued under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License:
// https://creativecommons.org/licenses/by-nc-nd/4.0/

#include "SolarPosition.h"
#include "buttons.h"
#include <ESP32Servo.h>

Servo ElevationServo;
Servo AzmuthServo;

// number of decimal digits to print
const uint8_t digits = 3;

//Blynk Stuff
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
char auth[] = "8sa298umPY0BFSt7w_zIH-LXR9YXAZic";
//WidgetLCD lcd(V1);

#define Button_1 13 //White Wire(FLUSH)
#define Button_2 12 //Yellow Wire(FILL)
#define Button_3 27 //Purple Wire(MIX)
#define Button_4 33 //Grey Wire(COLOR TEST)
#define Button_5 15 //Dispense Motor
#define ElevationServo_Pin 26
#define AzmuthServo_Pin 25
#define NO_INPUT 0

buttons buttons(Button_1, Button_2, Button_3, Button_4, Button_5);
// some test positions:
SolarPosition BYU(40.2518476, -111.6515043);  // BYU Campus
SolarPosition Timbuktu(16.775214, -3.007455); // Timbuktu, Mali, Africa
//SolarPosition Melbourne(-37.668987, 144.841006); //Melbourne Airport (MEL)
//SolarPosition Ulaanbaatar(47.847410, 106.769004); //Ulaanbaatar Airport (ULN)

// A solar position structure to demonstrate storing complete positions
SolarPosition_t savedPosition;

int user_input;
bool button_flag = false;

// create a fixed UNIX time to test fixed time method
int someS = 0;  //second
int someM = 0;  //minute
int someH = 16; //hour
int someD = 3; //day
int someMo = 11; //month
int someY = 2020; //year
tmElements_t someTime = {someS, someM, someH, 0, someD, someMo, CalendarYrToTm(someY) };
time_t someEpochTime = makeTime(someTime);

// program begins

void setup()
{
  Serial.begin(115200);
  Serial.println(F("\tSolar Position Demo"));

  // set Time clock to Jan. 1, 2000
  setTime((time_t)now());
  Serial.print(F("Setting clock to "));
  printTime((time_t)now());

  // set the Time library time service as the time provider
  SolarPosition::setTimeProvider(now);

  ElevationServo.attach(ElevationServo_Pin);
  AzmuthServo.attach(AzmuthServo_Pin);
  // save a complete current position
  //savedPosition = Ulaanbaatar.getSolarPosition();

  //fixed time method
  Serial.print(F("TIME: "));
  printTime(someEpochTime);
  Serial.print(F("BYU:\t"));
  printSolarPosition(BYU.getSolarPosition(someEpochTime), digits);
//  Serial.print(F("Melbourne:\t"));
//  printSolarPosition(Melbourne.getSolarPosition(someEpochTime), digits);
//  Serial.print(F("Timbuktu:\t"));
//  printSolarPosition(Timbuktu.getSolarPosition(someEpochTime), digits);
//  Serial.print(F("Ulaanbaatar:\t"));
//  printSolarPosition(Ulaanbaatar.getSolarPosition(someEpochTime), digits);
  Serial.println();

   //blynk setup
  Serial.println("Waiting for connections...");
  Blynk.setDeviceName("Breadboard Feather");
  Blynk.begin(auth);
}

void loop()
{
 Blynk.run();
  user_input = buttons.check_buttons();
  delay(60);
  if (user_input) {
    if (button_flag == false) {
      printf("button pressed: %d \n", user_input);
      button_flag = true;
      switch (user_input) {
        case Button_1  :
          ElevationServo.write(0);
          delay(15);
          AzmuthServo.write(0);
          delay(15);
          
          break; //optional
        case Button_2  :
          ElevationServo.write(180);
          delay(15);
          AzmuthServo.write(180);
          delay(15);
          break;
        case Button_3  :
//          ElevationServo.write(BYU.getSolarPosition(someEpochTime).elevation);
//          delay(15);
//          AzmuthServo.write(BYU.getSolarPosition(someEpochTime).azimuth);
//          delay(15);
          ElevationServo.write(Timbuktu.getSolarPosition(someEpochTime).elevation);
          delay(15);
          AzmuthServo.write(Timbuktu.getSolarPosition(someEpochTime).azimuth);
          delay(15);
          break;
        case Button_4  :
          break;
        case Button_5  :
        default : //Optional
          printf("ERROR: INPUT NOT RECOGNIZED\n");
      }
    }
  }
  if (user_input == NO_INPUT && button_flag == true) {
    button_flag = false;
  }
}

// Print a solar position to serial
//
void printSolarPosition(SolarPosition_t pos, int numDigits)
{
  Serial.print(F("el: "));
  Serial.print(pos.elevation, numDigits);
  Serial.print(F(" deg\t"));

  Serial.print(F("az: "));
  Serial.print(pos.azimuth, numDigits);
  Serial.println(F(" deg"));
}

// Print a time to serial
//
void printTime(time_t t)
{
  tmElements_t someTime;
  breakTime(t, someTime);

  Serial.print(someTime.Hour);
  Serial.print(F(":"));
  Serial.print(someTime.Minute);
  Serial.print(F(":"));
  Serial.print(someTime.Second);
  Serial.print(F(" UTC on "));
  Serial.print(dayStr(someTime.Wday));
  Serial.print(F(", "));
  Serial.print(monthStr(someTime.Month));
  Serial.print(F(" "));
  Serial.print(someTime.Day);
  Serial.print(F(", "));
  Serial.println(tmYearToCalendar(someTime.Year));
}
