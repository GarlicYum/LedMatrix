#include "SnakeGame.h"
#include "Helpers.h"
#include "Constants.h"

SnakeGame::SnakeGame()
{
  _bodyColors[0] = CRGB(10, 100, 32);
  _bodyColors[1] = CRGB(10, 130, 48);
  _bodyColors[2] = CRGB(18, 160, 48);
  _bodyColors[3] = CRGB(10, 130, 48);

  reset();
}

void SnakeGame::updateSnake(int input)
{
  handleInput(input);
  
  if(++_currentTick >= _ticksBeforeUpdate && _alive)
  {
    _currentTick = 0;
    moveSnake();
    handleApple();
    checkForDeaths();
  }
}

void SnakeGame::draw(CRGB* leds)
{
  leds[Helpers::convertIndex(_applePos.Y * ROW_SIZE + _applePos.X)] = CRGB::Red;
  drawBorder(leds);
  drawSnake(leds);

  FastLED.show();
  FastLED.delay(50);
}

void SnakeGame::handleInput(int input)
{
  switch(input)
  {
    case INPUT_OK:
      if(!_alive)
      {
        reset();
      }
      break;
    case INPUT_LEFT:
      if(_direction.Y != 0)
      {
        _direction = Vec2(-1, 0);
      }
      break;
    case INPUT_RIGHT:
      if(_direction.Y != 0)
      {
        _direction = Vec2(1, 0);
      }
      break;
    case INPUT_UP:
      if(_direction.X != 0)
      {
        _direction = Vec2(0, -1);
      }
      break;
    case INPUT_DOWN:
      if(_direction.X != 0)
      {
        _direction = Vec2(0, 1);
      }
      break;
    default:
      break;
  }
}

void SnakeGame::moveSnake()
{
  Vec2 lastPos(_snake[_length - 1]);

  for(int i = _length - 1; i > 0; i--)
  {
    _snake[i] = _snake[i - 1];
  }

  _snake[0].X += _direction.X;
  _snake[0].Y += _direction.Y;
  
  if(_hasEaten && _length <= MAX_LENGTH)
  {
    _hasEaten = false;
    _snake[_length++] = lastPos;
  }
}

void SnakeGame::drawBorder(CRGB* leds)
{
  for(int x = 0; x < ROW_SIZE; x++)
  {
    int index0 = Helpers::convertIndex(x);
    int index1 = Helpers::convertIndex((ROW_SIZE - 1) * ROW_SIZE + x);
    leds[index0] = leds[index1] = CRGB::Blue;
  }

  for(int y = 1; y < (ROW_SIZE - 1); y++)
  {
    int index0 = Helpers::convertIndex(y * ROW_SIZE);
    int index1 = Helpers::convertIndex(y * ROW_SIZE + (ROW_SIZE - 1));
    leds[index0] = leds[index1] = CRGB::Blue;
  }
}

void SnakeGame::drawSnake(CRGB* leds)
{
  leds[Helpers::convertIndex(_snake[0].Y * ROW_SIZE + _snake[0].X)] = CRGB::Yellow;
  
  for(int i = 1; i < _length; i++)
  {
    leds[Helpers::convertIndex(_snake[i].Y * ROW_SIZE + _snake[i].X)] = _bodyColors[i % N_BODY_COLORS];
  }

  if(!_alive)
  {
    for(int i = 0; i < _deathAnimCounter; i++)
    {
      leds[Helpers::convertIndex(_snake[i].Y * ROW_SIZE + _snake[i].X)] = CRGB::Red;
    }
    
    if(_deathAnimCounter < _length)
    {
      ++_deathAnimCounter;
    }
  }
}

void SnakeGame::handleApple()
{
  if(_snake[0].X == _applePos.X && _snake[0].Y == _applePos.Y)
  {
    _hasEaten = true;
    spawnApple();
  }
}

void SnakeGame::checkForDeaths()
{
  if(_snake[0].X == 0 || _snake[0].X == (ROW_SIZE - 1) || _snake[0].Y == 0 || _snake[0].Y == (ROW_SIZE - 1))
  {
    _alive = false;
  }

  for(int i = 1; i < _length; i++)
  {
    if(_snake[0].X == _snake[i].X && _snake[0].Y == _snake[i].Y)
    {
      _alive = false;
      break;
    }
  }
}

void SnakeGame::reset()
{
  _started = false;
  _alive = true;
  _hasEaten = false;
  _ticksBeforeUpdate = 2;
  _currentTick = 0;
  _length = _deathAnimCounter = 1;
  _direction = Vec2();

  _snake[0] = Vec2(ROW_SIZE / 2, ROW_SIZE / 2);
  
  spawnApple();
}

void SnakeGame::spawnApple()
{
  do
  {
    _applePos.X = random(1, ROW_SIZE - 1);
    _applePos.Y = random(1, ROW_SIZE - 1);
  } while(!applePosIsValid());
}

bool SnakeGame::applePosIsValid() const
{
  for(int i = 0; i < _length; i++)
  {
    if(_applePos.X == _snake[i].X && _applePos.Y == _snake[i].Y)
    {
      return false;
    }
  }
  return true;
}
