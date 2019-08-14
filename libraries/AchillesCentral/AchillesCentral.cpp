
#include <Arduino.h>
#include "AchillesCentral.h"
#include "AchillesLog.h"
#include "sound.h"
#include <Wire.h>
#include "RTClib.h"

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pointPixels(8 * 6, 3, NEO_GRB + NEO_KHZ800);

RTC_DS3231 rtc;

boolean rtcWorking = true;

void checkWedges() {
  for (int i = 0; i < numWedges; i++) {
    WedgeData& w = wedges[i];
    aalogf("Checking %s at %02x\n", w.name, w.address);
    Wire.beginTransmission(w.address);
    int error = Wire.endTransmission();
    if (error == 0)
      aalog(" found\n");
    else {
      aalogf(" got error %d\n", error);
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
  aalogf("Time is %02d:%02d:%02d\n", now.hour(), now.minute(), now.second());
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
#ifdef  ACHILLES_PACKET_DEBUG
  centralData.packetNum++;
#endif
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
    centralData.daytime = (Daytime) period;
    centralData.lightLevel = lightLevel;
    Serial.print("sunlight: ");
    Serial.println(centralData.daytime);
    Serial.print("light level: ");
    Serial.println(lightLevel);




  } else {
    centralData.daytime = UNKNOWN_SUNLIGHT;
    centralData.lightLevel = -2;


  }
}

unsigned long nextScanReport = 1000;

void scanWedges(SystemMode systemMode) {
  unsigned long microNow = micros();
  int totalPointSum = 0;
  updateCentralData(systemMode);
  int scannedWedges = 0;
  for (int i = 0; i < numWedges; i++) {
    WedgeData& w = wedges[i];
    aalogf("Wedge %s at %02x\n", w.name, w.address);
    if (!w.responsive) {
      aalog("not responsive, skipping\n");
      continue;
    }
    size_t sendSize = sizeof(ToWidgetData);
#ifdef  ACHILLES_PACKET_DEBUG
    centralData.packetAck = w.data.packetNum;
    aalogf("Writing package %d, last ack %d, size %d\n",
         centralData.packetNum,  centralData.packetAck, sendSize);
#endif

    Wire.beginTransmission(w.address);
    int written = Wire.write((uint8_t *)&centralData, sendSize);
    int error = Wire.endTransmission();
    if (written !=  sendSize)
      aalogf("Wrote %d bytes, expecting %d\n", written, sendSize);
    if (error != 0) {
      aalogf("Got error %d writing to %s\n", error, w.name);
      continue;
    }

    int bytesRead = Wire.requestFrom(w.address, sizeof(w.data));
    if (bytesRead !=  sizeof(w.data)) {
      aalogf("Got %d rather than %d bytes when reading from %s\n", bytesRead, sizeof(FromWidgetData),
           w.name);
      continue;
    }
    bytesRead = Wire.readBytes((uint8_t*)&w.data, sizeof(w.data)); // copy Rx data to databuf
    if (bytesRead !=  sizeof(w.data)) {
      aalogf("Got %d rather than %d bytes when calling readBytes on %s\n", bytesRead, sizeof(FromWidgetData),
           w.name);
      continue;
    }
#ifdef  ACHILLES_PACKET_DEBUG
    if (w.data.packetAck !=  centralData.packetNum)
      aalogf(" got ack of %d rather than %d from %s\n",
           w.data.packetAck,  centralData.packetNum, w.name);
#endif
    if (w.position >= 0)
      for (int p = 0; p < 8; p++)
        if ( w.data.pointsActivated & (1 << p)) {
          aalogf(" point %d : %d set for %s\n", w.position, p, w.name);
          pointPixels.setPixelColor(w.position * 8 + p, 0x00ff00);
          totalPointSum++;
        } else {
          pointPixels.setPixelColor(w.position * 8 + p, 0);
        }
    if (w.data.playThisTrack != 0)
      playSound(w);
    scannedWedges++;
  }
  unsigned long microDuration = micros() - microNow;
  unsigned long now = millis();
  if (now > nextScanReport) {
    aalogf("Scanned %d out of %d wedges, requiring %d microseconds\n",
         scannedWedges, numWedges, microDuration);
    nextScanReport = now + 5000;
  }
  pointPixels.show();

  totalPoints = totalPointSum;

}
