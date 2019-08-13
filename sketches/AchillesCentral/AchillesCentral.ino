#include <Wire.h>


#include "AchillesCentral.h"
#include "AchillesLog.h"
#include "sound.h"

WedgeData wedges[] = {
  //WedgeData("TestWedge", 0x71, 0),
  //  WedgeData("PhonoSonic", 0x71, 0),
  WedgeData("Hexaplexor",  0x72, 0),
  //WedgeData("HexaplexorHelper",  0x73, -1),
  //  WedgeData("TileMaze",  0x74, 4)
};

const size_t numWedges = sizeof(wedges) / sizeof(WedgeData);
unsigned long lastActivity;
int totalPoints;

void setup() {
  Serial.begin(115200);
  delay(1000);
  logf("Achilles Central, compiled %s, %s\n",
       F(__DATE__), F(__TIME__));
  int countdownMS = Watchdog.enable(14000);
  logf("Enabled the watchdog with max countdown of %d ms\n", countdownMS);
  initializeCentral();
  initializeSound();

}

void loop() {
  Watchdog.reset();
  lastActivity = millis();
  scanWedges(ACTIVE);
  updateSound();
  delay(50);

}
