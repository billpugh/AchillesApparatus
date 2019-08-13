
#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include "AchillesCentral.h"


RTC_DS3231 rtc;

DateTime playaTime;
bool clockRunning = false;

unsigned long lastClockUpdate = 0;
void initializeClock() {
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    return;
  }
  clockRunning = true;
}

void updateClock() {
  DateTime now = rtc.now();
  int hour = now.hour();
  int totalMinutes = now.hour() * 60 + now.minute();
  const int sunRise = 6 * 60 + 20;
  const int sunSet = 19 * 60 + 38;
  int period;
  int lightLevel;
  if (hour < 12) {
    // AM
    Serial.println(totalMinutes - sunRise);
    period = constrain(div30(totalMinutes - sunRise), -2, 1) + 2;
    lightLevel = constrain(div30(totalMinutes - sunRise), -3, 1) - 1;
  } else {
    // PM
    Serial.println(totalMinutes - sunRise);
    period = constrain(div30(totalMinutes - sunSet), -2, 1) + 5;
    if (period == 6) period = 0;
    lightLevel = constrain(div30(sunSet - totalMinutes), -2, 2) - 2;


  }
  centralData.sunlight = (Sunlight) period;
  centralData.lightLevel = lightLevel;
}
