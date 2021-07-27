#include <avr/pgmspace.h>
#include "FastLED.h"
#include "SnakeGame.h"
#include "Helpers.h"
#include "Constants.h"
#include <IRremote.h>
#include "Frames.h"

#define NUM_LEDS 576
#define DATA_PIN 5
#define IR_REC_PIN 6
#define LED_TYPE    WS2812
#define MAX_BRIGHTNESS 64
#define MIN_BRIGHTNESS 4
#define CYCLE_TICKS 120
#define DELAY_TIME 100

CRGB leds[NUM_LEDS];

enum eState
{
  State_Anim0,
  State_Anim1,
  State_Anim2,
  State_Anim3,
  State_Anim4,
  State_Anim5,
  State_Anim6,
  State_Anim7,
  State_Anim8,
  State_Anim9,
  State_Generated,
  State_Snake,
};

// Megaman Running
const int MegaManRunningFrameIndices[] = {0, 1, 2, 1};
const int MegaManRunningFrameCount = 4;
const int MegaManRunningTickCount = 2;

// Megaman Head
const int MegaManHeadFrameCount = 2;
const int MegaManHeadTickCount = 1;

// Heart Container
const int HeartContainerFrameCount = 4;
const int HeartContainerTickCount = 2;

// TMNT
const int TMNTFrameCount = 4;
const int TMNTTickCount = 30;

// Pokemon
const int PokemonFrameIndices[] = {0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5};
const int PokemonFrameCount = 6;
const int PokemonTickCount = 5;

// Mario walking
const int MarioWalkingFrameIndices[] = {0, 1, 0, 2};
const int MarioWalkingFrameCount = 4;
const int MarioWalkingTickCount = 1;

// Link Walking
const int LinkWalkingFrameCount = 10;
const int LinkWalkingTickCount = 1;

// Others
SnakeGame snakeGame;
unsigned int currentTickCount = 0;
int currentFrameCount = 0;
eState activeState = State_Anim0;
int brightness = 16;
int lastInput = -1;
bool cycling = false;
int cycleCounter = 0;

// Function declarations
void updateAnimation(const long frames[][NUM_LEDS], int tickCount, int frameCount, const int* frameIndices);
void updateAnimation(const long frames[][NUM_LEDS], int tickCount, int frameCount);

void updateFrame(int tickCount, int frameCount);
void handleIRInput();
void updateBrightness();
void reset();
void updateCycling();

IRrecv irRecv(IR_REC_PIN);
decode_results results;

void setup() 
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  irRecv.enableIRIn();
}

void loop() 
{
  handleIRInput();

  FastLED.clear();

  switch(activeState)
  {
    case State_Anim0:
      updateAnimation(MegaManRunningFrames, MegaManRunningTickCount, MegaManRunningFrameCount, MegaManRunningFrameIndices);
      break;
    case State_Anim1:
      updateAnimation(MegaManHeadFrames, MegaManHeadTickCount, MegaManHeadFrameCount);
      break;
    case State_Anim2:
      updateAnimation(HeartContainerFrames, HeartContainerTickCount, HeartContainerFrameCount);
      break;
    case State_Anim3:
      updateAnimation(TMNTFrames, TMNTTickCount, TMNTFrameCount);
      break;
    case State_Anim4:
      updateAnimation(PokemonFrames, PokemonTickCount, PokemonFrameCount, PokemonFrameIndices);
      break;
    case State_Anim5:
      updateAnimation(MarioWalkingFrames, MarioWalkingTickCount, MarioWalkingFrameCount, MarioWalkingFrameIndices);
      break;
    case State_Anim6:
      updateAnimation(LinkWalkingFrames, LinkWalkingTickCount, LinkWalkingFrameCount);
      break;
    case State_Anim7:
    case State_Anim8:
    case State_Anim9:
    case State_Snake:
      snakeGame.updateSnake(lastInput);
      snakeGame.draw(leds);
    case State_Generated:
      break;
  }

  updateCycling();
  currentTickCount++;
  FastLED.delay(DELAY_TIME);
}

void updateAnimation(const long frames[][NUM_LEDS], int tickCount, int frameCount, const int* frameIndices)
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = pgm_read_dword(&(frames[frameIndices[currentFrameCount]][Helpers::convertIndex(i)]));
  }

  updateFrame(tickCount, frameCount);
}

void updateAnimation(const long frames[][NUM_LEDS], int tickCount, int frameCount)
{
  for(int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = pgm_read_dword(&(frames[currentFrameCount][Helpers::convertIndex(i)]));
  }

  updateFrame(tickCount, frameCount);
}

void updateFrame(int tickCount, int frameCount)
{
  FastLED.show();

  if((currentTickCount % tickCount) == 0)
  {
    currentFrameCount = (currentFrameCount + 1) % frameCount;
  }
}

void handleIRInput()
{
  eState curState = activeState;
  
  if(irRecv.decode(&results))
  {
    if(results.value != 0xFFFFFFFF)
    {
      switch(results.value)
      {
        case INPUT_0:
          activeState = State_Anim0;
          break;
        case INPUT_1:
          activeState = State_Anim1;
          break;
        case INPUT_2:
          activeState = State_Anim2;
          break;
        case INPUT_3:
          activeState = State_Anim3;
          break;
        case INPUT_4:
          activeState = State_Anim4;
          break;
        case INPUT_5:
          activeState = State_Anim5;
          break;
        case INPUT_6:
          activeState = State_Anim6;
          break;
        case INPUT_7:
          activeState = State_Anim7;
          break;
        case INPUT_8:
          activeState = State_Anim8;
          break;
        case INPUT_9:
          activeState = State_Anim9;
          break;
        case INPUT_GENERATED:
          activeState = State_Generated;
          break;
        case INPUT_BRIGHTNESS:
          updateBrightness();
          break;
        case INPUT_OK:
          activeState = State_Snake;
          break;
        case INPUT_RIGHT:
          if(activeState <= State_Generated)
          {
            activeState = (eState)((activeState + 1) % 11);
          }
          break;
        case INPUT_LEFT:
          if(activeState <= State_Generated)
          {
            if(activeState == State_Anim0)
            {
              activeState = State_Generated;
            }
            else
            {
              activeState = (eState)(activeState - 1);
            }
          }
          break;
          case INPUT_UP:
            if(activeState <= State_Generated && !cycling)
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

    if(curState != activeState)
    {
      cycling = false;
      reset();
    }
  }
}

void updateCycling()
{
  if(cycling && ++cycleCounter >= CYCLE_TICKS)
  {
    cycleCounter = 0;
    activeState = (eState)((activeState + 1) % 11);
  }
}

void reset()
{
  currentFrameCount = 0;
  currentTickCount = 0;
}

void updateBrightness()
{
  brightness *= 2;
  brightness = brightness > MAX_BRIGHTNESS ? MIN_BRIGHTNESS : brightness;
  FastLED.setBrightness(brightness);
}
