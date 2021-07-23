#include "FastLED.h"

class SnakeGame
{
  public:
    void update(int input);
    void draw(CRGB* leds);

  private:
    struct Position
    {
      int x;
      int y;
    };
  
    CRGB _bodyColors[3];
    CRGB _headColor;

    Position _snake[200];
};
