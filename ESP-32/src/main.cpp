//Pinout of the ESP32 I'm using: 
//https://makeabilitylab.github.io/physcomp/esp32/assets/images/AdafruitHuzzah32PinDiagram.png

#include <Arduino.h>
#include "laser/laserSensor.hpp"
#include "socket/Socket.h"

//Status indicators and Manual Stop button
#define Red 4
#define Green 25
#define EStop 5
//These are helpful for a "headless" implementation on the ESP

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // open the serial port at 9600 bps:
  delay(1000); //1 second?
  Serial.print("Setting pins...\n");
  setPin();
  pinMode(Green, OUTPUT); //If lit, laser is waiting for a trigger
  pinMode(Red, OUTPUT); //If  lit, In process of detecting
  pinMode(EStop, INPUT); //If for some reason program needs to end. Stops on ALL machines
  startWifi("SSID","Password");
  delay(10);
  digitalWrite(Green, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Reading: ");
  Serial.print(readingString());
  Serial.println();
  //sendData(readingString());
  if(!readingRaw()) //Tripped laser is 0
  {
    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);
    Serial.println("Starting camera!");
    startCamera();
    //Need a small delay so camera has time to start up
    delay(10000);
    Serial.println("Starting Detection!");
    startDetection(); 
    //TESTing for camera on/off
    //delay(30000); //30 secs
    Serial.println("Stopping camera, reseting!!");
    //If here, detection is completed
    stopCamera();
    Serial.print("\n\n\n"); //Give some space
    digitalWrite(Red, LOW);
    digitalWrite(Green, HIGH);
  }
  if(digitalRead(EStop)) //If button is pressed
  {
    Stop();
    //Blink both LED's indicating everything is stopped. Needs manual reset.
    while(1)
    {
      digitalWrite(Green, HIGH);
      digitalWrite(Red, HIGH);
      delay(500);
      digitalWrite(Green, LOW);
      digitalWrite(Red, LOW);
      delay(500);
    }
  }

  delay(25); //Polling rate
}