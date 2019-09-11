#include "AchillesLog.h"

void setup() {
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  doit(0);

}


void doit(int volts) {
  int v = volts * 255/ 3080;
 // int v = volts * 7,140 110*30/28;
  int v2 = constrain(v, 0, 255);
  aalogf("%4d %3d %3d\n", volts, v, v2);
  analogWrite(5, v2);
  delay(2000);
}
void loop() {
  doit(0);
  doit(1000);
  doit(1900);
  doit(2000);

  doit(2500);
  doit(3000);
}
