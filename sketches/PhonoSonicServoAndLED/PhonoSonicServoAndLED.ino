#include <Adafruit_NeoPixel_ZeroDMA.h>

Adafruit_NeoPixel_ZeroDMA strip(1, 4, NEO_GRB);

void setup() {
  strip.begin();
  strip.show();

}

void loop() {
  strip.setPixelColor(0, 255, 0, 0);
  strip.show();
  delay(500);
  strip.setPixelColor(0, 0, 255, 0);
  strip.show();
  delay(500);
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();
  delay(500);
  strip.setPixelColor(0, 0, 0, 0);
  strip.show();
  delay(500);
 
}
