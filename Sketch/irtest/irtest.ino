#include <IRremote.h>

#define IR_PIN 4
#define INPUT_OK 0xFF38C7
#define ONBOARD_LED  2

IRrecv irRecv(IR_PIN);
decode_results results;

bool isOn = false;

void setup() 
{
  irRecv.enableIRIn();
  pinMode(ONBOARD_LED,OUTPUT);
}

void loop()
{
  if(irRecv.decode(&results))
  {
    if(results.value == INPUT_OK)
    {
      isOn = !isOn;

      digitalWrite(ONBOARD_LED,isOn ? HIGH : LOW);
    }
  }   

  delay(100);
}
