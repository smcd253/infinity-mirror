#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#define ADAFRUIT_NEOPIXEL_H 
#define NUM_LEDS 69 
#define PIN 3 
#define DEVICE_NAME "Mirror"


//Wifi settings
const char* ssid = "spencer_iphone";
const char* password = "weedistight";
IPAddress ip(172,20,10,20);
/*static ip*/
IPAddress gateway(172,20,10,1);   
IPAddress subnet(255,255,255,0); 

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void handleRoot();              // function prototypes for HTTP handlers
void handle_mirror();
void handleNotFound();
void theaterChase(byte red, byte green, byte blue, int SpeedDelay);

void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println("Starting LED-web-server");
  Serial.println('\n');
  if(WiFi.hostByName(DEVICE_NAME, ip) == 0){
    Serial.printf("could not configure host name to ip\n");
  };
  if(WiFi.config(ip, gateway, subnet) == false){
    Serial.printf("could not complete WiFi.config()\n");
  };
  
  Serial.printf("Connecting to %s...", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/LED", HTTP_POST, handle_mirror);  // Call the 'handle_mirror' function when a POST request is made to URI "/LED"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html", "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
}

void handle_mirror() {  
  Serial.print("handle_mirror\n");
  for (int i = 0; i < 100; i++) {
    theaterChase((byte)100, (byte)100, (byte)100, 10);
  }  
                        // If a POST request is made to URI /LED
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      strip.show();
      Serial.print("strip.show()");
     
      delay(SpeedDelay);
     
      for (int i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}

void loop(void){
  server.handleClient();                    // Listen for HTTP requests from clients
}