#include "Achilles.h"
#include "AchillesWedge.h"
#include "AchillesLog.h"
#include "FastLED.h"


#define LAMP_PIN         3
#define LED_PIN          2
#define NUM_LEDS         4
#define SPARKLE_RATE   100
#define LAMP_MAX       128
#define LAMP_DELAY      20

CRGB leds[NUM_LEDS];
bool lampStatus = false;
SystemMode lastSystemMode = NOT_RECEIVED;
unsigned long lastCommComplaint = 0;
unsigned long lastStatusReport = 0;

void setup() {
  // turn off the lamp
  lampOff();

  // turn off internal pullups on I2C pins
  pinMode(SDA, INPUT);
  pinMode(SCL, INPUT);
  
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    delay(1);
  delay(1000);
  Serial.println("Hexaplexor Crystals Achilles Appartus widget");
  setupComm(0x72);
  
  randomSeed(analogRead(0));
  FastLED.addLeds<WS2812, LED_PIN>(leds, NUM_LEDS);
  
  for (byte i=0; i<NUM_LEDS; ++i) {
    leds[i] = CRGB(64, 64, 64);
  }
  FastLED.show();
}

void lampOff() {
  lampStatus = false;
  analogWrite(LAMP_PIN, 0);
}

void lampOn() {
  for (byte i=0; i<LAMP_MAX; ++i) {
    analogWrite(LAMP_PIN, i);
    delay(LAMP_DELAY);
  }
  analogWrite(LAMP_PIN, LAMP_MAX);
  lampStatus = true;
}

void loop() {
  byte i;
  byte value;
  byte activeCrystal;
  SystemMode systemMode;
  unsigned long now = millis();
  
  if (!commOK() && now > 2000 &&  now - 2000 > lastCommComplaint) {
    lastCommComplaint = now;
    Serial.print("Comm not working at ");
    Serial.println(now);
  }
  if (commOK() && now > 5000 &&  now - 5000 > lastStatusReport) {
    lastStatusReport = now;
  }
  
  systemMode = getSystemMode();

  if (systemMode == DISCHARGING) {
    if (lampStatus) {
      lampOff();
    }
    
    if (random(6) == 0) {
      leds[random(NUM_LEDS)] = CHSV(random(255), 255, 128);
    }
    
    for (i=0; i<NUM_LEDS; ++i) {
      leds[i].fadeToBlackBy(64);
    }

    FastLED.show();
    FastLED.delay(30);
    
  } else if (systemMode == RESET) {
    if (lampStatus) {
      lampOff();
    }
    
    for (i=0; i<NUM_LEDS; ++i) {
      leds[i].fadeToBlackBy(64);
    }

    FastLED.show();
    FastLED.delay(30);

  // in all other system states, flicker LEDs, turn lamp on
  } else {
    if (!lampStatus) {
      lampOn();
    }
    activeCrystal = random(NUM_LEDS);
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
    FastLED.delay(30);   
  }

  lastSystemMode = systemMode;
}
