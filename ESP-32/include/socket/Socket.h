#pragma once
/* Shoutout to these resources!:
 * https://stackoverflow.com/questions/23936246/error-invalid-operands-of-types-const-char-35-and-const-char-2-to-binar
 * https://randomnerdtutorials.com/esp32-esp8266-raspberry-pi-lamp-server/
 * 
 * This is a library for setting up and sending data to the raspberry pi and Desktop
 */

#include "Socket.c"

//Connects ESP to the WiFi
void startWifi(char* ssid, char* password);
//Sends command to start the camera on the Raspberry Pi
void startCamera();
//Sends command to start object detection on Desktop, pauses here while detection is running
void startDetection(); //All done on RPI now
//Sends command to turn off the camera on the Raspberry Pi
void stopCamera();
//"EStop". Ends all programs on other machines, needs to be manually reset
void Stop();

//Creates socket connection to the raspberry
void toPi(char* command);
void toDesk(char* command);