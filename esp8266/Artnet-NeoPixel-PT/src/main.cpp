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
#include <pt-sem.h>

#include "../lib/artnet_neopixel_config.h"

const int universes[NUM_UNIVERSES] =          UNIVERSES_TO_ARRAY;
const int leds_per_universe[NUM_UNIVERSES] =  NUM_LEDS_PER_UNIVERSE_TO_ARRAY;
const int start_pixel[NUM_UNIVERSES] =        START_PIXEL_TO_ARRAY;   
const int num_leds =                          NUM_TOTAL_LEDS;
const int data_pin =                          DATA_PIN;

//Wifi settings
const char* ssid = "Daddy Leroy";
const char* password = "leroyjenkins";
/*static ip*/
IPAddress IP;
IPAddress GATEWAY;   
IPAddress subnet(255,255,255,0); 

// Neopixel settings
Adafruit_NeoPixel leds = Adafruit_NeoPixel(num_leds, data_pin, NEO_GRB + NEO_KHZ800);

// Artnet settings
ArtnetWifi artnet;


// protothread
static struct pt driver_pt;
// static struct pt_sem mutex;
volatile bool write = false;
const size_t num_channels = (size_t)num_leds * 3;
volatile uint8_t leds_state[num_channels];

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
        if(!write){
          int k = 0;
#ifdef DEBUG
          Serial.printf("start_pixel[%d] = %u | end = %u\n", i, start_pixel[i] * 3, 3 * (start_pixel[i] + leds_per_universe[i]));
#endif         
          for (int j = 3 * start_pixel[i]; j < 3 * (start_pixel[i] + leds_per_universe[i]); j++) {
            // add to buffer
            leds_state[j] = data[k];
            k++;      
          }
          write = true;  
        }
      }
    }
}

PT_THREAD(thread_receive_artnet(struct pt *pt, int interval)) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp

    // do work
    // PT_SEM_WAIT(pt, &mutex);
    artnet.read();
    // PT_SEM_SIGNAL(pt, &mutex);
  }
  PT_END(pt);
}

void write_leds()
{
  #ifdef DEBUG
  Serial.printf("leds_state[0] = %u |\t", leds_state[0]);
  Serial.printf("led_state[139*3] = %u\n", leds_state[139*3]);
  #endif
    if(write) {
      for (int i = 0; i < num_leds; i++) {
        int l = i * 3;
        leds.setPixelColor(i, leds_state[l], leds_state[l + 1], leds_state[l + 2]);
      }
      leds.show();
      write = false;
    }
     
}

PT_THREAD(thread_write_leds(struct pt *pt, int interval)) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp

    // do work
    // PT_SEM_WAIT(pt, &mutex);
    write_leds();
    // PT_SEM_SIGNAL(pt, &mutex);
  }
  PT_END(pt);
}

PT_THREAD(driver_thread(struct pt *pt)) {
  static struct pt pt_producer, pt_consumer;
  PT_BEGIN(pt);
  PT_INIT(&pt_producer);
  PT_INIT(&pt_consumer);

  // PT_SEM_INIT(&mutex, 1);
  
  PT_WAIT_THREAD(pt, thread_receive_artnet(&pt_producer, 5) &
                      thread_write_leds(&pt_consumer, 10));

  PT_END(pt);
}

void setup()
{
  Serial.begin(115200);
  PT_INIT(&driver_pt);

  ConnectWifi();
  artnet.begin();

  leds.begin();
  leds.setBrightness(50);
  leds.show();

  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);

  // protothread
  for (int i = 0; i < num_channels; i++) {
    leds_state[i] = 0;
  }
}

void loop()
{
  driver_thread(&driver_pt);
}