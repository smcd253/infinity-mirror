#include <ESP8266WiFi.h>
#include <ESPAsyncE131.h>
#include "FastLED.h"

#define DEVICE_NAME "Table"
#define U1_NUM_PIXELS 150  /* Number of pixels in Universe 1 */
#define U2_NUM_PIXELS 24 /* Number of pixels in Universe 2 */
#define TOT_PIXELS 174
#define UNIVERSE 5      /* Universe to listen for */
#define UNIVERSE_2 6   /* set to -1 if only 1 universe*/
#define UNIVERSE_COUNT 2
#define CHANNEL_START 1 /* Channel to start listening at */
#define DATA_PIN 3      /* Pixel output - GPIO3 */
// #define LED_TYPE    WS2812
// #define BRIGHTNESS  64 // Default LED brightness.
// #define COLOR_ORDER GRB
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER


CRGB leds[TOT_PIXELS];

const char ssid[] = "Top Shelf";         /* Replace with your SSID */
const char passphrase[] = "purpskurp69";   /* Replace with your WPA2 passphrase */
/*static ip*/
IPAddress ip(192,168,1,41);   
IPAddress gateway(192,168,1,1);   
IPAddress subnet(255,255,255,0); 
IPAddress dns(192,168,1,1);

// ESPAsyncE131 instance with UNIVERSE_COUNT buffer slots
ESPAsyncE131 e131(UNIVERSE_COUNT);

void setup() {
    Serial.begin(115200);
    delay(10);
    
    // Make sure you're in station mode    
    WiFi.mode(WIFI_STA);

    WiFi.hostname(DEVICE_NAME);      // DHCP Hostname (useful for finding device for static lease)
    WiFi.config(ip, subnet, gateway, dns);

    Serial.println("");
    Serial.print(F("Connecting to "));
    Serial.print(ssid);

    if (passphrase != NULL)
        WiFi.begin(ssid, passphrase);
    else
        WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print(F("Connected with IP: "));
    Serial.println(WiFi.localIP());
    
    // Choose one to begin listening for E1.31 data
    if (e131.begin(E131_UNICAST))                               // Listen via Unicast
        Serial.println(F("Listening for data..."));
    // if (e131.begin(E131_MULTICAST, UNIVERSE, UNIVERSE_COUNT))   // Listen via Multicast
    else 
        Serial.println(F("*** e131.begin failed ***"));

    /* Initialize output */
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, TOT_PIXELS);
}

void loop() {
    if (!e131.isEmpty()) {
        e131_packet_t packet;
        e131.pull(&packet);     // Pull packet from ring buffer
        
        Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
                htons(packet.universe),                 // The Universe for this packet
                htons(packet.property_value_count) - 1, // Start code is ignored, we're interested in dimmer data
                e131.stats.num_packets,                 // Packet counter
                e131.stats.packet_errors,               // Packet error counter
                packet.property_values[1]);             // Dimmer data for Channel 1

        if ((int)htons(packet.universe) == UNIVERSE) {
            Serial.printf("Universe 5\n");
            FastLED.clear();
            for (int i = 0; i < U1_NUM_PIXELS; i++) {
                int j = i * 3 + (CHANNEL_START - 1);
                leds[i] = CRGB((uint8_t)packet.property_values[j], (uint8_t)packet.property_values[j+1], (uint8_t)packet.property_values[j+2]);
            }
            FastLED.show();
        }
        else if ((int)htons(packet.universe) == UNIVERSE_2) {
            FastLED.clear();
            for (int i = U1_NUM_PIXELS; i < TOT_PIXELS; i++) {
                int j = i * 3 + (CHANNEL_START - 1);
                leds[i] = CRGB((uint8_t)packet.property_values[j], (uint8_t)packet.property_values[j+1], (uint8_t)packet.property_values[j+2]);
            }
            FastLED.show();
        }
    }
}