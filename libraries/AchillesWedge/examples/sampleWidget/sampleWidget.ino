

#include "Achilles.h"
#include "AchillesLog.h"
#include "AchillesWedge.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    delay(1);
  delay(1000);
  Serial.println("Sample Achilles Appartus widget");
  setupComm(0x71);
  pinMode(3, INPUT_PULLUP);
}


unsigned long lastCommComplaint = 0;
unsigned long lastStatusReport = 0;
int lastV = -1;
void loop() {
  unsigned long now = millis();
  if (!commOK() && now > 2000 &&  now - 2000 > lastCommComplaint) {
    lastCommComplaint = now;
    logf("Comm not working at %d\n", now);
  }
  if (commOK() && now > 5000 &&  now - 5000 > lastStatusReport) {
    lastStatusReport = now;
    logf("System mode: %s\n", systemModeName(getSystemMode()));
    logf("Daytime : %s\n", daytimeName(getDaytime()));
    logf("Light level : %d\n", getLightLevel());
  }

  int v = digitalRead(3);

  if (v != lastV) {
    setPointTo(0,v);
    localActivitySeen();
    if (v) {
      Serial.println("Turned on");
      playSound(1, false);
    } else
      Serial.println("Turned off");
    lastV = v;
  }

  delay(300);


}
