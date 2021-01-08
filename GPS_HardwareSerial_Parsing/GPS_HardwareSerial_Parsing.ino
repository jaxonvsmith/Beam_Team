// Test code for Ultimate GPS Using Hardware Serial (e.g. GPS Flora or FeatherWing)
//
// This code shows how to listen to the GPS module via polling. Best used with
// Feathers or Flora where you have hardware Serial and no interrupt
//
// Tested and works great with the Adafruit GPS FeatherWing
// ------> https://www.adafruit.com/products/3133
// or Flora GPS
// ------> https://www.adafruit.com/products/1059
// but also works with the shield, breakout
// ------> https://www.adafruit.com/products/1272
// ------> https://www.adafruit.com/products/746
//
// Pick one up today at the Adafruit electronics shop
// and help support open source hardware & software! -ada

//LSM9DS1 INFO
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>

Adafruit_Sensor *accelerometer, *gyroscope, *magnetometer;


// uncomment one combo 9-DoF!
//#include "LSM6DS_LIS3MDL.h"  // can adjust to LSM6DS33, LSM6DS3U, LSM6DSOX...
#include "LSM9DS.h"           // LSM9DS1 or LSM9DS0
//#include "NXP_FXOS_FXAS.h"  // NXP 9-DoF breakout

// pick your filter! slower == better quality output
//Adafruit_NXPSensorFusion filter; // slowest
Adafruit_Madgwick filter;  // faster than NXP
//Adafruit_Mahony filter;  // fastest/smalleset

#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM)
Adafruit_Sensor_Calibration_EEPROM cal;
#else
Adafruit_Sensor_Calibration_SDFat cal;
#endif

#define FILTER_UPDATE_RATE_HZ 100
#define PRINT_EVERY_N_UPDATES 10
//#define AHRS_DEBUG_OUTPUT

uint32_t timestamp;

//Function definitions
void printAttitude(float ax, float ay, float az, float mx, float my, float mz);

#include <Adafruit_GPS.h>
#include "SolarPosition.h"
#include <ESP32Servo.h>

Servo ElevationServo;
Servo AzmuthServo;

//Blynk Stuff
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#define ElevationServo_Pin 26
#define AzmuthServo_Pin 25
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
char auth[] = "8sa298umPY0BFSt7w_zIH-LXR9YXAZic";

#define pin_1 13
#define pin_2 27
#define pin_3 15
#define pin_4 33

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();
double our_lon = 0;
double our_lat = 0;
bool button_flag = false;
int limits = 0;
// number of decimal digits to print
const uint8_t digits = 3;

void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  while (!Serial) yield();
  //LSM9DS1 Setup
  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
  } else if (! cal.loadCalibration()) {
    Serial.println("No calibration loaded/found");
  }
  if (!init_sensors()) {
    Serial.println("Failed to find sensors");
    while (1) delay(10);
  }

  accelerometer->printSensorDetails();
  gyroscope->printSensorDetails();
  magnetometer->printSensorDetails();

  setup_sensors();
  filter.begin(FILTER_UPDATE_RATE_HZ);
  timestamp = millis();

  Wire.setClock(400000); // 400KHz
  //blynk setup
  Serial.println("Waiting for connections...");
  Blynk.setDeviceName("Breadboard Feather");
  Blynk.begin(auth);

  pinMode(pin_1, INPUT);
  pinMode(pin_2, INPUT);
  pinMode(pin_3, INPUT);
  pinMode(pin_4, INPUT);

  ElevationServo.attach(ElevationServo_Pin);
  AzmuthServo.attach(AzmuthServo_Pin);

  Serial.println("Adafruit GPS library basic test!");

  Wire.begin();

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

  SolarPosition::setTimeProvider(now);
}

void loop() // run over and over again
{
  Blynk.run();
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
    GPS.lastNMEA(); //ADDED THIS SO IT DIDNT PRINT EVERY SECOND. REMOVE AND UNCOMMENT LINE BELOW TO SWITCH BACK
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }


  // approximately every 2 seconds or so, print out the current stats
  //if (millis() - timer > 2000) {
  if ((digitalRead(pin_1)) && button_flag == false) {
    button_flag = true;
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) {
      Serial.print('0');
    }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      our_lat = GPS.latitudeDegrees;
      our_lon = GPS.longitudeDegrees;
      Serial.print("Location: ");
      Serial.print(GPS.latitudeDegrees, 4);
      Serial.print(", ");
      Serial.println(GPS.longitudeDegrees, 4);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("OUR LAT: "); Serial.println(our_lat, 4);
      Serial.print("OUR LON: "); Serial.println(our_lon, 4);
      SolarPosition GPS_location(our_lat, our_lon); //calc sun position
      tmElements_t someTime = {GPS.seconds, GPS.minute, GPS.hour, 0, GPS.day, GPS.month, CalendarYrToTm(2000 + GPS.year) };
      time_t someEpochTime = makeTime(someTime);
      printSolarPosition(GPS_location.getSolarPosition(someEpochTime), digits);//print sun position
    }
  }
  if ((digitalRead(pin_2)) && button_flag == false) {
    button_flag = true;
    if (limits == 0) {
      limits = 1;
      ElevationServo.write(0);
      delay(15);
      AzmuthServo.write(0);
      delay(15);
    }
    else if (limits == 1) {
      limits = 2;
      ElevationServo.write(90);
      delay(15);
      AzmuthServo.write(90);
      delay(15);
    }
    else {
      limits = 0;
      ElevationServo.write(180);
      delay(15);
      AzmuthServo.write(180);
      delay(15);
    }
  }
  if ((digitalRead(pin_3)) && button_flag == false) {
    //button_flag = true;
    // Read the motion sensors
  

  // print the heading, pitch and roll
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();
  Serial.print("Orientation: ");
  Serial.print(heading);
  Serial.print(", ");
  Serial.print(pitch);
  Serial.print(", ");
  Serial.println(roll);

  float qw, qx, qy, qz;
  filter.getQuaternion(&qw, &qx, &qy, &qz);
  Serial.print("Quaternion: ");
  Serial.print(qw, 4);
  Serial.print(", ");
  Serial.print(qx, 4);
  Serial.print(", ");
  Serial.print(qy, 4);
  Serial.print(", ");
  Serial.println(qz, 4);  
  
#if defined(AHRS_DEBUG_OUTPUT)
  Serial.print("Took "); Serial.print(millis()-timestamp); Serial.println(" ms");
#endif
  }
  if ((digitalRead(pin_4)) && button_flag == false) {
    button_flag = true;
  }
  if ((digitalRead(pin_1) || digitalRead(pin_2) || digitalRead(pin_3))) {
    return;
  }
  else {
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
