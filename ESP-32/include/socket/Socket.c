/* Shoutout to these resources!:
 * https://stackoverflow.com/questions/23936246/error-invalid-operands-of-types-const-char-35-and-const-char-2-to-binar
 * https://randomnerdtutorials.com/esp32-esp8266-raspberry-pi-lamp-serv/
 * 
 * This is a library for setting up and sending data to the raspberry pi webserv. 
 */

#include <WiFi.h>
#include <HTTPClient.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define RasPORT 4422
#define DesPORT 5422
//IP on phone
//#define RasPi "172.20.10.3"
//IP when at home
#define RasPi "192.168.254.25"
//Actual Desktop
#define Desktop "192.168.254.20"
//Macbook
//#define Desktop "192.168.254.11"


void startWifi(char* ssid, char* password)
{
    int timer = 0;
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) { 
        delay(500);
        Serial.print(".");
        if(timer == 30)
            ESP.restart(); //Doesn't reconnect immediately
        timer++;
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void toPi(char* command)
{
    struct sockaddr_in serv_addr;
    int sock = 0;
    //int client_fd;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        ESP.restart(); //Prob ran out of resources
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(RasPORT);
    serv_addr.sin_addr.s_addr = inet_addr(RasPi);

    int client_fd;
    if ((client_fd
         = connect(sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    int valread;
    char buffer[1024] = { 0 };

    if(send(sock, command, 15, 0) <0 )
    {
        printf("Failed to send... \n");
        return;
    }
    printf("Sent %s command to Pi\n", command);
    if(command == "Stop")
        return;
    if((valread = read(sock, buffer, 1024)) <0) //Should wait to recieve 
    {
        printf("Failed to receieve...\n");
        return;
    }
    //Put some error detection in here...
    printf("%s\n", buffer);
    close(client_fd);
}

void toDesk(char* command)
{
    struct sockaddr_in serv_addr;
    int sock = 0;
    //int client_fd;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        ESP.restart(); //Prob ran out of resources
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(DesPORT);
    serv_addr.sin_addr.s_addr = inet_addr(Desktop);

    int client_fd;
    if ((client_fd
         = connect(sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    int valread;
    char buffer[1024] = { 0 };

    if(send(sock, command, 5, 0) <0 )
    {
        printf("Failed to send... \n");
        return;
    }
    printf("Sent %s command to Desktop\n",command);
    if(command == "Stop")
        return;
    //Here it should pause until the desktop sends back an "I'm done" signal, code will continue as normal
    if((valread = read(sock, buffer, 1024)) <0) //Should wait to recieve 
    {
        printf("Failed to receieve...\n");
        return;
    }
    //Put some error detection in here...
    printf("%s\n", buffer);
    close(client_fd);
}

void startCamera()
{
    toPi("Start");
    //Turn on a little status LED?
}

void stopCamera()
{
    toPi("Off");
    //Turn off little status LED?
}

void startDetection()
{
    toDesk("Start");
}

void Stop()
{
    toPi("Stop");
    toDesk("Stop");
}

