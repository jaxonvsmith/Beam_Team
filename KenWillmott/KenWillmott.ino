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

#include <Adafruit_GPS.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

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

int user_input = 0;
double my_lon = 0.0;
double my_lat = 0.0;
int my_sec = 0;
int my_min = 0;
int my_hour = 0;
int my_day = 0;
int my_month = 0;
int my_year = 2000;
bool button_flag = false;

//// create a fixed UNIX time to test fixed time method
//int someS = 0;  //second
//int someM = 0;  //minute
//int someH = 16; //hour
//int someD = 3; //day
//int someMo = 11; //month
//int someY = 2020; //year
//tmElements_t someTime = {someS, someM, someH, 0, someD, someMo, CalendarYrToTm(someY) };
//time_t someEpochTime = makeTime(someTime);

// program begins

void setup()
{
  Serial.begin(115200);
  Serial.println(F("\tSolar Position Demo"));

  //  // set Time clock to Jan. 1, 2000
  //  setTime((time_t)now());
  //  Serial.print(F("Setting clock to "));
  //  printTime((time_t)now());

  // set the Time library time service as the time provider
  SolarPosition::setTimeProvider(now);

  ElevationServo.attach(ElevationServo_Pin);
  AzmuthServo.attach(AzmuthServo_Pin);
  // save a complete current position
  //savedPosition = Ulaanbaatar.getSolarPosition();

  //  //fixed time method
  //  Serial.print(F("TIME: "));
  //  printTime(someEpochTime);
  //  Serial.print(F("BYU:\t"));
  //  printSolarPosition(BYU.getSolarPosition(someEpochTime), digits);
  //  //  Serial.print(F("Melbourne:\t"));
  //  //  printSolarPosition(Melbourne.getSolarPosition(someEpochTime), digits);
  //  //  Serial.print(F("Timbuktu:\t"));
  //  //  printSolarPosition(Timbuktu.getSolarPosition(someEpochTime), digits);
  //  //  Serial.print(F("Ulaanbaatar:\t"));
  //  //  printSolarPosition(Ulaanbaatar.getSolarPosition(someEpochTime), digits);
  //  Serial.println();

  //GPS SETUP
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);

  //blynk setup
  Serial.println("Waiting for connections...");
  Blynk.setDeviceName("Breadboard Feather");
  Blynk.begin(auth);

}

void loop()
{
  Blynk.run();
  // read data from the GPS in the 'main loop'
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    my_hour = GPS.hour;
    if (GPS.minute < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    my_min = GPS.minute;
    if (GPS.seconds < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    my_sec = GPS.seconds;
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    my_day = GPS.day;
    Serial.print(GPS.month, DEC); Serial.print("/20");
    my_month = GPS.month;
    Serial.println(GPS.year, DEC);
    my_year = 2000 + GPS.year;
    Serial.print("MY YEAR: "); Serial.println(my_year);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitudeDegrees, 4);
      my_lat = GPS.latitudeDegrees;
      Serial.print(", ");
      Serial.println(GPS.longitudeDegrees, 4);
      my_lon = GPS.longitudeDegrees;
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }
  }
  user_input = buttons.check_buttons();
  delay(60);
  if (user_input) {
    if (button_flag == false) {
      printf("button pressed: %d \n", user_input);
      button_flag = true;
      switch (user_input) {
        case Button_1  :
          {
            ElevationServo.write(0);
            delay(15);
            AzmuthServo.write(0);
            delay(15);
          }
          break; //optional
        case Button_2  :
          {
            ElevationServo.write(180);
            delay(15);
            AzmuthServo.write(180);
            delay(15);
          }
          break;
        case Button_3  :
          {
            SolarPosition GPS_location(my_lat, my_lon); //calc sun position
            tmElements_t someTime = {my_sec, my_min, my_hour, 0, my_day, my_month, CalendarYrToTm(my_year) };
            time_t someEpochTime = makeTime(someTime);
            printSolarPosition(GPS_location.getSolarPosition(someEpochTime), digits);//print sun position
          }
          break;
        case Button_4  :
          {

          }
          break;
        case Button_5  :
          {}
          break;
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
