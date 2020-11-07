/*
Fixtures
Name: Table       | IP ADDR: 192.168.1.101 | UNIVERSE(s): 1, 2 | NUM LEDS: 174 | DIMENSIONS: 34, 53, 34, 53
Name: HyperCube   | IP ADDR: 192.168.1.102 | UNIVERSE(s): 3, 4 | NUM LEDS: 184 | DIMENSIONS: 17x12 (edges of cube)
Name: Mirrors1_2  | IP ADDR: 192.168.1.103 | UNIVERSE(s): 5    | NUM LEDS: 137 | DIMENSIONS: 17, 17, 17, 17, 15, 18, 17, 2 
Name: Mirror3     | IP ADDR: 192.168.1.104 | UNIVERSE(s): 6    | NUM LEDS: 69  | DIMENSIONS: 17, 17, 17, 17, 15, 18, 17, 2 
Name: Mirrors4_5  | IP ADDR: 192.168.1.105 | UNIVERSE(s): 7    | NUM LEDS: 136 | DIMENSIONS: 17, 17, 17, 17, 15, 17, 17, 2 
Name: Mirrors6_9  | IP ADDR: 192.168.1.106 | UNIVERSE(s): 8, 9 | NUM LEDS: 206 | DIMENSIONS: 17, 17, 18, 17, 17, 17, 18, 17, 17, 17, 17, 18 
*/
#include <ArtnetWifi.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "../lib/artnet_neopixel_config.h"

const int universes[NUM_UNIVERSES] =          UNIVERSES_TO_ARRAY;
const int leds_per_universe[NUM_UNIVERSES] =  NUM_LEDS_PER_UNIVERSE_TO_ARRAY;
const int start_pixel[NUM_UNIVERSES] =        START_PIXEL_TO_ARRAY;   
const int num_leds =                          NUM_TOTAL_LEDS;
const int data_pin =                          DATA_PIN;

//Wifi settings
const char* ssid = SSID;
const char* password = PASSWORD;
/*static ip*/
IPAddress IP;
IPAddress GATEWAY;   
IPAddress subnet(255,255,255,0); 


// Neopixel settings
Adafruit_NeoPixel leds = Adafruit_NeoPixel(num_leds, data_pin, NEO_GRB + NEO_KHZ800);

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
  // Serial.println("init leds");
  int total_leds = 0;
  for (int i = 0; i < sizeof(leds_per_universe)/sizeof(leds_per_universe[0]); i++) {
    total_leds += leds_per_universe[i];
  }

  if(total_leds != num_leds) {
    Serial.println("LEDs by Universe do Not Add Up to Total # LEDs!");
  }

  for (int i = 0 ; i < num_leds ; i++)
    leds.setPixelColor(i, 127, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < num_leds ; i++)
    leds.setPixelColor(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < num_leds ; i++)
    leds.setPixelColor(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0 ; i < num_leds ; i++)
    leds.setPixelColor(i, 0, 0, 0);
  leds.show();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
#ifdef DEBUG
  Serial.printf("Universe = %u | Length = %u | Sequence = %u | Data[0] = %u\n", 
                universe, length, sequence, data[0]);
#endif
  for (int i = 0; i < NUM_UNIVERSES; i++) {
    if (universe == universes[i]) {
      int k = 0;
      for (int j = start_pixel[i]; j < start_pixel[i] + leds_per_universe[i]; j++) {
        int l = k * 3;
        leds.setPixelColor(j, data[l], data[l + 1], data[l + 2]);
        k++;
        ESP.wdtFeed();      
      }
      leds.show();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  artnet.begin();
  leds.begin();
  leds.setBrightness(50);
  leds.show();

  // ESP.disable();
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