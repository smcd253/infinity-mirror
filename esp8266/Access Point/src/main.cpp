#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library

const char *ssid = "Kira570"; // The name of the Wi-Fi network that will be created
const char *password = "kirarock5";   // The password required to connect to it, leave blank for an open network

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  if(!WiFi.softAPConfig(IPAddress(192, 168, 68, 1), IPAddress(192, 168, 68, 1), IPAddress(255, 255, 255, 0))){
      Serial.println("AP Config Failed");
  }
  
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer
}

void loop() { }