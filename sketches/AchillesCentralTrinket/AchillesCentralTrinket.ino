#include <Wire.h>


#include "AchillesCentral.h"
#include "AchillesLog.h"
#include "time.h"
#include "sound.h"

WedgeData wedges[] = {
  //WedgeData("TestWedge", 0x71, 0),
  //  WedgeData("PhonoSonic", 0x71, 0),
  WedgeData("Hexaplexor",  0x72, 0, 1),
  //WedgeData("HexaplexorHelper",  0x73, -1),
  //  WedgeData("TileMaze",  0x74, 4)
};

const size_t numWedges = sizeof(wedges) / sizeof(WedgeData);
unsigned long lastActivity;
int totalPoints;
int lastPin;

void setup() {
  pinMode(1, INPUT_PULLUP);
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
  int nextPin = digitalRead(1);
  if (nextPin == 0 && lastPin == 1) {
    updateCentralData((SystemMode)((centralData.systemMode + 1) % 6));
    Serial.print("Change system mode to ");
    Serial.println(centralData.systemMode);
  }
  lastPin = nextPin;
  
  lastActivity = millis();
  updateClock();
  scanWedges(centralData.systemMode);
  updateSound();
  delay(50);
}
