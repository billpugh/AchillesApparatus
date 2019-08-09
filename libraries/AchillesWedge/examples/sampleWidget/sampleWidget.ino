

#include "Achilles.h"
#include "AchillesLog.h"
#include "AchillesWedge.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    delay(1);
  delay(1000);
  Serial.println("Sample Achilles Appartus widget");
  setupComm(0x71);
  pinMode(3, INPUT_PULLUP);
}


int lastV = -1;
unsigned long lastMessageReported = 0;
void loop() {
  if (receivedMsg && lastMessageReported != lastMsgReceivedAt) {
    logf("Received message at %d\n", lastMsgReceivedAt);
    lastMessageReported = lastMsgReceivedAt;
  }
  int v = digitalRead(3);
  if (v != lastV)
    Serial.println(v);
  fromWidgetData.pointsActivated = v;
  lastV = v;

  delay(300);
  // put your main code here, to run repeatedly:

}
