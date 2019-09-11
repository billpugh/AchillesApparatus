#ifndef BLINKY_TAPE_H
#define BLINKY_TAPE_H

#include <FastLED.h>

#define LED_COUNT 240   // Number of LEDs connected to the board. This is also the maximum number of LEDs that can be controlled via serial


const int BLANK_AT_FRONT=0;
class Pattern {
  public:
    virtual void draw(CRGB * leds);
    virtual void reset();
};

#endif
