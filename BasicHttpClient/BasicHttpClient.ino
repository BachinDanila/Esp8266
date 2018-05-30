/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

#define ssid "Home"
#define password "1234Qawsed"
#define addr "http://192.168.1.3/"

void setup() {

    Serial.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    delay(4000);
    WiFiMulti.addAP(ssid, password);

}

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        http.setTimeout(500000);
        Serial.print("[HTTP] begin...\n");
        
        String request = addr +  String("!!!!!!!!!!!!!");
        http.begin(request); //HTTP

        Serial.print("[HTTP] GET...\n");
        
        int httpCode = http.GET();
        if(httpCode > 0) {
            
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } 
        else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }
}

