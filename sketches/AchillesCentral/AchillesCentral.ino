#include <Wire.h>


#include "AchillesCentral.h"
#include "AchillesLog.h"
#include "time.h"
#include "sound.h"

WedgeData wedges[] = {

  WedgeData("PhonoSonic", 0x71, 0),
  WedgeData("Hexaplexor",  0x72, 0, 1),
  WedgeData("HexaplexorHelper",  0x73, -1),
  WedgeData("TileMaze",  0x74, 2),
  WedgeData("Generator",  0x74, 3)
};

const size_t numWedges = sizeof(wedges) / sizeof(WedgeData);
unsigned long lastActivity;
int totalPoints;
int lastPin;

void setup() {
  Serial.begin(115200);
  while (!Serial & millis() < 3000) delay(10);

  aalogf("Achilles Central, compiled %s, %s\n",
         F(__DATE__), F(__TIME__));
  //  int countdownMS = Watchdog.enable(14000);
  //  logf("Enabled the watchdog with max countdown of %d ms\n", countdownMS);
  initializeCentral();
  lastPin = digitalRead(1);
  //initializeClock();
  //initializeSound();
}

void loop() {
  // Watchdog.reset();
  lastActivity = millis();
  updateClock();
  scanWedges(centralData.systemMode);
  updateSound();
  delay(50);
}
