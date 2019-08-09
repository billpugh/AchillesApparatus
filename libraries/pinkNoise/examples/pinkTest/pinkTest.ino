#include "pinkNoise.h"

PinkNoise pink(8);

void setup() {
  Serial.begin(115200);
  while (!Serial ) delay(1);

}

void loop() {
  Serial.println(pink.generate());
  delay(300);

}
