#include <pgmspace.h>
#include "FastLED.h"
#include "SnakeGame.h"
#include "Constants.h"
#include <IRremote.h>
#include "Frames.h"
#include "Animation.h"
#include "AudioVisualizer.h"

#define LED_PIN 5
#define IR_REC_PIN 4
#define LED_TYPE    WS2812
#define BRIGHTNESS 32
#define NUM_ANIMS 29

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
const int PokemonFrameIndices[] = {0, 1, 0, 1, 0, 1, 0, 1, 2, 3, 2, 3, 2, 3, 2, 3, 4, 5, 4, 5, 4, 5, 4, 5, 6,7,6,7,6,7,6,7,
8,9,8,9,8,9,8,9,10,11,10,11,10,11,10,11,12,13,12,13,12,13,12,13,14,15,14,15,14,15,14,15,16,17,16,17,16,17,16,17,18,19,18,19,18,19,18,19,
20,21,20,21,20,21,20,21,22,23,22,23,22,23,22,23,24,25,24,25,24,25,24,25,26,27,26,27,26,27,26,27,28,29,28,29,28,29,28,29,30,31,30,31,30,31,30,31,
32,33,32,33,32,33,32,33,34,35,34,35,34,35,34,35,36,37,36,37,36,37,36,37,38,39,38,39,38,39,38,39,40,41,40,41,40,41,40,41};
const int PokeBallFrameIndices[] = {0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9};
const int MarioWalkingFrameIndices[] = {0, 1, 0, 2};
const int KirbyFrameIndices[] = {0, 1, 2, 1};
const int NemoFrameIndices[] = {0, 1, 2, 1};
const int PiranhaFrameIndices[] = {0, 1, 2, 3, 2, 1};
const int AdventureAttackFrameIndices[] = {0, 0, 0, 0, 0, 1, 2, 3, 4};
const int BabyYodaFrameIndices[] = {0, 0, 1, 1, 1, 1, 0, 0, 2, 2, 3, 4, 3, 2, 2};
const int PikaFrameIndices[] = {0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 20, 
20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, 25, 25};
const int JediFrameIndices[] = {0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,12,14,12,13,12,14,12,13,12,14,12,13,12,14};
const int JakeFrameIndices[] = {0,1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,0,1,2,3,4,5,6,7,8,9,10,11,12,13,13,13,14,14,14,13,13,13,14,14,14,13,13,13,14,14,14,13,13,13,14,14,14,13,13,13,14,14,14,15,16,17,18,19,20,
21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,22,23,23,23,23,23,24,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,26,27,28};
const int TurtleFrameIndices[] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,8,9,10,11,12,13,14,15,8,9,10,11,12,13,14,15,8,9,10,11,12,13,14,15,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23,16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31,24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,40,41,42,43,44,45,46,47,40,41,42,43,44,45,46,47,40,41,42,43,44,45,46,47,40,41,42,43,44,45,46,47};

// Others
SnakeGame snakeGame;
eState activeState = State_Anim;
int lastInput = -1;
bool cycling = true;
bool paused = false;
AudioVisualizer audioVisualizer(leds);

void setupAnimations();
void handleIRInput();

IRrecv irRecv(IR_REC_PIN);
decode_results results;

void setup() 
{
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  irRecv.enableIRIn();
  setupAnimations();
}

void loop() 
{  
  handleIRInput();

  FastLED.clear();

  if(paused)
  {
    delay(100);
  }
  else
  {
    switch(activeState)
    {
      case State_Anim:
        animations[currentAnim].updateAnim(leds);
        break;
      case State_AudioVisualizer:
        audioVisualizer.updateVisualizer();
        break;
      case State_Snake:
        snakeGame.updateSnake(lastInput);
        snakeGame.draw(leds);
    }
    
    EVERY_N_SECONDS(10) 
    {
      if (cycling && activeState == State_Anim) 
      {
        currentAnim = (currentAnim + 1) % NUM_ANIMS;
      }
    }
  }
}

void handleIRInput()
{
  int anim = currentAnim;
  
  if(irRecv.decode(&results))
  {
    if(results.value != 0xFFFFFFFF)
    {
      if(results.value == INPUT_PAUSE)
      {
        paused = !paused;
      }      

      if(paused)
      {
        irRecv.resume();
        return;
      }

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
          if(activeState == State_AudioVisualizer)
          {
            audioVisualizer.changeWidth();
          }
        
          activeState = State_AudioVisualizer;
          break;
        case INPUT_OK:
          activeState = State_Snake;
          break;
        case INPUT_RIGHT:
          if(activeState == State_Anim)
          {
            currentAnim = (currentAnim + 1) % NUM_ANIMS;
          }
          else if(activeState == State_AudioVisualizer)
          {
            audioVisualizer.incrementMode();
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
          else if(activeState == State_AudioVisualizer)
          {
            audioVisualizer.decrementMode();
          }
          break;
          case INPUT_UP:
            if(activeState == State_Anim && !cycling)
            {
              cycling = true;
            }
            else if(activeState == State_AudioVisualizer)
            {
              audioVisualizer.setAutoMode(true);
            }
            break;
           case INPUT_DOWN:
            cycling = false;

            if(activeState == State_AudioVisualizer)
            {
              audioVisualizer.setAutoMode(false);
            }
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

void setupAnimations()
{
  int i = 0;
  animations[i++] = Animation(sizeof(MegaManRunningFrameIndices) / sizeof(*MegaManRunningFrameIndices), 2, 100, MegaManRunningFrames, MegaManRunningFrameIndices);
  animations[i++] = Animation(sizeof(WilyFrames) / sizeof(*WilyFrames) / NUM_LEDS, 1, 100, WilyFrames);
  animations[i++] = Animation(sizeof(MegaManHeadFrames) / sizeof(*MegaManHeadFrames) / NUM_LEDS, 1, 100, MegaManHeadFrames);
  animations[i++] = Animation(sizeof(HeartContainerFrames) / sizeof(*HeartContainerFrames) / NUM_LEDS, 2, 100, HeartContainerFrames);
  animations[i++] = Animation(sizeof(TMNTFrames) / sizeof(*TMNTFrames) / NUM_LEDS, 10, 100, TMNTFrames);
  animations[i++] = Animation(sizeof(TurtleFrameIndices) / sizeof(*TurtleFrameIndices), 1, 100, TurtleFrames, TurtleFrameIndices);
  animations[i++] = Animation(sizeof(PokemonFrameIndices) / sizeof(*PokemonFrameIndices), 5, 100, PokemonFrames, PokemonFrameIndices);
  animations[i++] = Animation(sizeof(PokeBallFrameIndices) / sizeof(*PokeBallFrameIndices), 1, 100, PokeBallFrames, PokeBallFrameIndices);
  animations[i++] = Animation(sizeof(PikaFrameIndices) / sizeof(*PikaFrameIndices), 1, 50, PikaFrames, PikaFrameIndices);
  animations[i++] = Animation(sizeof(MarioWalkingFrameIndices) / sizeof(*MarioWalkingFrameIndices), 1, 100, MarioWalkingFrames, MarioWalkingFrameIndices);
  animations[i++] = Animation(sizeof(PiranhaFrameIndices) / sizeof(*PiranhaFrameIndices), 1, 100, PiranhaFrames, PiranhaFrameIndices);
  animations[i++] = Animation(sizeof(ShellFrames) / sizeof(*ShellFrames) / NUM_LEDS, 1, 100, ShellFrames);
  animations[i++] = Animation(sizeof(ItemsFrames) / sizeof(*ItemsFrames) / NUM_LEDS, 1, 100, ItemsFrames);
  animations[i++] = Animation(sizeof(KirbyFrameIndices) / sizeof(*KirbyFrameIndices), 2, 100, KirbyFrames, KirbyFrameIndices);
  animations[i++] = Animation(sizeof(KirbyWalkingFrames) / sizeof(*KirbyWalkingFrames) / NUM_LEDS, 1, 100, KirbyWalkingFrames);
  animations[i++] = Animation(sizeof(LinkNesFrames) / sizeof(*LinkNesFrames) / NUM_LEDS, 3, 100, LinkNesFrames);
  animations[i++] = Animation(sizeof(LinkPastFrames) / sizeof(*LinkPastFrames) / NUM_LEDS, 1, 100, LinkPastFrames);
  animations[i++] = Animation(sizeof(LinkWalkingFrames) / sizeof(*LinkWalkingFrames) / NUM_LEDS, 1, 100, LinkWalkingFrames);
  animations[i++] = Animation(sizeof(BubbleFrames) / sizeof(*BubbleFrames) / NUM_LEDS, 5, 100, BubbleFrames);
  animations[i++] = Animation(sizeof(NemoFrameIndices) / sizeof(*NemoFrameIndices), 2, 100, NemoFrames, NemoFrameIndices);
  animations[i++] = Animation(sizeof(FireFrames) / sizeof(*FireFrames) / NUM_LEDS, 1, 100, FireFrames);
  animations[i++] = Animation(sizeof(WizardFrames) / sizeof(*WizardFrames) / NUM_LEDS, 2, 100, WizardFrames);
  animations[i++] = Animation(sizeof(AdventureIdleFrames) / sizeof(*AdventureIdleFrames) / NUM_LEDS, 2, 100, AdventureIdleFrames);
  animations[i++] = Animation(sizeof(AdventureAttackFrameIndices) / sizeof(*AdventureAttackFrameIndices), 1, 100, AdventureAttackFrames, AdventureAttackFrameIndices);
  animations[i++] = Animation(sizeof(JakeFrameIndices) / sizeof(*JakeFrameIndices), 1, 60, JakeFrames, JakeFrameIndices);
  animations[i++] = Animation(sizeof(DinoFrames) / sizeof(*DinoFrames) / NUM_LEDS, 2, 100, DinoFrames);
  animations[i++] = Animation(sizeof(BabyYodaFrameIndices) / sizeof(*BabyYodaFrameIndices), 1, 100, BabyYodaFrames, BabyYodaFrameIndices);
  animations[i++] = Animation(sizeof(YodaFrames) / sizeof(*YodaFrames) / NUM_LEDS, 1, 60, YodaFrames);
  animations[i++] = Animation(sizeof(JediFrameIndices) / sizeof(*JediFrameIndices), 1, 100, JediFrames, JediFrameIndices);
}
