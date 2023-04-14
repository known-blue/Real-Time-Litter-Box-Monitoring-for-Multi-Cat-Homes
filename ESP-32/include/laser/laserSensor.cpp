/* This is just a library for manipulating the laser trip-wire
 * Link to specific sensor: https://www.waveshare.com/laser-sensor.htm
 * Note: I have it set the read pin to 26, which is right next to ground */
#include <Arduino.h>
#include <stdlib.h> //For bool
#define PIN 26 

void setPin()
{
    //Set the pin to be input
    pinMode(PIN, INPUT);
    Serial.print("Laser pin set!\n");
}

//WHEN READING: 1 is a trip (object in way)
bool readingRaw() 
{
    return digitalRead(PIN);
}

char* readingString()
{
    bool read = readingRaw();
    if(read)
        return "Not Tripped";
    else
        return "Tripped";
}