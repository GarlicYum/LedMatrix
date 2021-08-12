#include <FastLED.h>

#define LED_PIN 5
#define LED_TYPE    WS2812
#define BRIGHTNESS 64
#define NUM_LEDS 576
#define LEDS_PER_UNIT 64
#define UNIT_DIM 8
#define ROW_SIZE 24
#define UNITS_PER_ROW 3

int curIndex = 0;

CRGB leds[NUM_LEDS];

int convertIndex(int i)
{
  return i % UNIT_DIM + ( i / UNIT_DIM ) * LEDS_PER_UNIT - ( i / ROW_SIZE ) * ( UNITS_PER_ROW * LEDS_PER_UNIT ) + ((UNIT_DIM * ( i / ROW_SIZE )) % LEDS_PER_UNIT) + (i / (LEDS_PER_UNIT * UNITS_PER_ROW)) * (LEDS_PER_UNIT * UNITS_PER_ROW);
}

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  FastLED.clear();
  leds[convertIndex(curIndex)] = CRGB(0, 0, 255);
  curIndex = (curIndex + 1) % NUM_LEDS;
  FastLED.show();
  FastLED.delay(100);
  // put your main code here, to run repeatedly:

}
