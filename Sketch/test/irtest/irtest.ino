#include <IRremote.h>

#define DATA_PIN 5

IRrecv irRecv(DATA_PIN);
decode_results results;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irRecv.enableIRIn();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(irRecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    irRecv.resume();
  }

  delay(100);
}
