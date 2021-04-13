# Beam_Team

This repository has been simplified to include only the relevent code. The current version of code for the full size prototype can be found under the Temp folder. We used an Arduio Mega as the microcontroller to control the array. 
Documentation for setting up and using an Arduino Mega: https://www.arduino.cc/en/Guide/ArduinoMega2560

Not all of features have been implimented. We were hoping to attach a potentiometer to measure wind and go flat if wind speed could damage the array. The code should be ready to just add threashold for "WIND_THREASHOLD" found in the StateMachine.h file.

The same is true for the functionalitly to go flat during overcast and close at night. Thresholds need to be added based on the voltage being read from the solar panel that is connected with the system battery. These threasholds can also be found in the StateMachine.h file.

We also spent time trying to figure out a method that just used GPS in combination with a LSM9DS1 sensor. The progress that we made can be found in the GPS_HardwareSerial_Parsing. We pivoted because of the constant calibration that would be required to make this work. We were able to figure out how to calculate the suns position based on the information we recieve from the GPS but we found the LDR sensor method much more robust and easier for the user. 