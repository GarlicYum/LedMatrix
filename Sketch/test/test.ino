#include "FastLED.h"

#define NUM_LEDS 128
#define DATA_PIN 5
#define LED_TYPE    WS2812
#define LEDS_PER_UNIT 64
#define UNIT_DIM 8
#define ROW_SIZE 16
#define UNITS_PER_ROW 2

CRGB leds[NUM_LEDS];
int index = 0;

int convertIndex(int i);

void setup() 
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(15);
}

void loop() 
{
  FastLED.clear();

  leds[convertIndex(index)] = CRGB::Green;
  index = (index + 1) % NUM_LEDS;

  FastLED.show();

  FastLED.delay(100);
}

int convertIndex(int i)
{
  return i % UNIT_DIM + ( i / UNIT_DIM ) * LEDS_PER_UNIT - ( i / ROW_SIZE ) * (UNITS_PER_ROW * LEDS_PER_UNIT) + UNIT_DIM * (i / ROW_SIZE);
}
