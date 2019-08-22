

#include <Arduino.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <RGBDigitZero.h>
#include "math.h"

const uint8_t PROGMEM gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};


RGBDigit digits(4, 6);


Adafruit_NeoPixel buttons(NUMPIXELS, A5, NEO_RGB + NEO_KHZ800);


uint16_t meterColor = 0x808080;

#include "AchillesLog.h"
void setupLEDs() {
  buttons.begin();
  buttons.show();
  digits.begin();
  digits.clearAll();
}

uint8_t translateColorForMeterBacklight(int c) {
  return constrain( c / 2 + 64, 64, 255);
}
uint8_t myGamma(uint8_t v) {
  int total = v;
  total += gamma8[v];
  return total / 2;
}
void setMeter(int voltage) {
  int r, g, b;
  if (voltage < 10) {
    r = g = 0;
    b = 250;
  } else if (voltage < 500) {
    r = g = map(voltage, 0, 500, 0, 100);
    b  = map(voltage, 0, 500, 200, 100);
  } else if (voltage < 1900) {
    r = map(voltage, 500, 1900, 100, 150);
    g =  map(voltage, 500, 1900, 100, 150);
    b =  map(voltage, 500, 1900, 100, 120);
  } else if (voltage < 1970) {
    r = map(voltage, 1900, 1970, 100, 32);
    g = map(voltage, 1900, 1970, 200, 255);
    b = map(voltage, 1900, 1970, 80, 0);
  } else if (voltage < 1990) {
    r = map(voltage, 1970, 1990, 0, 70);
    g = map(voltage, 1970, 1990, 255, 255);
    b = map(voltage, 1970, 1990, 0, 0);
  } else if (voltage < 2100) {
    r = map(voltage, 1990, 2100, 70, 255);
    g = map(voltage, 1990, 2100, 255, 0);
    b = 0;
  } else {
    r = 255;
    g = b = 0;
    int hz = 2;
    float t = millis() * 3.14159 / 1000.0 * hz;
    float s = sin(t);
    int degrees = (t / 3.14159 * 180);
    degrees = degrees % 360;
    aalogf("%3d ", degrees);
    Serial.print(s);
    Serial.print(" ");

    if (s < 0) {
      float drop = s * (voltage - 2100) * 3.0 / 9.0; // range up to 200;
      Serial.print(drop);
      Serial.print(" ");
      Serial.print(r);
      r = r + drop;
      Serial.print(" ");
      Serial.print(r);

      r = constrain(r, 0, 255);
      Serial.print(" ");
      Serial.print(r);
    }
    Serial.println();

  }
  int rawR = r;
  r = myGamma(r);
  g = myGamma(g);
  b = myGamma(b);
  int v = voltage;
  int place = 1000;
  bool sawNonzero = false;
  aalogf("RGB: %4d %4d %4d %4d %4d\n", voltage, rawR * 10, r * 10, g * 10, b * 10);
  for (int digit = 0; digit <= 3; digit++) {
    int thisPosition = v / place;
    v = v - thisPosition * place;
    place /= 10;
    if (thisPosition != 0 || sawNonzero) {
      digits.setDigit(thisPosition, digit, r, g, b);
      sawNonzero = true;
    } else
      digits.clearDigit(digit);
  }
  meterColor = translateColorForMeterBacklight(r) << 16
               + translateColorForMeterBacklight(g) << 8
               + translateColorForMeterBacklight(b);
}

void showText(const char * text, int msDelay, int r, int g, int b) {
  int length = strlen(text);
  int pos = 0;
  if (length < 4)
    pos = length - 4; // will be negative.
  while (true) {
    for (int digit = 0; digit < 4; digit++) {
      int sIndex = pos + digit;
      if (sIndex < 0) digits.clearDigit(digit);
      else digits.setDigit(text[sIndex], digit, r, g, b);
    }
    delay(msDelay);
    if (pos + 3 == length - 1) {
      // have shown last character
      return;
    }
    pos++;
  }
}


unsigned long nextErrorStateAt = 0;
int errorState;


void startError() {
  errorStart = millis();
  errorState = 1;
}

void endError() {
  errorState = 0;
}


void showString(const char *s, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < 4; i++)
    digits.setDigit(s[i], i, r, g, b);
}
void showErr() {
  unsigned long now = millis();
  if (now <= nextErrorStateAt) return;


  switch (errorState) {
    case 1: digits.clearAll();
      nextErrorStateAt = now + 1000;
      break;
    case 2:
      showString("----", 255, 0, 0);
      nextErrorStateAt = now + 1000;
      break;
    case 3:
      showString(" err", 255, 0, 0);
      nextErrorStateAt = now + 1000;
      break;
    case 4:
      showString("boot", 255, 0, 0);
      nextErrorStateAt = now + 1000;
      break;
    case 26:    digits.clearAll();
      nextErrorStateAt = now + 200;
      break;
    case 27:
      endError();
      break;

    default:
      int dotsToShow = (errorState - 5) % 5;
      digits.clearAll();
      int b = myGamma(200);
      for (int i = 0; i < dotsToShow; i++)
        digits.showDot(k, b, b, b);
      nextErrorStateAt = now + 300;
      break;
  }
  errorState ++;
}
