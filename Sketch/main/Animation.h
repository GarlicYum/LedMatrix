#ifndef ANIMATION_H
#define ANIMATION_H

#include "FastLED.h"

class Animation
{
  public:
    Animation();
    Animation(int frameCount, int tickCount, int delayTime, const long* frames, const int* frameIndices = NULL, float colorBrightness = 1.0f);
    
    void updateAnim(CRGB* leds);
  
  private:
    int _frameCount;
    int _tickCount;
    int _delayTime;
    const long* _frames;
    const int* _frameIndices;
    int _currentTickCount;
    int _currentFrameCount;
    float _colorBrightness;
};

#endif
