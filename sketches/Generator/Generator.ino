

#include "leds.h"
#include "Gen.h"
#include "AchillesLog.h"

void setup() {


  Serial.begin(115200);
  while (!Serial & millis() < 3000) delay(10);
  Serial.println("starting");
  setupLEDs();
  setupGenerators();
  pinMode(5, OUTPUT);
  delay(1000);

}


void setAnalogMeter(int volts) {
  int v = volts * 255 / 3080;
  int v2 = constrain(v, 0, 255);
  analogWrite(5, v2);
}

int voltage = 0;
void loop() {
  updateGenerators();
  delay(50);
}
