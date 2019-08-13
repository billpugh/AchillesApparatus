#include <Adafruit_SleepyDog.h>
#include "Achilles.h"
#include "AchillesLog.h"
#include "animations.h"
#include<FastLED.h>
#include <i2c_t3.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(5);
  log("Tower brain");
  logf("Compiled %s, %s\n", F(__DATE__), F(__TIME__));
  setupAnimations();
  logf("number of opc files: %d\n", numberOfOPCFiles);
  int countdownMS = Watchdog.enable(14000);
  logf("Enabled the watchdog with max countdown of %d ms\n", countdownMS);
}



void loop() {
  Watchdog.reset();
  unsigned long now = millis();
  updateAnimation(now);

  currentAnimation->update(now);
  copyLEDs();
  LEDS.show();
  delay(33);


}
