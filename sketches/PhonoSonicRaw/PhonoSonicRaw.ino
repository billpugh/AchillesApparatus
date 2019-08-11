
#include "AchillesLog.h"


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("PhotoSonic raw data");
  logf("Compiled %s, %s\n", F(__DATE__), F(__TIME__));

}


int getReading(int r) {
  int v = analogRead(r) - 325;
  if ( -25 <= v && v <= 25) return 0;
  return v;
}

void loop() {
  logf("%3d %3d %3d %3d\n", getReading(A0),
       getReading(A1), -  getReading(A2), getReading(A3));

  delay(20);



}
