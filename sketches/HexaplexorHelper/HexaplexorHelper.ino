#include "FastLED.h"

#include "Achilles.h"
#include "AchillesWedge.h"
#include "AchillesLog.h"


#define LAMP_PIN         3
#define LED_PIN          2
#define NUM_LEDS         4
#define SPARKLE_RATE   100
#define LAMP_MAX       128
#define LED_DELAY       30
#define WIN_BLINK_RATE 750

CRGB leds[NUM_LEDS];
bool lampStatus = false;
SystemMode lastSystemMode = NOT_RECEIVED;
unsigned long lastLED = 0;
unsigned long lastCommComplaint = 0;
unsigned long lastStatusReport = 0;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Hexaplexor Helper Achilles Appartus Widget");  
  setupComm(0x73);

  // turn off the lamp
  lampOff();
  
  randomSeed(analogRead(0));
  FastLED.addLeds<WS2812, LED_PIN>(leds, NUM_LEDS);
  
  for (byte i=0; i<NUM_LEDS; ++i) {
    leds[i] = CRGB(64, 64, 64);
  }
  FastLED.show();
  lastLED = millis();
}

void lampOff() {
  lampStatus = false;
  analogWrite(LAMP_PIN, 0);
}

void lampOn() {
  analogWrite(LAMP_PIN, LAMP_MAX);
  lampStatus = true;
}

void loop() {
  byte i;
  byte value;
  byte activeCrystal;
  SystemMode systemMode;
  unsigned long now = millis();
  bool commStatus;
  
  // handle i2c comms
  commStatus = commOK();
  if (!commStatus && now > 2000 &&  now - 2000 > lastCommComplaint) {
    lastCommComplaint = now;
  }
  if (commStatus && now > 5000 &&  now - 5000 > lastStatusReport) {
    lastStatusReport = now;
  }
  
  systemMode = getSystemMode();

  if (systemMode != lastSystemMode) {
    Serial.print("Change SystemMode: ");
    Serial.println(systemMode);
  }

  if (systemMode == DISCHARGING) {
    if (lampStatus) {
      lampOff();
    }
    
    if (random(WIN_BLINK_RATE) == 0) {
      leds[random(NUM_LEDS)] = CHSV(random(255), 255, 128);
    }

    if (now - lastLED > LED_DELAY) {
      for (i=0; i<NUM_LEDS; ++i) {
        leds[i].fadeToBlackBy(64);
      }
  
      FastLED.show();
      lastLED = now;
    }
    
  } else if (systemMode == RESET) {
    if (lampStatus) {
      lampOff();
    }
    
    if (now - lastLED > LED_DELAY) {
      for (i=0; i<NUM_LEDS; ++i) {
        leds[i].fadeToBlackBy(64);
      }
  
      FastLED.show();
      lastLED = now;
    }

  // in all other system states, flicker LEDs, turn lamp on
  } else {
    if (!lampStatus) {
      lampOn();
    }
    activeCrystal = random(NUM_LEDS);

    if (now - lastLED > LED_DELAY) {
      for (i=0; i<NUM_LEDS; ++i) {
        if (i == activeCrystal) {
          if (random(SPARKLE_RATE) == 0) {
            leds[i] = CHSV(random(255), 255, 128);
          } else {
            byte value = random(5) + 50;
            leds[i] = CRGB(value, value, value);
          }
        } else {
          byte value = random(5) + 50;
          leds[i] = CRGB(value, value, value);
        }
      }
      FastLED.show();
      lastLED = now;
    }
  }

  lastSystemMode = systemMode;
}
