

#include "leds.h"
#include "Gen.h"
#include "AchillesLog.h"
#include <Adafruit_SleepyDog.h>

void setup() {


  Serial.begin(115200);
  while (!Serial & millis() < 3000) delay(10);
  Serial.println("starting");
  int countdownMS = Watchdog.enable(4000);
  Serial.print("Enabled the watchdog with max countdown of ");
  Serial.print(countdownMS, DEC);
  Serial.println(" milliseconds!");
  Serial.println();

  setupLEDs();
  setupGenerators();
  pinMode(5, OUTPUT);
  delay(1000);

}


void setAnalogMeter(int volts) {
  int v = volts * 255 / 3080;
  int v2 = constrain(v, 0, 255);
  analogWrite(5, v2);
}

int voltage = 0;
void loop() {
  Serial.println(millis());
  Watchdog.reset();
  updateGenerators();
  delay(50);
}
