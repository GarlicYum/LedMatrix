#include "AudioVisualizer.h"
#include "Constants.h"

#define SAMPLING_FREQ   40000         // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE       1000          // Depending on your audio source level, you may need to alter this value. Can be used as a 'sensitivity' control.
#define AUDIO_IN_PIN    35            // Signal in on this pin
#define NOISE           500           // Used as a crude noise filter, values below this are ignored
#define TOP (ROW_SIZE - 1)

DEFINE_GRADIENT_PALETTE( purple_gp ) {
  0,   0, 212, 255,   //blue
255, 179,   0, 255 }; //purple
DEFINE_GRADIENT_PALETTE( outrun_gp ) {
  0, 141,   0, 100,   //purple
127, 255, 192,   0,   //yellow
255,   0,   5, 255 };  //blue
DEFINE_GRADIENT_PALETTE( greenblue_gp ) {
  0,   0, 255,  60,   //green
 64,   0, 236, 255,   //cyan
128,   0,   5, 255,   //blue
192,   0, 236, 255,   //cyan
255,   0, 255,  60 }; //green
DEFINE_GRADIENT_PALETTE( redyellow_gp ) {
  0,   200, 200,  200,   //white
 64,   255, 218,    0,   //yellow
128,   231,   0,    0,   //red
192,   255, 218,    0,   //yellow
255,   200, 200,  200 }; //white
CRGBPalette16 purplePal = purple_gp;
CRGBPalette16 outrunPal = outrun_gp;
CRGBPalette16 greenbluePal = greenblue_gp;
CRGBPalette16 heatPal = redyellow_gp;

AudioVisualizer::AudioVisualizer(CRGB* leds)
: leds(leds)
, FFT(arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ))
, sampling_period_us(round(1000000 * (1.0 / SAMPLING_FREQ)))
{}

void AudioVisualizer::updateVisualizer()
{
  FastLED.clear();
  
  resetBands();
  getSamples();
  computeFFT();
  analyzeResults();
  draw();
  decayPeak();
  incrementColorTimer();
  cycleModes();

  FastLED.show();
}

void AudioVisualizer::getSamples()
{
  for (int i = 0; i < SAMPLES; i++) 
  {
    newTime = micros();
    vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
    vImag[i] = 0;
    while ((micros() - newTime) < sampling_period_us) { /* chill */ }
  }
}

void AudioVisualizer::computeFFT()
{
  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();
}

void AudioVisualizer::analyzeResults()
{
  // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
  for (int i = 2; i < (SAMPLES/2); i++)
  {      
    if (vReal[i] > NOISE) 
    {              
      //24 bands, 16kHz top band
      if      (i<=2)   bandValues[0]  += (int)vReal[i];
      else if (i<=3)   bandValues[1]  += (int)vReal[i];
      else if (i<=4)   bandValues[2]  += (int)vReal[i];
      else if (i<=5)   bandValues[3]  += (int)vReal[i];
      else if (i<=6)   bandValues[4]  += (int)vReal[i];
      else if (i<=7)   bandValues[5]  += (int)vReal[i];
      else if (i<=9)   bandValues[6]  += (int)vReal[i];
      else if (i<=12)  bandValues[7]  += (int)vReal[i];
      else if (i<=15)  bandValues[8]  += (int)vReal[i];
      else if (i<=18)  bandValues[9]  += (int)vReal[i];
      else if (i<=23)  bandValues[10] += (int)vReal[i];
      else if (i<=29)  bandValues[11] += (int)vReal[i];
      else if (i<=37)  bandValues[12] += (int)vReal[i];
      else if (i<=46)  bandValues[13] += (int)vReal[i];
      else if (i<=58)  bandValues[14] += (int)vReal[i];
      else if (i<=73)  bandValues[15] += (int)vReal[i];
      else if (i<=92)  bandValues[16] += (int)vReal[i];
      else if (i<=116) bandValues[17] += (int)vReal[i];
      else if (i<=146) bandValues[18] += (int)vReal[i];
      else if (i<=184) bandValues[19] += (int)vReal[i];
      else if (i<=232) bandValues[20] += (int)vReal[i];
      else if (i<=292) bandValues[21] += (int)vReal[i];
      else if (i<=367) bandValues[22] += (int)vReal[i];
      else             bandValues[23] += (int)vReal[i];
    }
  }
}

void AudioVisualizer::draw()
{
  // Process the FFT data into bar heights
  for (byte band = 0; band < ROW_SIZE; band++) 
  {
    // Scale the bars for the display
    int barHeight = bandValues[band] / AMPLITUDE;
    
    if (barHeight > TOP) 
    {
      barHeight = TOP;
    }

    // Small amount of averaging between frames
    barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

    // Move peak up
    if (barHeight > peak[band]) 
    {
      peak[band] = min(ROW_SIZE, barHeight);
    }

    switch (_mode) 
    {
      case Mode_Rainbow:
        rainbowBars(band, barHeight);
        whitePeak(band);
        break;
      case Mode_OutrunPeak:
        outrunPeak(band);
        break;
      case Mode_Purple:
        purpleBars(band, barHeight);
        whitePeak(band);
        break;
      case Mode_Center:
        centerBars(band, barHeight);
        break;
      case Mode_Changing:
        changingBars(band, barHeight);
        break;
      default:
        break;
    }
    oldBarHeights[band] = barHeight;
  }
}

void AudioVisualizer::switchMode()
{
  autoChangePatterns = false;
  _mode = (eModes)((_mode + 1) % Num_Modes);
}

void AudioVisualizer::setAutoMode(bool b)
{
  autoChangePatterns = b;
}

void AudioVisualizer::rainbowBars(int band, int barHeight) 
{
  for (int y = TOP; y >= TOP - barHeight; y--) 
  {
    leds[y * ROW_SIZE + band] = CHSV(band * (255 / ROW_SIZE), 255, 255);
  }
}

void AudioVisualizer::purpleBars(int band, int barHeight) 
{
  for (int y = TOP; y >= TOP - barHeight; y--) 
  {
    leds[y * ROW_SIZE + band] = ColorFromPalette(purplePal, y * (255 / (barHeight + 1)));
  }
}

void AudioVisualizer::changingBars(int band, int barHeight) 
{
  for (int y = TOP; y >= TOP - barHeight; y--) 
  {
    leds[y * ROW_SIZE + band] = CHSV(y * (255 / ROW_SIZE) + colorTimer, 255, 255);
  }
}

void AudioVisualizer::centerBars(int band, int barHeight) 
{
  if (barHeight & 1 == 0) 
  {
    barHeight--;
  }
    
  int yStart = ((ROW_SIZE - barHeight) / 2 );
  for (int y = yStart; y <= (yStart + barHeight); y++) 
  {
    int colorIndex = constrain((y - yStart) * (255 / barHeight), 0, 255);
    leds[y * ROW_SIZE + band] = ColorFromPalette(heatPal, colorIndex);
  }
}

void AudioVisualizer::whitePeak(int band) 
{
  int peakHeight = TOP - peak[band] - 1;
  leds[peakHeight * ROW_SIZE + band] = CHSV(0,0,255);
}

void AudioVisualizer::outrunPeak(int band) 
{
  int peakHeight = TOP - peak[band] - 1;
  leds[peakHeight * ROW_SIZE + band] = ColorFromPalette(outrunPal, peakHeight * (255 / ROW_SIZE));
}

void AudioVisualizer::resetBands()
{
  for (int i = 0; i < ROW_SIZE; i++)
  {
    bandValues[i] = 0;
  }
}

void AudioVisualizer::decayPeak()
{
  EVERY_N_MILLISECONDS(60) 
  {
    for (byte band = 0; band < ROW_SIZE; band++)
    {
      if (peak[band] > 0)
      {
        peak[band] -= 1;
      }
    }
      
    colorTimer++;
  }
}

void AudioVisualizer::incrementColorTimer()
{
  EVERY_N_MILLISECONDS(10) 
  {
    colorTimer++;
  }
}

void AudioVisualizer::cycleModes()
{
  EVERY_N_SECONDS(10) 
  {
    if (autoChangePatterns) 
    {
      _mode = (eModes)((_mode + 1) % Num_Modes);
    }
  }
}
