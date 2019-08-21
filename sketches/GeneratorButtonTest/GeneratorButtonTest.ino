

int generatorInputPins [] = {A4, A3, A2, A1, A0,
                             9, 10, 11, 12, 13
                            };


int ledPos[] = {1, 2, 3, 4, -1, 5, 6, 7, 8, 9, 10};

bool state[10];
bool wasPressed[10];
#include "AchillesLog.h"
#include <Adafruit_NeoPixel.h>
#define PIN        A5

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 10 // Popular NeoPixel ring size


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);


void setup() {
  Serial.begin(115200);
  while (!Serial & millis() < 3000) delay(10);
  for (int i = 0; i < 10; i++) {
    pinMode(generatorInputPins[i], INPUT_PULLUP);
    state[i] = wasPressed[i] = false;
  }
  pixels.begin();
}

void loop() {
  for (int i = 0; i < 10; i++) {
    bool pressed = !digitalRead(generatorInputPins[i]);
    if (pressed == wasPressed[i]) continue;
    wasPressed[i] = pressed;
    if (ledPos[i] < 0) {
      aalog("Generator %2d disabled\n");
    } else {
      state[i] = !state[i];

      if (state[i])
        aalogf("Generator %2d turned on\n", i);
      else
        aalogf("Generator %2d turned off\n", i);
      pixels.setPixelColor(ledPos[i], state[i] ? 0x800000 : 0);
    }
    pixels.show();
  }
  delay(50);

}
