
#include <Arduino.h>

#include <Tsunami.h>
#include "pinkNoise.h"
#include "hallSensors.h"
#include "AchillesLog.h"

Tsunami tsunami(Serial1);

const int OUTPUT_CHANNEL = 0;

char gTsunamiVersion[VERSION_STRING_LEN];    // Tsunami version string


int dbBoost(int dbDecrease) {
  float v = 2 - pow(10.0, dbDecrease / 20.0);
  v = 20.0 * log10(v);
  return (int) (v + 0.5);
}

void tsunamiDelay(int ms) {
  unsigned long done = millis() + ms;
  do {
    tsunami.update();
    delay(1);

  } while (millis() < done);

}


void tsunamiReset(Tsunami &t) {
  t.start();
  delay(10);

  int num = t.getNumTracks();
  Serial.print("num tracks: ");
  Serial.println(num);

  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  t.stopAllTracks();
  for (int i = 0; i < TSUNAMI_NUM_OUTPUTS; i++) {
    t.samplerateOffset(i, 0);
    t.masterGain(i, 0);
  }
}

void initializeSoundEffects() {
  // Tsunami startup at 57600
  tsunamiReset(tsunami);

  tsunami.setReporting(true);
  Serial.println("starting");

  // Allow time for the Tsunami to respond with the version string and
  //  number of tracks.
  delay(100);
  if (tsunami.getVersion(gTsunamiVersion, VERSION_STRING_LEN)) {
    Serial.print(gTsunamiVersion);
    Serial.print("\n");
    int gNumTracks = tsunami.getNumTracks();
    Serial.print("Number of tracks = ");
    Serial.print(gNumTracks);
    Serial.print("\n");
  }
  else
    Serial.println("Didn't get version response");

  for (int t = 1; t <= 4; t++) {

    tsunami.trackLoop(t, true);
    tsunami.trackGain(t, -70);

    tsunami.trackPlayPoly(t, OUTPUT_CHANNEL, true);
  }
  tsunami.trackGain(5, 0);
  tsunami.trackGain(6, 0);
  tsunami.trackLoop(6, true);
}


const int masterV = 2;
int mainTrack = 1;

PinkNoise pink(4);

unsigned long lastStaticAdjustment = 0;



int mainTrackLevel() {
  float p = pink.generate();
  int c = (int) 20 * p;
  int change = c;
  if (change > 8) change = change * 2 - 8;
  change = 8 - change;
  return change;
}

void directionChangeSound() {
  logf("direction changed: %d -> %d %3d\n", prevDirection, direction, rpm);
  if (direction == 0) {
    tsunami.trackFade(mainTrack, -70, 3000, false);
    tsunami.trackFade(3, -70, 6000, false);
  } else {
    tsunami.trackGain(mainTrack, -70);
    if (direction == 1)
      mainTrack = 1;
    else
      mainTrack = 2;
  }
}

int currentSampleRateOffset = 0;


void updateSoundEffects() {
  unsigned long now = millis();
  //logf("direction %d, rpm %d\n", direction, rpm);
  if (direction == 0 || rpm == 0) {
    currentSampleRateOffset = 0;
    return;
  }
  int delayForStaticAdjustment = 6000 / rpm;
  if (false) {
    int targetRateOffset = constrain( (rpm - 33) * 1000, -30000, 30000);
    if (-10000 < targetRateOffset && targetRateOffset < 10000)
      targetRateOffset = 0;
    if (targetRateOffset != currentSampleRateOffset) {
      if (currentSampleRateOffset == 0)
        currentSampleRateOffset = targetRateOffset;
      else if (currentSampleRateOffset < targetRateOffset)
        currentSampleRateOffset = min(currentSampleRateOffset + 500, targetRateOffset);
      else if (currentSampleRateOffset > targetRateOffset)
        currentSampleRateOffset = max(currentSampleRateOffset - 500, targetRateOffset);
      // logf("sample rate %d\n", currentSampleRateOffset);
      tsunami.samplerateOffset(OUTPUT_CHANNEL, currentSampleRateOffset);
    }
  }
  if (now > lastStaticAdjustment +  delayForStaticAdjustment) {
    lastStaticAdjustment = now;

    int lvl = mainTrackLevel();
    if (lvl < 0) {
      tsunami.trackGain(mainTrack, masterV + lvl);
      tsunami.trackGain(3, masterV + dbBoost(lvl));

    } else {
      tsunami.trackGain(mainTrack, masterV + dbBoost(-lvl));
      tsunami.trackGain(3, masterV - lvl);

    }
  }
}

void hotspotShutdownSound() {
  tsunami.trackStop(5);
  tsunami.trackStop(6);
}

void hotspotWinddownSound() {
  tsunami.trackFade(6, -20, hotspotUntilShutdown - hotspotUntilWinddown + 20, false);
}

void hotspotStartSound() {
  tsunami.trackPlayPoly(5, OUTPUT_CHANNEL, false);
  tsunami.trackGain(6, -40);
  tsunami.trackPlayPoly(6, OUTPUT_CHANNEL, false);
  tsunami.trackFade(6, 250, -10, false);
}

void hotspotResumeSound() {
  tsunami.trackFade(6, 200, -10, false);
}
