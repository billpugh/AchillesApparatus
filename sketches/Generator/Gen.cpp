
#include <Arduino.h>
#include "AchillesLog.h"
#include "Gen.h"
#include "leds.h"

int generatorInputPins [] = {A4, A3, A2, A1, A0,
                             9, 10, 11, 12, 13
                            };

int ledPos[] = {0, 1, 2, 3, 4, 9, 8, 7, 6, 5};


int voltages[][10] = {
  {100, 200, 300, 400, 500, 100, 200, 300, 400, 500},
  {400, 250, 300, 600, 200, 50, 100, 850, 750, 350},
  {400, 250, 300, 600, 200, 50, 100, 850, 750, 350},
  { 50,  100,  150,  200,  250,  300, 350, 750,  850, 2100}
};


int Gen::operatingVoltage() {
  return voltages[1][id];
}


const int bonusVoltage = 175;
const float halfLife = 1;


enum PowerLevel {POWER_LOW, POWER_JUST_RIGHT, POWER_OVERLOAD };
PowerLevel getPowerLevel(int v) {
  if (v < 1900) return POWER_LOW;
  if (v <= 2000) return POWER_JUST_RIGHT;
  return POWER_OVERLOAD;
}

PowerLevel currentPowerLevel;
unsigned long currentPowerLevelStarted = 0;
unsigned long currentPowerLevelSampled = 0;

// Overload is 0..1000; at 1000, reboot
// if current state is overload,
// . overload increases by 200 + (volts-2100)/150 every second
// Gives 5 seconds at less than 3100 volts
// gives 1.25 seconds at 3000 volts
// If 2000 volts or less, decreases at 150 per second
float overload;
const int numGenerators = 10;

Gen*  gen  = new Gen[numGenerators];

boolean rebooting;
unsigned long rebootStarted = 0;

int rebootState = 0;


void setupGenerators() {
  for (int i = 0; i < 10; i++)
    gen[i].init(i);
}



void updateGenerators() {
  int v = 0;
  for (int i = 0; i < 10; i++) {
    gen[i].update();
    int v0 = gen[i].currentVoltageContribution();
    Serial.print(v0);
    Serial.print(" ");
    v += v0;
  }
  Serial.println(v);
  setDigitalMeter(v);
  PowerLevel p = getPowerLevel(v);
  unsigned long now = millis();
  if (p != currentPowerLevel) {

    currentPowerLevelStarted = now;
    currentPowerLevel = p;
    currentPowerLevelSampled = now;
  } else  {
    unsigned long sampleTime = now - currentPowerLevelSampled;
    currentPowerLevelSampled = now;
    if (currentPowerLevel == POWER_OVERLOAD) {
      overload += sampleTime * (0.2 + (min(v, 3100) - 3100) / 1500.0);
      if (overload > 1000) {
        // boom54
      }
    } else
      overload -= sampleTime * 0.15;
    if (overload < 0) overload = 0;
  }
}




void Gen::init(int id) {
  this->id = id;
  pinMode(generatorInputPins[id], INPUT_PULLUP);
}

void startReboot() {
  if (rebooting)
    Serial.println("already rebooting");

}

uint8_t Gen::brightness() {
  if (on) return 255;
  return 0;
}

void Gen::update() {
  bool pressed = !digitalRead(generatorInputPins[id]);
  if (pressed == buttonPressed) return;
  unsigned long now = millis();
  if (lastChange + 10 > now) return;
  lastChange = now;

  buttonPressed = pressed;
  if (pressed)
    changeState();
}

int Gen:: currentVoltageTarget() {
  if (on) return operatingVoltage();
  else return 0;
}
void Gen:: changeState() {
  voltageWhenLastChanged = currentVoltageContribution();
  stable = false;
  stateLastChanged = millis();
  on = !on;

}
int Gen:: currentVoltageContribution() {
  int oVoltage = operatingVoltage();

  if (stable)
    if (on)
      return oVoltage;
    else
      return 0;

  int bVoltage = bonusVoltage();

  int spread;
  if (on)
    spread = oVoltage + bVoltage - voltageWhenLastChanged;
  else
    spread = -(voltageWhenLastChanged + bVoltage);
  unsigned long diff = millis() - stateLastChanged;
  float delta = diff / 1000.0;
  int v = (delta) / (delta + halfLife) * spread + voltageWhenLastChanged;
  aalogf("%2d %5d %5d %5d %5d\n", id,
         voltageWhenLastChanged, spread, diff, v);
  if (on) {
    if (v > oVoltage) {
      v = oVoltage;
      stable = true;
    }
  } else if (v < 0) {
    v = 0;
    stable = true;
  }
  return v;
}
