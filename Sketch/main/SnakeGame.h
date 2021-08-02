#include "FastLED.h"
#define MAX_LENGTH 200
#define N_BODY_COLORS 4

class SnakeGame
{
  public:
    SnakeGame();
    void updateSnake(int input);
    void draw(CRGB* leds);

  private:
    struct Vec2
    {
      Vec2()
      : X(0)
      , Y(0)
      {}

      Vec2(int x, int y)
      : X(x)
      , Y(y)
      {}
      
      int X;
      int Y;
    };

    void reset();
    void spawnApple();
    void handleInput(int input);
    void moveSnake();
    void drawBorder(CRGB* leds);
    void drawSnake(CRGB* leds);
    void handleApple();
    void checkForDeaths();
    bool applePosIsValid() const;
  
    CRGB _bodyColors[N_BODY_COLORS];

    bool _started = false;
    bool _alive = true;
    bool _hasEaten = false;
    int _ticksBeforeUpdate = 4;
    int _currentTick = 0;
    int _length = 1;
    int _deathAnimCounter = 1;
    
    Vec2 _applePos;
    Vec2 _direction;
    Vec2 _snake[MAX_LENGTH];
};
