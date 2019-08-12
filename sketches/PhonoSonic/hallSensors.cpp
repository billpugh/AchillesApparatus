#include <Arduino.h>
#include "AchillesLog.h"
#include "hallSensors.h"
#include "soundEffects.h"


// A0 - innermost
// A1 - middle
// A2 - outer
// A3 - tone arm

int getReading(int r) {
  int v = analogRead(r) - 325;
  if ( -25 <= v && v <= 25) return 0;
  return v;
}


static inline int8_t sign(int val) {
  if (val < 0) return -1;
  if (val == 0) return 0;
  return 1;
}



unsigned long startPrevSensor = 0;
int prevHallSensor = -1;
int prevHallSign;
unsigned long startCurrSensor = 0;
int currHallSensor = -1;
int currHallSign;
bool wasZero = true;
unsigned long startThreePosSensor = 0;



unsigned long startedHotspot = 0;
unsigned long lastSawHotspot = 0;
boolean hotspotWinddownStarted = false;
boolean hotspotWinddown() {
  return startedHotspot != 0 &&  millis() - lastSawHotspot > hotspotUntilWinddown;
}

boolean hotspotShutdown() {
  return startedHotspot != 0 &&  millis() - lastSawHotspot > hotspotUntilShutdown;
}

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
  int abs3 = abs(h3);

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
      directionChangeSound();
    }

  } else {
    int hallSign = sign(hallValue);
    int hallAbs = abs(hallValue);
    if (hallSensor == 2)
      hallSign = -hallSign;
    const bool detail = true;

    if (abs3 > hallAbs / 3 + 2) {
      if (detail && false) log("tone arm over hot spot\n");

      if (startedHotspot == 0) {
        if (detail) log("Starting hotspot\n");
        startedHotspot =  now;
        hotspotStartSound();

      } else if (hotspotWinddown()) {
        hotspotResumeSound();
        hotspotWinddownStarted = false;
        if (detail) log("Restarting hotspot\n");
      }
      lastSawHotspot = now;
    }
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
        directionChangeSound();
      }
    } else {



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
            directionChangeSound();
          if (detail) logf("%3d ", (1 + currHallSensor)*currHallSign);
          logf("%d %3d ", direction, rpm);
          log("\n");
        }
      }

    }
    wasZero = false;
  }
  if (hotspotShutdown()) {
    log("hotspot shutdown\n");
    startedHotspot = 0;
    hotspotWinddownStarted = false;
    hotspotShutdownSound();
  } else if (hotspotWinddown()) {
    if (!hotspotWinddownStarted) {
      log("hotspot winddown\n");
      hotspotWinddownStarted = true;
      hotspotWinddownSound();
    }
  } else
    hotspotWinddownStarted = false;
}
