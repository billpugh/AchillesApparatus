#include <FastLED.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include "BlinkyTape.h"
#define LED_PIN    6
#include "Shimmer.h"
#if not defined (__AVR_ATtiny85__)
#include <Adafruit_SleepyDog.h>
#endif

struct CRGB leds[LED_COUNT];


unsigned long nextAutoAdvance;
const unsigned long autoAdvanceDelay = 5 * 60 * 1000L; /// 5 minutes

uint8_t currentPattern = 0;
uint8_t patternCount = 0;
#define MAX_PATTERNS 10
Pattern* patterns[MAX_PATTERNS];


// Register a pattern
void registerPattern(Pattern* newPattern) {
  // If there is space for this pattern
  if (MAX_PATTERNS <= patternCount) {
    return;
  }

  patterns[patternCount] = newPattern;
  patternCount++;
}

// Change the current pattern
void initializePattern(uint8_t newPattern) {
  // Check if this is a valid pattern
  if (newPattern >= MAX_PATTERNS) {
    return;
  }


  currentPattern = newPattern;
  patterns[currentPattern]->reset();

}

void advancePattern() {
  if (patternCount == 1)
    return;
  initializePattern((currentPattern + 1) % patternCount);
}

// Run one step of the current pattern
void runPattern() {
  patterns[currentPattern]->draw(leds);
}


// Shimmer shimmer0(0);
Shimmer shimmer2(2);

bool welcomePrinted = false;
int countdownMS = 0;
void setup()
{
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
#if not defined (__AVR_ATtiny85__)
  countdownMS = Watchdog.enable(14000);

  Serial.begin(57600);
#endif
  LEDS.addLeds<WS2811, LED_PIN, GRB>(leds, LED_COUNT);

  LEDS.setBrightness(60);
  LEDS.show();
  if (false)  for (int i = 0; i < LED_COUNT; i++) {
      leds[i] = 0x100000;
      LEDS.show();
      leds[i] = 0x00000;
      delay(10);
    }
  nextAutoAdvance = millis() + autoAdvanceDelay;

  registerPattern(&shimmer2);


  // registerPattern(&shimmer0);


  currentPattern = 0;
  initializePattern(currentPattern);
}

void loop() {

#if not defined (__AVR_ATtiny85__)

  Watchdog.reset();
  if (Serial & !welcomePrinted) {

    Serial.print("Enabled the watchdog with max countdown of ");
    Serial.print(countdownMS, DEC);
    Serial.println(" milliseconds!");
    Serial.println();
    Serial.print("Current time ");
    Serial.println(millis());

    welcomePrinted = true;
  }
#endif

  unsigned long halfSeconds = millis() / 500;
  digitalWrite(13, halfSeconds % 2 == 1);
  if (millis() > nextAutoAdvance) {
#if not defined (__AVR_ATtiny85__)
    Serial.print("Advancing pattern @ ");
    Serial.println(millis());
#endif

    advancePattern();
    nextAutoAdvance = millis() + autoAdvanceDelay;
  }

  // Draw the current pattern
  runPattern();


  LEDS.show();
}
