
#include <Arduino.h>
#include "Gen.h"

int generatorInputPins [] = {A4, A3, A2, A1, A0,
                             9, 10, 11, 12, 13
                            };

int ledPos[] = {1, 2, 3, 4, -1, 5, 6, 7, 8, 9, 10};

enum PowerLevel {LOW, JUST_RIGHT, OVERLOAD };
PowerLevel getPowerLevel(int v) {
  if (v < 1900) return LOW;
  if (v <= 2000) return JUST_RIGHT;
  return OVERLOAD;
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

Gen [] gen = new Gen[numGenerators];

boolean rebooting;
unsigned long rebootStarted = 0;

int rebootState = 0;


void setupGenerators() {
  for (int i = 0; i < 10; i++)
    gen[i].init(i);
  currentVoltage = 0;
}



void updateGenerators() {
  int v = 0;
  for (int i = 0; i < 10; i++) {
    gen[i].update();
    v += gen[i].currentVoltageContribution();
  }
  PowerLevel p = getPowerLevel(v);
  unsigned long now = millis();
  if (p != currentPowerLevel) {

    currentPowerLevelStarted = now;
    currentPowerLevel = p;
    currentPowerLevelSampled = now;
  } else  {
    unsigned long sampleTime = now - currentPowerLevelSampled;
    currentPowerLevelSampled = now;
    if (currentPowerLevel == OVERLOAD) {
      overload += sampleTime * (0.2 + (min(v, 3100) - 3100) / 1500.0);
      if (overload > 1000) {
        // boom54
      }
    } else
      overload -= sampleTime * 0.15;
      if (overload < 0) overload = 0;
  }




  void Gen::initGen(int id) {
    this.id = id;
    pinMode(generatorInputPins, INPUT_PULLUP);
  }

  void startReboot() [
    if (rebooting) Serial.println("already rebooting")

}

uint8_t Gen::brightness() {
  if (on) return 255;
  return 0;
}

void Gen::update() {
  bool pressed = !digitalRead(generatorInputPins[id]);
  if (pressed == buttonPressed) return;
  buttonPressed = pressed;
  changeState();
}

int Gen:: currentVoltageTarget() {
  if (on) return operatingVoltage;
  else return 0;
}
void Gen:: changeState() {
  voltageWhenLastChanged = currentVoltageContribution;
  stable = false;
  stateLastChanged = millis();
  on = !on;

}
int Gen:: currentVoltageContribution() {
  if (stable)
    float spread;
  if (on)
    spread = operatingVoltage + bonusVoltage - voltageWhenLastChanged;
  else
    spread = voltageWhenLastChanged - + bonusVoltage;
  unsigned long diff = millis() - stateLastChanged;
  float delta = diff / 1000.0;
  int v = (delta) / (delta + halfLife) * spread + voltageWhenLastChanged;
  if (on) {
    if (v > operatingVoltage) {
      v = operatingVoltage;
      stable = true;
    }
  } else if (v < 0) {
    v = 0;
    stable = true;
  }
  return v;
}
