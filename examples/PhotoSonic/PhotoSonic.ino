#include "log.h"


static inline int8_t sign(int val) {
  if (val < 0) return -1;
  if (val == 0) return 0;
  return 1;
}
void setup() {
  Serial.begin(115200);

}


int getReading(int r) {
  int v = analogRead(r) - 325;
  if ( -20 <= v && v <= 20) return 0;
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

void measure() {
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

  if (hallValue == 0) {
    wasZero = true;
    // nothing to do
    return;
  }
  int hallSign = sign(hallValue);
  int hallAbs = abs(hallValue);
  if (hallSensor == 2)
    hallSign = -hallSign;
  unsigned long now = millis();

  if (currHallSensor == hallSensor && currHallSign == hallSign) {
    // still on the last sensor we were on
    if (wasZero) {
      startCurrSensor = now;
      if (currHallSensor == 2 && currHallSign == 1)
        startThreePosSensor = now;
    }
  } else {
    const bool detail = false;
    if (currHallSensor == 2 && currHallSign == 1) {
      int diff = now - startThreePosSensor;
      if (diff > 0) {
        logf("\n%3d %3d ", 0, 60000 / diff);
        if (detail) log("|n");
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
    if (prevHallSensor != -1 && diff > 0) {

      if (detail) logf("moving from %d,%d -> %d,%d, %d ms\n",
                       prevHallSensor, prevHallSign, currHallSensor, currHallSign, diff);
      // ms to perform one rotation = angle * diff
      // RPM = 60000 / (ms to perform one rotation)

      int sectorDiff = 10;
      if (prevHallSensor != currHallSensor) {
        if (prevHallSensor + currHallSensor == 3)
          sectorDiff = 54;
        else
          sectorDiff = 34;
      }
      else switch (currHallSensor) {
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
      logf("%3d %3d ", (1 + currHallSensor)*currHallSign, 60000 * sectorDiff / 160 / diff);
     if (detail) log("|n");
    }

  }
  wasZero = false;
}

void loop() {
  measure();
  delay(20);
  if (false) {
    Serial.print(getReading(A0)); Serial.print(" ");
    Serial.print(getReading(A1)); Serial.print(" ");
    Serial.print(getReading(A2)); Serial.print(" ");
    Serial.print(getReading(A3)); Serial.print(" ");
    Serial.println();
  }
}
