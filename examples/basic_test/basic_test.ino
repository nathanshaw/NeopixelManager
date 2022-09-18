#include <NeopixelManager.h>
#include <WS2812Serial.h>

////////////////////////// LED MAPPING MODES ////////////////
#define LED_MAPPING_STANDARD      0
#define LED_MAPPING_BOTTOM_UP     1
#define LED_MAPPING_ROUND         2
#define LED_MAPPING_INSIDE_OUT    3

S2812Serial leds(NUM_LED, LED_DISPLAY_MEMORY, LED_DRAWING_MEMORY, LED_PIN, WS2812_GRB);
NeoGroup neos = NeoGroup(&leds, 0, NUM_LED - 1, "All Neos", MIN_FLASH_TIME, MAX_FLASH_TIME);

void setup() {
  Serial.begin(57600);
  delay(5000);
  neos.changeMapping(LED_MAPPING_STANDARD;
}

void loop() {
  for (int i = 0; i < 255; i++) {
    neos.colorWipe(i, 255-i, i);
  } 
}
