#include "AchillesLog.h"
#include "pinkNoise.h"
#include "energyTubes.h"
#include "hallSensors.h"
#include "soundEffects.h"




void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("PhotoSonic demo");
  logf("Compiled %s, %s\n", F(__DATE__), F(__TIME__));
  log("initializing tubes");
  initializeTubes();
  log("initializing sound effects");
  initializeSoundEffects();

}


void loop() {
  Serial.println(millis());
  measure();
  updateSoundEffects();
  delay(40);
}
