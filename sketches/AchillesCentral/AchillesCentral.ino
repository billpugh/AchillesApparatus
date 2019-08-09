#include <Wire.h>


#include "AchillesCentral.h"
#include "AchillesLog.h"

WedgeData wedges[] = {
  WedgeData("TestWedge", 0x71, 0),
//  WedgeData("PhotoSonic", 0x71, 0),
//  WedgeData("Hexaplexor",  0x72, 2),
//  WedgeData("HexaplexorHelper",  0x73, -1),
//  WedgeData("TileMaze",  0x74, 4)
};

const size_t numWedges = sizeof(wedges) / sizeof(WedgeData);
unsigned long lastActivity;
int totalPoints;

void setup() {
  Serial.begin(115200);
  while (!Serial ) delay(1);
  logf("Achilles Central, compiled %s, %s\n",
       F(__DATE__), F(__TIME__));

  initializeCentral();

}

void loop() {
  
  lastActivity = millis();
   scanWedges(ACTIVE);
   delay(1000);

}
