
#include <Arduino.h>
#include "AchillesCentral.h"
#include "AchillesLog.h"
#include <Wire.h>
#include "RTClib.h"

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pointPixels(8 * 6, 3, NEO_GRB + NEO_KHZ800);

RTC_DS3231 rtc;

boolean rtcWorking = true;

void checkWedges() {
  for (int i = 0; i < numWedges; i++) {
    WedgeData& w = wedges[i];
    logf("Checking %s at %02x\n", w.name, w.address);
    Wire.beginTransmission(w.address);
    int error = Wire.endTransmission();
    if (error == 0)
      log(" found\n");
    else {
      logf(" got error %d\n", error);
      //w.responsive = false;
    }

  }
}


void initializeCentral() {
  pointPixels.begin();

  for (int i = 0; i < 8; i++)
    pointPixels.setPixelColor(i, 0x00ff00);
  pointPixels.show();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    rtcWorking = false;
  }
  DateTime now = rtc.now();
  logf("Time is %02d:%02d:%02d\n", now.hour(), now.minute(), now.second());
  checkWedges();
  for (int i = 0; i < 8; i++)
    pointPixels.setPixelColor(i, 0);
  pointPixels.show();
}


ToWidgetData centralData;


int div30(int x) {
  return (x + 3000) / 30 - 100;
}

void updateCentralData(SystemMode systemMode) {
  unsigned long ms = millis();
  centralData.packetNum++;
  centralData.minutesSinceGlobalBoot = ms / 1000 / 60;
  unsigned long secondsSinceActivity = (ms - lastActivity) / 1000;
  centralData.secondsSinceGlobalActivity = min(secondsSinceActivity, 65000);
  centralData. systemMode = systemMode;

  if (rtcWorking) {
    DateTime now = rtc.now();
    int totalMinutes = now.hour() * 60 + now.minute();
    const int sunRise = 6 * 60 + 20;
    const int sunSet = 19 * 60 +  38;
    int period;
    int lightLevel;
    if (now.hour() < 12) {
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
    centralData. sunlight = (Sunlight) period;
    centralData.lightLevel = lightLevel;
    Serial.print("sunlight: ");
    Serial.println(centralData.sunlight);
    Serial.print("light level: ");
    Serial.println(lightLevel);



    centralData.hours = now.hour();
    centralData.minutes = now.minute();
    centralData.seconds  = now.minute();
  } else {
    centralData.sunlight = UNKNOWN_SUNLIGHT;
    centralData.lightLevel = -2;
    centralData.hours = centralData.minutes = centralData.seconds = 0;

  }
}


void scanWedges(SystemMode systemMode) {
  totalPoints = 0;
  uint16_t lastPacketNum = centralData.packetNum;
  updateCentralData(systemMode);
  for (int i = 0; i < numWedges; i++) {
    WedgeData& w = wedges[i];
    logf("Wedge %s at %02x\n", w.name, w.address);
    if (!w.responsive) {
      log("not responsive, skipping\n");
      continue;
    }
    size_t sendSize = sizeof(ToWidgetData);
    centralData.packetAck = w.data.packetNum;
    logf("Writing package %d, last ack %d, size %d\n",
         centralData.packetNum,  centralData.packetAck, sendSize);

    Wire.beginTransmission(w.address);
    int written = Wire.write((uint8_t *)&centralData, sendSize);
    int error = Wire.endTransmission();
    logf("Wrote %d bytes, expecting %d\n", written, sendSize);
    if (error != 0) {
      logf("Got error %d writing to %s\n", error, w.name);
      continue;
    }

    int bytesRead = Wire.requestFrom(w.address, sizeof(w.data));
    if (bytesRead !=  sizeof(w.data)) {
      logf("Got %d rather than %d bytes when reading from %s\n", bytesRead, sizeof(FromWidgetData),
           w.name);
      continue;
    }
    Wire.readBytes((uint8_t*)&w.data, sizeof(w.data)); // copy Rx data to databuf
    if (w.data.packetAck != lastPacketNum)
      logf(" got ack of %d rather than %d from %s\n",
           w.data.packetAck, lastPacketNum, w.name);
    if (w.position >= 0)
      for (int p = 0; p < 8; p++)
        if ( w.data.pointsActivated & (1 << p)) {
          logf(" point %d set for %s\n", p, w.name);
          pointPixels.setPixelColor(w.position * 8 + p, 0x00ff00);
          totalPoints++;
        } else {
          pointPixels.setPixelColor(w.position * 8 + p, 0);
        }
  }
  pointPixels.show();
}
