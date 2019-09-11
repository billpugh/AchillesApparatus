#include <Adafruit_SleepyDog.h>
#include "Achilles.h"
#include "AchillesLog.h"
#include "animations.h"
#include<FastLED.h>
#include <i2c_t3.h>

int led = 13;
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  aalog("Tower brain");
  aalogf("Compiled %s, %s\n", F(__DATE__), F(__TIME__));
  int countdownMS = Watchdog.enable(14000);
  aalogf("Enabled the watchdog with max countdown of %d ms\n", countdownMS);
  setupAnimations();
  aalogf("number of opc files: %d\n", numberOfOPCFiles);
}



void loop() {
  Watchdog.reset();
  unsigned long now = millis();
  int halfSections = (now/500);
  digitalWrite(led, halfSections % 2 == 1);
  
  updateAnimation(now);

  currentAnimation->update(now);
  copyLEDs();
  LEDS.show();
  delay(33);


}
