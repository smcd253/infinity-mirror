/*
Fixtures
Name: Table       | IP ADDR: 192.168.1.101 | UNIVERSE(s): 1, 2 | NUM LEDS: 174 | DIMENSIONS: 34, 53, 34, 53
Name: HyperCube   | IP ADDR: 192.168.1.102 | UNIVERSE(s): 3, 4 | NUM LEDS: 184 | DIMENSIONS: 17x12 (edges of cube)
Name: Mirrors1_2  | IP ADDR: 192.168.1.103 | UNIVERSE(s): 5    | NUM LEDS: 137 | DIMENSIONS: 17, 17, 17, 17, 15, 18, 17, 2 
Name: Mirror3     | IP ADDR: 192.168.1.104 | UNIVERSE(s): 6    | NUM LEDS: 69  | DIMENSIONS: 17, 17, 17, 17, 15, 18, 17, 2 
Name: Mirrors4_5  | IP ADDR: 192.168.1.105 | UNIVERSE(s): 7    | NUM LEDS: 136 | DIMENSIONS: 17, 17, 17, 17, 15, 17, 17, 2 
*/
#include <ArtnetWifi.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define DEVICE_NAME "HyperCube"
IPAddress ip(192,168,1,101);
const int num_universes = 2;
const int universes[num_universes] = {3, 4};
const int numLeds = 204;
const int leds_per_universe[num_universes] = {153, 51};
const int num_pins = 3;
const byte dataPins[num_pins] = {15, 13, 12};
const int start_pixel[num_pins] = {0, 68, 0};
const int pins_per_universe[num_universes] = {2, 1};
const int pixels_per_pin[num_pins] = {68, 85, 51};  

//Wifi settings
const char* ssid = "infinity-mirror";
const char* password = "weedistight";
/*static ip*/
IPAddress gateway(192,168,1,1);   
IPAddress subnet(255,255,255,0); 

// Neopixel settings
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(pixels_per_pin[0], dataPins[0], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(pixels_per_pin[1], dataPins[1], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(pixels_per_pin[2], dataPins[2], NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel strips[num_pins] = {strip1, strip2, strip3};
// Artnet settings
ArtnetWifi artnet;

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.hostname(DEVICE_NAME);      // DHCP Hostname (useful for finding device for static lease)
  WiFi.config(ip, subnet, gateway);

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void initTest()
{
  int total_leds = 0;
  for (int i = 0; i < sizeof(leds_per_universe)/sizeof(leds_per_universe[0]); i++) {
    total_leds += leds_per_universe[i];
  }

  if(total_leds != numLeds) {
    Serial.println("LEDs by Universe do Not Add Up to Total # LEDs!");
  }

  // for (int j = 0; j < num_pins; j++) {
    for (int i = 0 ; i < pixels_per_pin[0] ; i++) 
      strip1.setPixelColor(i, 127, 0, 0);
    strip1.show();
    delay(500);
    for (int i = 0 ; i < pixels_per_pin[0] ; i++)
      strip1.setPixelColor(i, 0, 127, 0);
    strip1.show();
    delay(500);
    for (int i = 0 ; i < pixels_per_pin[0] ; i++)
      strip1.setPixelColor(i, 0, 0, 127);
    strip1.show();
    delay(500);
    for (int i = 0 ; i < pixels_per_pin[0] ; i++)
      strip1.setPixelColor(i, 0, 0, 0);
    strip1.show();
  // }
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  // Serial.printf("Universe = %u | Length = %u | Sequence = %u | Data[0] = %u\n", 
  //               universe, length, sequence, data[0]);
  if (universe == 3) {
    int k = 0;
    // TODO: clean this up
    for (int j = 0; j < pixels_per_pin[0]; j++) {
      int l = k * 3;
      strip1.setPixelColor(j, data[l], data[l + 1], data[l + 2]);
      k++;      
      ESP.wdtFeed();
    }
    strip1.show();
    for (int j = 0; j < pixels_per_pin[1]; j++) {
      int l = k * 3;
      strip2.setPixelColor(j, data[l], data[l + 1], data[l + 2]);
      k++;      
      ESP.wdtFeed();
    }
    strip2.show();
  }
  else if (universe == 4) {
    int k = 0;
    for (int j = 0; j < pixels_per_pin[1]; j++) {
      int l = k * 3;
      strip3.setPixelColor(j, data[l], data[l + 1], data[l + 2]);
      k++;      
      ESP.wdtFeed();
    }
    strip3.show();
  }
}

void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  artnet.begin();
  
  strip1.begin();
  strip1.setBrightness(50);
  strip1.show(); // Initialize all pixels to 'off'

  strip2.begin();
  strip2.setBrightness(50);
  strip2.show(); // Initialize all pixels to 'off'

  strip3.begin();
  strip3.setBrightness(50);
  strip3.show(); // Initialize all pixels to 'off'

  initTest();
  ESP.wdtFeed();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
  ESP.wdtFeed();
}