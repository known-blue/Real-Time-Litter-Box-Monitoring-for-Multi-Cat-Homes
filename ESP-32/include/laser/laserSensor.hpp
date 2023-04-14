#pragma once
/* This is just a library for manipulating the laser trip-wire
 * Link to specific sensor: https://www.waveshare.com/laser-sensor.htm
 * Note: I have it set the read pin to 26, which is right next to ground */

#include "laserSensor.cpp"
#include <stdlib.h> //For bool


//The laser sensor uses GPIO pin 26 here, right next to ground
void setPin();

//For printing string of tripped/not tripped
char* readingString(); 

//For reading the raw 0/1, 0 = object in way
bool readingRaw(); 

