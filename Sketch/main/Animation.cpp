#include "Animation.h"
#include "Helpers.h"
#include "Constants.h"

Animation::Animation()
{}

Animation::Animation(int frameCount, int tickCount, int delayTime, const long* frames, const int* frameIndices)
: _frameCount(frameCount)
, _tickCount(tickCount)
, _delayTime(delayTime)
, _currentTickCount(0)
, _currentFrameCount(0)
, _frames(frames)
, _frameIndices(frameIndices)
{
}
    
void Animation::updateAnim(CRGB* leds)
{
  if(_frameIndices)
  {
    for(int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = pgm_read_dword(&(_frames[_frameIndices[_currentFrameCount] * NUM_LEDS + Helpers::convertIndex(i)]));
    }
  }
  else
  {
    for(int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = pgm_read_dword(&(_frames[_currentFrameCount * NUM_LEDS + Helpers::convertIndex(i)]));
    }
  }
   
  FastLED.show();

  if((_currentTickCount % _tickCount) == 0)
  {
    _currentFrameCount = (_currentFrameCount + 1) % _frameCount;
  }

  _currentTickCount = (_currentTickCount + 1) % _tickCount;

  FastLED.delay(_delayTime);
}
