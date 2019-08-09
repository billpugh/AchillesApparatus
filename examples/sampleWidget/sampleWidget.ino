

#include "log.h"
#include "AchillesWedge.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Sample Achilles Appartus widget");
  setupComm(0x44);
}

void loop() {
  // put your main code here, to run repeatedly:

}
