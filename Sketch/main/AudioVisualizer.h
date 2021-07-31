#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#include "FastLED.h"
#include "Constants.h"
#include <arduinoFFT.h>

#define SAMPLES 1024          // Must be a power of 2

class AudioVisualizer
{
  public:
    AudioVisualizer(CRGB* leds);
  
    enum eModes
    {
      Mode_Standard,
      Mode_Rainbow,
      Mode_Purple,
      Mode_Changing,
      Mode_Center,
      Mode_WhitePeak,
      Mode_OutrunPeak,
      Num_Modes
    };

    void updateVisualizer();
    void switchMode();
    void setAutoMode(bool b);
  
  private:
    void getSamples();
    void computeFFT();
    void analyzeResults();
    void draw();
    void resetBands();
    void decayPeak();
    void incrementColorTimer();
    void cycleModes();

    void standardBars(int band, int barHeight);
    void rainbowBars(int band, int barHeight);
    void purpleBars(int band, int barHeight);
    void changingBars(int band, int barHeight);
    void centerBars(int band, int barHeight);
    void whitePeak(int band);
    void outrunPeak(int band);
  
    eModes _mode = Mode_Standard;

    CRGB* leds;
    unsigned int sampling_period_us;
    byte peak[ROW_SIZE] = {0};
    int oldBarHeights[ROW_SIZE] = {0};
    int bandValues[ROW_SIZE] = {0};
    double vReal[SAMPLES];
    double vImag[SAMPLES];
    unsigned long newTime;
    arduinoFFT FFT;
    bool autoChangePatterns = false;
    uint8_t colorTimer = 0;
};

#endif
