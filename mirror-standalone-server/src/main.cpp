/*ESP8266WiFiWebServer
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 */

/*The following project leans heavily on the NeoPixel library from Adafruit */
/* Many thanks to Adafruit for the NeoPixel library and NeoCandle code. */
/* https://www.adafruit.com/ */


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "../lib/html_pages.h"

#define PIN 3
#define NUMPIXELS 69                // Number of LED's in your strip

// START OF CANDLE MODE RELATED STUFF ////////////////////////////////////////////////
#define CANDLEPIXELS 35      //This is how may led's will represent your 'wick'. With a 60 LED strip, 30 gives the effect of a tealight or small candle when in an opaque container.
// END OF CANDLE MODE RELATED STUFF //////////////////////////////////////////////////


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// START OF CANDLE MODE RELATED STUFF ////////////////////////////////////////////////
Adafruit_NeoPixel *wick;
byte state;
unsigned long flicker_msecs;
unsigned long flicker_start;
byte index_start;
byte index_end;
// END OF CANDLE MODE RELATED STUFF ////////////////////////////////////////////////////

#define DEVICE_NAME "Mirror"

//Wifi settings
const char* ssid = "BarbaraHill1941-2.4";
const char* password = "Risdal111";
IPAddress ip(10,0,0,69);
/*static ip*/
IPAddress gateway(10,0,0,1);   
IPAddress subnet(255,255,255,0);

MDNSResponder mdns;

ESP8266WebServer server ( 80 );

const int delayval = 500;
String rgb_now = "#0000ff";    //global rgb state values for use in various html pages
long red_int = 0;
long green_int = 0;
long blue_int = 255;
int brightness = 255;
int mode_flag = 1;

// START OF CANDLE MODE RELATED STUFF ////////////////////////////////////////////////
#define WICK_PIN                PIN
// Any unconnected pin, to try to generate a random seed
#define UNCONNECTED_PIN         2
 
// The LED can be in only one of these states at any given time
#define BRIGHT                  0
#define UP                      1
#define DOWN                    2
#define DIM                     3
#define BRIGHT_HOLD             4
#define DIM_HOLD                5
 
// Percent chance the LED will suddenly fall to minimum brightness
#define INDEX_BOTTOM_PERCENT    10
// Absolute minimum red value (green value is a function of red's value)
#define INDEX_BOTTOM            128
// Minimum red value during "normal" flickering (not a dramatic change)
#define INDEX_MIN               192
// Maximum red value
#define INDEX_MAX               255
 
// Decreasing brightness will take place over a number of milliseconds in this range
#define DOWN_MIN_MSECS          20
#define DOWN_MAX_MSECS          250
// Increasing brightness will take place over a number of milliseconds in this range
#define UP_MIN_MSECS            20
#define UP_MAX_MSECS            250
// Percent chance the color will hold unchanged after brightening
#define BRIGHT_HOLD_PERCENT     20
// When holding after brightening, hold for a number of milliseconds in this range
#define BRIGHT_HOLD_MIN_MSECS   0
#define BRIGHT_HOLD_MAX_MSECS   100
// Percent chance the color will hold unchanged after dimming
#define DIM_HOLD_PERCENT        5
// When holding after dimming, hold for a number of milliseconds in this range
#define DIM_HOLD_MIN_MSECS      0
#define DIM_HOLD_MAX_MSECS      50
 
#define MINVAL(A,B)             (((A) < (B)) ? (A) : (B))
#define MAXVAL(A,B)             (((A) > (B)) ? (A) : (B))
 
// END OF CANDLE MODE RELATED STUFF ////////////////////////////////////////////////////

void handleIndex();
void handleNotFound();
void handleSwitchOn();
void handleSwitchOff();
void handleSetColour();
void handleColour();
void handleSetBrightness();
void handleBrightness();
void handleSelectMode();
void handle_mode1();
void handle_mode2();
void handle_mode3();
void handle_mode4();
void light_up_all();
void turn_off_all();
uint32_t Wheel(byte);

void setup ( void ) {
	Serial.begin ( 115200 );
  Serial.println("Starting ESP8266-Reactive-Server");
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

  EEPROM.begin(5);                           // set up EEPROM storage space for config values

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	if ( mdns.begin ( "esp8266", WiFi.localIP() ) ) {
		Serial.println ( "MDNS responder started" );
	}

  Serial.println ( "HTTP server started" );

  server.on ( "/", handleIndex );
  server.onNotFound ( handleNotFound );
  
  server.on ( "/switch_on", handleSwitchOn);
  server.on ( "/switch_off", handleSwitchOff);
  server.on ( "/set_colour", handleSetColour);
  server.on ( "/set_colour_hash", handleColour );
  server.on ( "/set_brightness", handleSetBrightness);
  server.on ( "/set_bright_val", handleBrightness);
  server.on ( "/select_mode", handleSelectMode);
  server.on ( "/set_mode1", handle_mode1); 
  server.on ( "/set_mode2", handle_mode2);
  server.on ( "/set_mode3", handle_mode3);
  server.on ( "/set_mode4", handle_mode4);

//EEPROM Memory//
//Mem Location ||--0--||--1--||--2--||--3--||--4--||--5--||--6--||
//                red   green  blue   bright  mode

  red_int = EEPROM.read(0);       //restore colour to last used value. Ensures RGB lamp is same colour as when last switched off
  green_int = EEPROM.read(1);
  blue_int = EEPROM.read(2);
  brightness = EEPROM.read(3);
  mode_flag = EEPROM.read(4);
	
	server.begin();
  strip.setBrightness(50);
  strip.begin();
  strip.show();


  handleSwitchOn();
}

// void handleNotFound() {
//   server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
// }

void loop ( void ) {
	mdns.update();
	server.handleClient();
  return;
  }
 
void handleIndex(){
Serial.println ( "Request for index page received" );
server.send ( 200, "text/html", page_contents);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

 void handleSwitchOn() {
  mode_flag = EEPROM.read(4);                       // start-up in last saved mode
  delay(100);
  switch(mode_flag){
      case 1:handle_mode1();
      break;
      case 2:handle_mode2();
      break;
      case 3:handle_mode3();
      break;
      case 4:handle_mode4();
      break;
      default:                  
           light_up_all();                          //Default to fixed colour should the EEProm become corrupted
      break;
  }
  server.send ( 200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>" );
  };  
  
 void handleSwitchOff() {
  mode_flag=1;                                       //go to default fixed color mode and turn off all pixels
  delay(100);
  turn_off_all();
  server.send ( 200, "text/html", "<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>" );
  }
  
void handleSetColour() {     
        server.send ( 200, "text/html", colour_picker);
  }
 
  
void handleSetBrightness(){
   server.send ( 200, "text/html", bright_set);
  }  

    
void handleSelectMode(){
  server.send ( 200, "text/html", mode_page );
//  Serial.println ( "Mode select page" );
  }


void handleColour(){
  char buf_red[3];                               //char buffers to hold 'String' value converted to char array
  char buf_green[3];                       
  char buf_blue[3];                       
  String message = server.arg(0);                //get the 1st argument from the url which is the hex rgb value from the colour picker ie. #rrggbb (actually %23rrggbb)
  rgb_now = message; 
  rgb_now.replace("%23", "#");                   // change %23 to # as we need this in one of html pages
  String red_val = rgb_now.substring(1,3);       //extract the rgb values
  String green_val = rgb_now.substring(3,5); 
  String blue_val = rgb_now.substring(5,7);

  mode_flag=1;                                   //get to fixed colour mode if not already

  red_val.toCharArray(buf_red,3);                //convert hex 'String'  to Char[] for use in strtol() 
  green_val.toCharArray(buf_green,3);           
  blue_val.toCharArray(buf_blue,3);             

  red_int = gamma_adjust[strtol( buf_red, NULL, 16)];          //convert hex chars to ints and apply gamma adjust
  green_int = gamma_adjust[strtol( buf_green, NULL, 16)];
  blue_int = gamma_adjust[strtol( buf_blue, NULL, 16)];

 EEPROM.write(0,red_int);                  //write the colour values to EEPROM to be restored on start-up
 EEPROM.write(1,green_int);
 EEPROM.write(2,blue_int);
 EEPROM.commit();

   light_up_all(); 
   String java_redirect = "<SCRIPT language='JavaScript'>window.location='/set_colour?";
          java_redirect += message;                                                 //send hash colour value in URL to update the colour picker control
          java_redirect += "';</SCRIPT>";
   server.send ( 200, "text/html", java_redirect );                                 // all done! - take user back to the colour picking page                                                                                          
  }

 void handleBrightness(){
  String message = server.arg(0);                //get the 1st argument from the url which is the brightness level set by the slider
  String bright_val = message.substring(0,3);    //extract the brightness value from the end of the argument in the URL
  brightness =  bright_val.toInt();
  EEPROM.write(3,brightness);                    //write the brightness value to EEPROM to be restored on start-up
  EEPROM.commit();

   String java_redirect = "<SCRIPT language='JavaScript'>window.location='/set_brightness?";
          java_redirect += brightness;                                              //send brightness value in URL to update the slider control
          java_redirect += "';</SCRIPT>";
   server.send ( 200, "text/html", java_redirect);                                  // all done! - take user back to the brightness selection page                                                                                          
  }

  void light_up_all(){
  for(int i=0;i<NUMPIXELS;i++){      
   strip.setPixelColor(i, strip.Color(brightness*red_int/255,brightness*green_int/255,brightness*blue_int/255));                            // Set colour with gamma correction and brightness adjust value. 
   strip.show();}                                                                                                                            
  }

  void turn_off_all(){
   mode_flag=999;                                       //go to non-existent mode and turn off all pixels
  for(int i=0;i<NUMPIXELS;i++){                                                                                                              // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
   strip.setPixelColor(i, strip.Color(0,0,0));                                                                                               // Turn off led strip
   strip.show();}                                                                                                                            // This sends the updated pixel color to the hardware.
  }  

void handle_mode1(){                                  //fixed colour mode
  mode_flag = 1;
  EEPROM.write(4,mode_flag);                          //write mode to EEProm so can be restored on start-up
  EEPROM.commit();
  server.send ( 200, "text/html","<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
                                   
  while(mode_flag==1){                                // Check the mode hasn't been changed whilst we wait, if so - leave immediately
            light_up_all();                           //set mode to default state - all led's on, fixed colour. This loop will service any brightness changes
            loop();                                   // Not much to do except service the main loop       
  }return;          
}


void handle_mode2(){                                 //colour fade mode
  mode_flag = 2;
  EEPROM.write(4,mode_flag);                         //write mode to EEProm so can be restored on start-up
  EEPROM.commit();
  server.send ( 200, "text/html","<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
  uint16_t i, j, k;
  int wait = 10;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266

 while(mode_flag==2){
  for(j=0; j<256; j++) {
   loop();
    for(i=0; i<NUMPIXELS; i++) {
      loop();
      strip.setPixelColor(i, Wheel((j) & 255));
      strip.show();
    } loop(); 
      
     for(k=0; k < 200; k++){                          // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
      delay(wait);
      loop();
     }loop();}
     return;}
}

void handle_mode3(){                                //rainbow mode
  mode_flag = 3;
  EEPROM.write(4,mode_flag);                        //write mode to EEProm so can be restored on start-up
  EEPROM.commit();
  server.send ( 200, "text/html","<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
  uint16_t i, j, k;
  int wait = 10;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266
  int wait_num = 10;
  while(mode_flag==3){                               // do this indefenitely or until mode changes
    for(j=0; j < 256*5; j++) {                        // 5 cycles of all colors on wheel
    loop();
      for(i=0; i < NUMPIXELS; i++) {
        loop();
        strip.setPixelColor(i,Wheel(((i * 256 / NUMPIXELS) + j) & 255));
        if(mode_flag!=3){return;}                    //the mode has been changed - get outta here!
      }
      strip.show();

      for(k=0; k < wait_num; k++){                         // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
      delay(wait);
      loop();                    
      }
  }}return;
 }


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    loop();
    return strip.Color(brightness*(255 - WheelPos * 3)/255, 0, brightness*(WheelPos * 3)/255);  //scale the output values by a factor of global 'brightness' so that the brightness remains as set
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    loop();
    return strip.Color(0,brightness*(WheelPos * 3)/255, brightness*(255 - WheelPos * 3)/255);
  }
  WheelPos -= 170;
  loop();
  return strip.Color(brightness*(WheelPos * 3)/255, brightness*(255 - WheelPos * 3)/255, 0);
}

// Set a LED color (not yet visible)
void setPixel(int Pixel, byte red, byte green, byte blue) {
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    loop();
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUMPIXELS; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
        if(mode_flag!=4){return;}                    //the mode has been changed - get outta here!

      }
      strip.show();
     
      delay(SpeedDelay);
     
      for (int i=0; i < NUMPIXELS; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
        if(mode_flag!=4){return;}                    //the mode has been changed - get outta here!
      }
    }
  }
}

void handle_mode4(){                                //Theater Chase mode
  mode_flag = 4;
  EEPROM.write(4,mode_flag);                        //write mode to EEProm so can be restored on start-up
  EEPROM.commit();
  server.send ( 200, "text/html","<SCRIPT language='JavaScript'>window.location='/';</SCRIPT>");
  int wait = 10;  //DON'T ever set this more than '10'. Use the 'k' value in the loop below to increase delays. This prevents the watchdog timer timing out on the ESP8266
  int wait_num = 10;
  while(mode_flag==4){                               // do this indefenitely or until mode changes
      theaterChase(100, 100, 100, 100);
      for(int k=0; k < wait_num; k++){                         // Do ten loops of the 'wait' and service loop routine inbetween. Total wait = 10 x 'wait'. This prevents sluggishness in the browser html front end menu.
        delay(wait);
        loop();                    
      }
  }return;
}


  


  

