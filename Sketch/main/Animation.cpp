#include "Animation.h"
#include "Helpers.h"
#include "Constants.h"

Animation::Animation()
{}

Animation::Animation(int frameCount, int tickCount, int delayTime, const long* frames, const int* frameIndices, float colorBrightness)
: _frameCount(frameCount)
, _tickCount(tickCount)
, _delayTime(delayTime)
, _currentTickCount(0)
, _currentFrameCount(0)
, _frames(frames)
, _frameIndices(frameIndices)
, _colorBrightness(colorBrightness)
{
}
    
void Animation::updateAnim(CRGB* leds)
{
  if(_frameIndices)
  {
    for(int i = 0; i < NUM_LEDS; i++)
    {
      unsigned int color = pgm_read_dword(&(_frames[_frameIndices[_currentFrameCount] * NUM_LEDS + i]));
      byte r = (byte)((float)((color & 0xFF000000) >> 24) * _colorBrightness);
      byte g = (byte)((float)((color & 0x00FF0000) >> 16) * _colorBrightness);
      byte b = (byte)((float)((color & 0x0000FF00) >> 8) * _colorBrightness);
      byte a = (byte)((float)((color & 0x000000FF) >> 0) * _colorBrightness);

      color = r << 24 + g << 16 + b << 8 + a;

      leds[Helpers::convertIndex(i)] = color;
    }
  }
  else
  {
    for(int i = 0; i < NUM_LEDS; i++)
    {
      unsigned int color = pgm_read_dword(&(_frames[_currentFrameCount * NUM_LEDS + i]));
      byte r = (byte)((float)((color & 0xFF000000) >> 24) * _colorBrightness);
      byte g = (byte)((float)((color & 0x00FF0000) >> 16) * _colorBrightness);
      byte b = (byte)((float)((color & 0x0000FF00) >> 8) * _colorBrightness);
      byte a = (byte)((float)((color & 0x000000FF) >> 0) * _colorBrightness);

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
