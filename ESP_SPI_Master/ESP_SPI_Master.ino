#include <SPI.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "base64.h"

#define ssid "Home"
#define password "1234Qawsed"
#define addr "http://192.168.1.3/"
#define CS 15 //define chip select line for manual control
#define font ArialMT_Plain_16

#define TAB         9
#define ENTER       13
#define BACKSPACE   127
#define ESC         27
#define INSERT      0
#define DELETE      127
#define PAGEUP      25
#define PAGEDOWN    26
#define UPARROW     11
#define LEFTARROW   8
#define DOWNARROW   10
#define RIGHTARROW  21
#define Error 255

#define KC_BREAK  0xf0
#define KC_ENTER  0x0d
#define KC_ESC    0x76
#define KC_KPLUS  0x79
#define KC_KMINUS 0x7b
#define KC_KMULTI 0x7c
#define KC_NUM    0x77
#define KC_BKSP   0x66

SSD1306Wire  display(0x3c, D2, D1);
ESP8266WiFiMulti WiFiMulti;

String str = "";
String master_request = "";
String master_response = "";

void setup()
{
    pinMode(CS, OUTPUT);                                       //configure the line as output
    Serial.begin(115200);                                      //Set UART baud rate
    digitalWrite(CS, HIGH);                                    //Initialize the CS line to HIGH
    SPI.begin();                                               //Initialize the SPI module--> configures the MOSI, MISO and CLOCK lines
    display.init();

    display.flipScreenVertically();
    display.setFont(font);
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    delay(4000);
    WiFiMulti.addAP(ssid, password);
}
 
void loop()
{
    int spi_dat;
    display.clear();
    
    spi_dat = recievefromarduino(str);
    
    str = recognize(spi_dat,str);
    
    display.drawStringMaxWidth(0, 0, 128, str);//UART - Print the data received from the slave
    display.display();
    delay(100);                                               //Delay of 1s
}

int recievefromarduino(String string){
  digitalWrite(CS, LOW);                                     //Pull CS Line Low
  SPI.transfer(0x02);                                        //Send a byte (0x02) to the slave i.e. Arduino UNO
  digitalWrite(CS, HIGH);                            //Pull CS Line High
  delayMicroseconds(10);                                     //Give some time for the slave to process/do something with the recived data

  digitalWrite(CS, LOW);                                     //Pull CS Line Low
  int data = SPI.transfer(0x00);                              //Received the processed data byte from the slave
  digitalWrite(CS, HIGH);                                    //Pull CS Line High
  return data;
}

String sendtoserver(String data){
  String response = "None";
  if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;                  //Http begin
        http.setTimeout(500000);
        
        String request = addr + base64::encode(data);
        http.begin(request); //HTTP
  
        int httpCode = http.GET();      //Http Get
        
        if(httpCode > 0) {
          if(httpCode == HTTP_CODE_OK) {
            response = http.getString();
          }
          else{
            response = "Unknown Response";
          }
        } 
        else {
           response = http.errorToString(httpCode).c_str();
        }
        http.end();
    }
    return response;
}

String recognize(int spi_dat,String str){
  switch (spi_dat) {
    case Error:
      return str;
      break;
    case ESC:
      return "";
      break;
    case ENTER:
      master_response = sendtoserver(str);
      delay(10);
      master_send(master_response);
      delay(10);
      return master_response;
      break;
    case BACKSPACE:
      return str.substring(0,str.length() - 1);
      break;
    default:
      return str + char(spi_dat);
      break;
  }
}
void send_raw(int b){
    digitalWrite(CS, LOW);                                     //Pull CS Line Low
    SPI.transfer(b);                                        //Send a byte (0x02) to the slave i.e. Arduino UNO
    digitalWrite(CS, HIGH); 
    delayMicroseconds(10);
}

void master_send(String mas){
  for(int i=0;i<=mas.length();i++){
    send_raw(int(mas[i]));
  }
}

