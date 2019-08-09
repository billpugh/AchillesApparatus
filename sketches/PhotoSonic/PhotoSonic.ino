#include "log.h"
#include "pinkNoise.h"

#include <Tsunami.h>
Tsunami tsunami(Serial1);


const int OUTPUT_CHANNEL = 0;


char gTsunamiVersion[VERSION_STRING_LEN];    // Tsunami version string


static inline int8_t sign(int val) {
  if (val < 0) return -1;
  if (val == 0) return 0;
  return 1;
}


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


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Tsunami demo");
  logf("Compiled %s, %s\n", F(__DATE__), F(__TIME__));

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


}


int getReading(int r) {
  int v = analogRead(r) - 325;
  if ( -25 <= v && v <= 25) return 0;
  return v;
}

unsigned long startPrevSensor = 0;
int prevHallSensor = -1;
int prevHallSign;
unsigned long startCurrSensor = 0;
int currHallSensor = -1;
int currHallSign;
bool wasZero = true;
unsigned long startThreePosSensor = 0;

int currentMaxValue;

void directionChange();

int rpm;
int direction;
int prevDirection;
unsigned long lastRPMUpdate = 0;
void measure() {
  prevDirection = direction;
  int h0 = getReading(A0);
  int h1 = getReading(A1);
  int h2 = getReading(A2);
  int h3 = getReading(A3);
  int abs0 = abs(h0);
  int abs1 = abs(h1);
  int abs2 = abs(h2);

  int hallSensor;
  int hallValue;
  if (abs0 > abs1) {
    if (abs0 > abs2)  {
      hallSensor = 0; hallValue = h0;
    } else {
      hallSensor = 2; hallValue = h2;
    }
  } else if (abs1 > abs2)  {
    hallSensor = 1; hallValue = h1;
  } else {
    hallSensor = 2; hallValue = h2;
  }
  unsigned long now = millis();

  if (hallValue == 0) {
    wasZero = true;
    if (now > lastRPMUpdate + 2000 && direction != 0) {
      log("Haven't seen any RPM updates\n");
      direction = 0;
      rpm = 0;
      directionChange();
    }
    return;
  }
  int hallSign = sign(hallValue);
  int hallAbs = abs(hallValue);
  if (hallSensor == 2)
    hallSign = -hallSign;

  if (currHallSensor == hallSensor && currHallSign == hallSign) {
    // still on the last sensor we were on
    if (wasZero) {
      startCurrSensor = now;
      if (currHallSensor == 2 && currHallSign == 1)
        startThreePosSensor = now;
    }
    if (now > lastRPMUpdate + 2000 && direction != 0) {
      log("Haven't seen any RPM updates\n");
      direction = 0;
      rpm = 0;
      directionChange();
    }
  } else {
    const bool detail = true;
    lastRPMUpdate = now;
    if (currHallSensor == 2 && currHallSign == 1) {
      int diff = now - startThreePosSensor;
      if (diff > 0) {
        logf("\n%3d %3d ", 0, 60000 / diff);
        if (detail) log("\n");
      }
      startThreePosSensor = now;
    }
    // new sensor
    prevHallSensor = currHallSensor;
    prevHallSign = currHallSign;
    startPrevSensor = startCurrSensor;
    currHallSensor = hallSensor;
    currHallSign = hallSign;
    startCurrSensor = now;
    int diff = (startCurrSensor - startPrevSensor);
    if (prevHallSensor != -1 && diff > 50) {

      if (detail) logf("moving from %d,%d -> %d,%d, %d ms\n",
                         prevHallSensor, prevHallSign, currHallSensor, currHallSign, diff);
      if (prevHallSensor != currHallSensor && currHallSign == prevHallSign) {
        logf("Fuck: %d, %d -> %d, %d\n",
             prevHallSensor, prevHallSign, currHallSensor, currHallSign);
         logf("      %3d, %3d, %3d\n",   analogRead(A0) - 325, analogRead(A1) - 325, analogRead(A2) - 325);
      } else {
        // ms to perform one rotation = angle * diff
        // RPM = 60000 / (ms to perform one rotation)
        int sectorDiff = 10;
        if (prevHallSensor != currHallSensor) {
          direction = -(currHallSign - prevHallSign) / 2;

          if (prevHallSensor + currHallSensor == 3)
            sectorDiff = 54;
          else
            sectorDiff = 34;
        }
        else {
          direction = (currHallSign - prevHallSign) / 2;
          switch (currHallSensor) {

            case 0:
              sectorDiff = 17;
              break;
            case 1:
              sectorDiff = 11;
              break;
            case 2:
              sectorDiff = 6;
              break;
          }
        }
        rpm = constrain(60000 * sectorDiff / 160 / diff, 0, 60);
        if (rpm < 3) {

          logf("Moving too slowly\: %3d %4d\n", sectorDiff, diff );
          direction = 0;
          rpm = 0;
        }
        if (prevDirection != direction)
          directionChange();
        if (detail) logf("%3d ", (1 + currHallSensor)*currHallSign);
        logf("%d %3d ", direction, rpm);
        log("\n");
      }
    }

  }
  wasZero = false;
}
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

const int masterV = 2;
int mainTrack = 1;

void directionChange() {
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

void loop() {
  measure();
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
  delay(40);

}
