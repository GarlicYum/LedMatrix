#include "Animation.h"
#include "Helpers.h"
#include "Constants.h"

#define COLOR_BRIGHTNESS 1.0

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
      unsigned int color = pgm_read_dword(&(_frames[_frameIndices[_currentFrameCount] * NUM_LEDS + i]));
      byte r = (byte)((double)((color & 0xFF000000) >> 24) * COLOR_BRIGHTNESS);
      byte g = (byte)((double)((color & 0xFF000000) >> 16) * COLOR_BRIGHTNESS);
      byte b = (byte)((double)((color & 0xFF000000) >> 8) * COLOR_BRIGHTNESS);
      byte a = (byte)((double)((color & 0xFF000000) >> 0) * COLOR_BRIGHTNESS);

      color = r << 24 + g << 16 + b << 8 + a;

      leds[Helpers::convertIndex(i)] = color;
    }
  }
  else
  {
    for(int i = 0; i < NUM_LEDS; i++)
    {
      unsigned int color = pgm_read_dword(&(_frames[_currentFrameCount * NUM_LEDS + i]));
      byte r = (byte)((double)((color & 0xFF000000) >> 24) * COLOR_BRIGHTNESS);
      byte g = (byte)((double)((color & 0xFF000000) >> 16) * COLOR_BRIGHTNESS);
      byte b = (byte)((double)((color & 0xFF000000) >> 8) * COLOR_BRIGHTNESS);
      byte a = (byte)((double)((color & 0xFF000000) >> 0) * COLOR_BRIGHTNESS);

      color = r << 24 + g << 16 + b << 8 + a;
      
      leds[Helpers::convertIndex(i)] = color;
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
