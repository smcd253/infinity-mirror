#include <ESP8266WiFi.h>
#include <E131.h>
#include <NeoPixelBus.h>

#define NUM_PIXELS 69  /* Number of pixels */
#define UNIVERSE 4      /* Universe to listen for */
#define LEDS_PER_UNIVERSE 150
#define CHANNEL_START 1 /* Channel to start listening at */
#define DATA_PIN 3      /* Pixel output - GPIO0 */

const char ssid[] = "Top Shelf";         /* Replace with your SSID */
const char passphrase[] = "purpskurp69";   /* Replace with your WPA2 passphrase */
/*static ip*/
IPAddress ip(192,168,1,39);   
IPAddress gateway(192,168,1,1);   
IPAddress subnet(255,255,255,0); 
IPAddress dns(192,168,1,1);

E131 e131;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> ledstrip(NUM_PIXELS, DATA_PIN);

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Choose one to begin listening for E1.31 data */
    e131.begin(ssid, passphrase, ip, subnet, gateway, dns);                       /* via Unicast on the default port */

    //e131.beginMulticast(ssid, passphrase, UNIVERSE);  /* via Multicast for Universe 1 */

    /* Initialize output */
    ledstrip.Begin();
    ledstrip.Show();
}

void loop() {
    /* Parse a packet */
    uint16_t num_channels = e131.parsePacket();
    if(num_channels) {
        Serial.printf("Universe %u / %u Channels | Packet#: %u / Errors: %u / CH1: %u\n",
                e131.universe,              // The Universe for this packet
                num_channels,               // Number of channels in this packet
                e131.stats.num_packets,     // Packet counter
                e131.stats.packet_errors,   // Packet error counter
                e131.data[0]);              // Dimmer data for Channel 1
        if (e131.universe == UNIVERSE) {
            for (int i = 0; i < LEDS_PER_UNIVERSE; i++) {
                int j = i * 3 + (CHANNEL_START - 1);
                RgbColor pixel((uint8_t)e131.data[j], (uint8_t)e131.data[j+1], (uint8_t)e131.data[j+2]);
                ledstrip.SetPixelColor(i, pixel);
            }
            ledstrip.Show();
        }
    }
}
