#include "Helpers.h"
#include "Constants.h"

int Helpers::convertIndex(int i)
{
  return i % UNIT_DIM + ( i / UNIT_DIM ) * LEDS_PER_UNIT - ( i / ROW_SIZE ) * ( UNITS_PER_ROW * LEDS_PER_UNIT ) + ((UNIT_DIM * ( i / ROW_SIZE )) % LEDS_PER_UNIT) + (i / (LEDS_PER_UNIT * UNITS_PER_ROW)) * (LEDS_PER_UNIT * UNITS_PER_ROW);
}
