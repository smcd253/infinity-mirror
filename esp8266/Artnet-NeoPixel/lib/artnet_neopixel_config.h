/********************************************
 *                                          *
 *        :)     USER CONFIG      (:        *
 *                                          *
 ********************************************/
/*** Uncoment the fixture you would like to program ***/
// #define SIDE_PANEL_LEFT
// #define COL1
// #define COL2
// #define COL3
// #define COL4
#define COL5
// #define SIDE_PANEL_RIGHT
// #define HYPERCUBE
// #define TABLE
// #define LED_MATRIX1
// #define LED_MATRIX2

// WIFI INFORMATION
#define USE_DEFAULT_ADDRESSES       // comment out if you would like to manually input addresses
#define SSID        "infinity-mirror"
#define PASSWORD    "weedistight"
#define SUBNET      192, 168, 1
#ifndef USE_DEFAULT_ADDRESSES
#define ADDR        103
#endif

/********************************************
 *                                          *
 *       !!!    DEVELOPER ONLY    !!!       *
 *                                          *
 ********************************************/
// #define DEBUG
#ifdef TABLE
#define NUM_LEDS_UNIVERSE1 138
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 138
#endif
#ifdef HYPERCUBE
#endif
#ifdef COL1
#define DEVICE_NAME "Col1"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 103
#endif
#define NUM_LEDS_UNIVERSE1 136
#define NUM_LEDS_UNIVERSE2 68
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 136
#endif
#ifdef COL2
#define DEVICE_NAME "Col2"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 104
#endif
#define NUM_LEDS_UNIVERSE1 138
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 138
#endif
#ifdef COL3
#define DEVICE_NAME "Col3"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 105
#endif
#define NUM_LEDS_UNIVERSE1 138
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 138
#endif
#ifdef COL4
#define DEVICE_NAME "Col4"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 106
#endif
#define NUM_LEDS_UNIVERSE1 138
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 138
#endif
#ifdef COL5
#define DEVICE_NAME "Col5"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 107
#endif
#define NUM_LEDS_UNIVERSE1 138
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 138
#endif
#ifdef SIDE_PANEL_LEFT
#define DEVICE_NAME "Side Panel - Left"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 111
#endif
#define NUM_LEDS_UNIVERSE1 139
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 139
#endif
#ifdef SIDE_PANEL_RIGHT
#define DEVICE_NAME "Side Panel - Right"
#ifdef USE_DEFAULT_ADDRESSES
#define ADDR 109
#endif
#define NUM_LEDS_UNIVERSE1 139
#define NUM_LEDS_UNIVERSE2 69
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 139
#endif
#ifdef LED_MATRIX1
#define NUM_LEDS_UNIVERSE1 128
#define NUM_LEDS_UNIVERSE2 128
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 128
#endif
#ifdef LED_MATRIX2
#define NUM_LEDS_UNIVERSE1 128
#define NUM_LEDS_UNIVERSE2 128
#define START_PIXEL_UNIVERSE1 0
#define START_PIXEL_UNIVERSE2 128
#endif

// aggregate options
#define IP ip(SUBNET, ADDR)
#define GATEWAY gateway(SUBNET, 1)

#define DATA_PIN 3
#define NUM_UNIVERSES 2
#define UNIVERSES_TO_ARRAY {1, 2}
#define NUM_TOTAL_LEDS (NUM_LEDS_UNIVERSE1 + NUM_LEDS_UNIVERSE2)
#define NUM_LEDS_PER_UNIVERSE_TO_ARRAY {NUM_LEDS_UNIVERSE1, NUM_LEDS_UNIVERSE2}
#define START_PIXEL_TO_ARRAY {START_PIXEL_UNIVERSE1, START_PIXEL_UNIVERSE2}