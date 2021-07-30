#include <avr/pgmspace.h>
#include "FastLED.h"
#include "SnakeGame.h"
#include "Constants.h"
#include <IRremote.h>
#include "Frames.h"
#include "Animation.h"

#define DATA_PIN 5
#define IR_REC_PIN 6
#define LED_TYPE    WS2812
#define MAX_BRIGHTNESS 64
#define MIN_BRIGHTNESS 4
#define CYCLE_TICKS 120
#define DELAY_TIME 100
#define NUM_ANIMS 16

CRGB leds[NUM_LEDS];

enum eState
{
  State_Anim,
  State_AudioVisualizer,
  State_Snake,
};

// Animation variables
int currentAnim = 0;
Animation animations[NUM_ANIMS];
const int MegaManRunningFrameIndices[] = {0, 1, 2, 1};
const int PokemonFrameIndices[] = {0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5};
const int PokeBallFrameIndices[] = {0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9};
const int MarioWalkingFrameIndices[] = {0, 1, 0, 2};
const int KirbyFrameIndices[] = {0, 1, 2, 1};
const int NemoFrameIndices[] = {0, 1, 2, 1};
const int PiranhaFrameIndices[] = {0, 1, 2, 3, 2, 1};

// Others
SnakeGame snakeGame;
eState activeState = State_Anim;
int brightness = 16;
int lastInput = -1;
bool cycling = false;
int cycleCounter = 0;

void setupAnimations();
void handleIRInput();
void updateBrightness();
void updateCycling();

IRrecv irRecv(IR_REC_PIN);
decode_results results;

void setup() 
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  irRecv.enableIRIn();
  setupAnimations();
}

void loop() 
{
  handleIRInput();

  FastLED.clear();

  switch(activeState)
  {
    case State_Anim:
      animations[currentAnim].updateAnim(leds);
      break;
    case State_AudioVisualizer:
      break;
    case State_Snake:
      snakeGame.updateSnake(lastInput);
      snakeGame.draw(leds);
  }

  updateCycling();
  FastLED.delay(DELAY_TIME);
}

void handleIRInput()
{
  int anim = currentAnim;
  
  if(irRecv.decode(&results))
  {
    if(results.value != 0xFFFFFFFF)
    {
      switch(results.value)
      {
        case INPUT_0:
          activeState = State_Anim;
          currentAnim = 0;
          break;
        case INPUT_1:
          activeState = State_Anim;
          currentAnim = 1;
          break;
        case INPUT_2:
          activeState = State_Anim;
          currentAnim = 2;
          break;
        case INPUT_3:
          activeState = State_Anim;
          currentAnim = 3;
          break;
        case INPUT_4:
          activeState = State_Anim;
          currentAnim = 4;
          break;
        case INPUT_5:
          activeState = State_Anim;
          currentAnim = 5;
          break;
        case INPUT_6:
          activeState = State_Anim;
          currentAnim = 6;
          break;
        case INPUT_7:
          activeState = State_Anim;
          currentAnim = 7;
          break;
        case INPUT_8:
          activeState = State_Anim;
          currentAnim = 8;
          break;
        case INPUT_9:
          activeState = State_Anim;
          currentAnim = 9;
          break;
        case INPUT_AUDIO:
          activeState = State_AudioVisualizer;
          break;
        case INPUT_BRIGHTNESS:
          updateBrightness();
          break;
        case INPUT_OK:
          activeState = State_Snake;
          break;
        case INPUT_RIGHT:
          if(activeState == State_Anim)
          {
            currentAnim = (currentAnim + 1) % NUM_ANIMS;
          }
          break;
        case INPUT_LEFT:
          if(activeState == State_Anim)
          {
            currentAnim--;
            if(currentAnim < 0)
            {
              currentAnim = NUM_ANIMS - 1;
            }
          }
          break;
          case INPUT_UP:
            if(activeState == State_Anim && !cycling)
            {
              cycling = true;
              cycleCounter = 0;
            }
            break;
           case INPUT_DOWN:
            cycling = false;
            break;
      }

      lastInput = results.value;
    }
    
    irRecv.resume();

    if(currentAnim != anim)
    {
      cycling = false;
    }
  }
}

void updateCycling()
{
  if(cycling && ++cycleCounter >= CYCLE_TICKS && activeState == State_Anim)
  {
    cycleCounter = 0;
    currentAnim = (currentAnim + 1) % NUM_ANIMS;
  }
}

void updateBrightness()
{
  brightness *= 2;
  brightness = brightness > MAX_BRIGHTNESS ? MIN_BRIGHTNESS : brightness;
  FastLED.setBrightness(brightness);
}

void setupAnimations()
{
  animations[0] = Animation(4, 2, 100, MegaManRunningFrames, MegaManRunningFrameIndices);
  animations[1] = Animation(2, 1, 100, MegaManHeadFrames);
  animations[2] = Animation(4, 2, 100, HeartContainerFrames);
  animations[3] = Animation(4, 10, 100, TMNTFrames);
  animations[4] = Animation(6, 5, 100, PokemonFrames, PokemonFrameIndices);
  animations[5] = Animation(16, 1, 100, PokeBallFrames, PokeBallFrameIndices);
  animations[6] = Animation(4, 1, 100, MarioWalkingFrames, MarioWalkingFrameIndices);
  animations[7] = Animation(4, 2, 100, KirbyFrames, KirbyFrameIndices);
  animations[8] = Animation(10, 1, 100, LinkWalkingFrames);
  animations[9] = Animation(2, 5, 100, BubbleFrames);
  animations[10] = Animation(4, 2, 100, NemoFrames, NemoFrameIndices);
  animations[11] = Animation(6, 1, 100, PiranhaFrames, PiranhaFrameIndices);
  animations[12] = Animation(10, 1, 100, FireFrames);
  animations[13] = Animation(6, 1, 100, ShellFrames);
  animations[14] = Animation(4, 1, 100, ItemsFrames);
  animations[15] = Animation(6, 1, 100, WilyFrames);
}
